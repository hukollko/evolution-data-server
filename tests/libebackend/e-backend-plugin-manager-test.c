/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include <gio/gio.h>

#include <libebackend/libebackend.h>

typedef struct _InitRecorder {
	GPtrArray *order;
	const gchar *name;
	gboolean *flag;
} InitRecorder;

static gboolean
test_init_record (gpointer user_data,
		  GError **error)
{
	InitRecorder *recorder = user_data;

	(void) error;

	if (recorder->flag)
		*recorder->flag = TRUE;

	if (recorder->order)
		g_ptr_array_add (recorder->order, (gpointer) recorder->name);

	return TRUE;
}

static void
test_shutdown_noop (gpointer user_data)
{
	(void) user_data;
}

static gboolean
ensure_gsettings_schema (void)
{
	GSettingsSchemaSource *source;
	GSettingsSchema *schema;

	source = g_settings_schema_source_get_default ();

	if (!source)
		return FALSE;

	schema = g_settings_schema_source_lookup (source, "org.gnome.evolution-data-server", FALSE);

	if (schema)
		g_settings_schema_unref (schema);

	return schema != NULL;
}

static void
set_plugin_settings (const gchar * const *enabled,
		     const gchar * const *disabled)
{
	GSettings *settings;

	settings = g_settings_new ("org.gnome.evolution-data-server");

	g_settings_set_strv (settings, "enabled-backend-plugins", enabled);
	g_settings_set_strv (settings, "disabled-backend-plugins", disabled);
	g_object_unref (settings);
}

static void
test_metadata_validation (void)
{
	EBackendPluginMetadata valid = {
		"metadata-valid",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"valid metadata",
		NULL
	};
	EBackendPluginMetadata invalid = {
		NULL,
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"invalid metadata",
		NULL
	};
	GError *error = NULL;

	g_assert_true (e_backend_plugin_metadata_is_valid (&valid, &error));
	g_assert_no_error (error);

	g_assert_false (e_backend_plugin_metadata_is_valid (&invalid, &error));
	g_assert_error (error, E_BACKEND_PLUGIN_MANAGER_ERROR, E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA);
	g_clear_error (&error);
}

static void
test_dependency_ordering (void)
{
	EBackendPluginManager *manager;
	GPtrArray *order;
	InitRecorder plugin_a;
	InitRecorder plugin_b;
	const gchar *deps_b[] = { "plugin-a", NULL };
	EBackendPluginMetadata metadata_a = {
		"plugin-a",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"dependency root",
		NULL
	};
	EBackendPluginMetadata metadata_b = {
		"plugin-b",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"depends on plugin-a",
		deps_b
	};

	if (!ensure_gsettings_schema ()) {
		g_test_skip ("GSettings schema org.gnome.evolution-data-server is not available");
		return;
	}

	set_plugin_settings (NULL, NULL);

	manager = e_backend_plugin_manager_new ();
	order = g_ptr_array_new ();

	plugin_a.order = order;
	plugin_a.name = "plugin-a";
	plugin_a.flag = NULL;

	plugin_b.order = order;
	plugin_b.name = "plugin-b";
	plugin_b.flag = NULL;

	g_assert_true (e_backend_plugin_manager_add (manager, &metadata_b, test_init_record, test_shutdown_noop, &plugin_b, NULL));
	g_assert_true (e_backend_plugin_manager_add (manager, &metadata_a, test_init_record, test_shutdown_noop, &plugin_a, NULL));
	g_assert_true (e_backend_plugin_manager_initialize (manager, NULL));

	g_assert_cmpuint (order->len, ==, 2);
	g_assert_cmpstr (g_ptr_array_index (order, 0), ==, "plugin-a");
	g_assert_cmpstr (g_ptr_array_index (order, 1), ==, "plugin-b");

	g_ptr_array_unref (order);
	e_backend_plugin_manager_free (manager);
}

static void
test_unresolved_dependency_skip (void)
{
	EBackendPluginManager *manager;
	gboolean independent_initialized = FALSE;
	InitRecorder independent = { NULL, "independent", &independent_initialized };
	const gchar *deps_missing[] = { "missing-plugin", NULL };
	EBackendPluginMetadata with_missing_dep = {
		"needs-missing",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"missing dependency",
		deps_missing
	};
	EBackendPluginMetadata independent_meta = {
		"independent",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"independent plugin",
		NULL
	};
	GError *error = NULL;

	if (!ensure_gsettings_schema ()) {
		g_test_skip ("GSettings schema org.gnome.evolution-data-server is not available");
		return;
	}

	set_plugin_settings (NULL, NULL);

	manager = e_backend_plugin_manager_new ();

	g_assert_true (e_backend_plugin_manager_add (manager, &with_missing_dep, test_init_record, test_shutdown_noop, NULL, NULL));
	g_assert_true (e_backend_plugin_manager_add (manager, &independent_meta, test_init_record, test_shutdown_noop, &independent, NULL));

	g_assert_false (e_backend_plugin_manager_initialize (manager, &error));
	g_assert_error (error, E_BACKEND_PLUGIN_MANAGER_ERROR, E_BACKEND_PLUGIN_MANAGER_ERROR_MISSING_DEPENDENCY);
	g_assert_true (independent_initialized);

	g_clear_error (&error);
	e_backend_plugin_manager_free (manager);
}

static void
test_enabled_disabled_behavior (void)
{
	EBackendPluginManager *manager;
	gboolean init_enabled = FALSE;
	gboolean init_disabled = FALSE;
	gboolean init_filtered_out = FALSE;
	InitRecorder enabled = { NULL, "plugin-enabled", &init_enabled };
	InitRecorder disabled = { NULL, "plugin-disabled", &init_disabled };
	InitRecorder filtered_out = { NULL, "plugin-filtered-out", &init_filtered_out };
	const gchar *enabled_plugins[] = { "plugin-enabled", "plugin-disabled", NULL };
	const gchar *disabled_plugins[] = { "plugin-disabled", NULL };
	EBackendPluginMetadata metadata_enabled = {
		"plugin-enabled",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"enabled plugin",
		NULL
	};
	EBackendPluginMetadata metadata_disabled = {
		"plugin-disabled",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"disabled plugin",
		NULL
	};
	EBackendPluginMetadata metadata_filtered = {
		"plugin-filtered-out",
		"1.0",
		E_BACKEND_PLUGIN_KIND_OTHER,
		"not in enabled list",
		NULL
	};

	if (!ensure_gsettings_schema ()) {
		g_test_skip ("GSettings schema org.gnome.evolution-data-server is not available");
		return;
	}

	set_plugin_settings (enabled_plugins, disabled_plugins);

	manager = e_backend_plugin_manager_new ();

	g_assert_true (e_backend_plugin_manager_add (manager, &metadata_enabled, test_init_record, test_shutdown_noop, &enabled, NULL));
	g_assert_true (e_backend_plugin_manager_add (manager, &metadata_disabled, test_init_record, test_shutdown_noop, &disabled, NULL));
	g_assert_true (e_backend_plugin_manager_add (manager, &metadata_filtered, test_init_record, test_shutdown_noop, &filtered_out, NULL));
	g_assert_true (e_backend_plugin_manager_initialize (manager, NULL));

	g_assert_true (init_enabled);
	g_assert_false (init_disabled);
	g_assert_false (init_filtered_out);

	e_backend_plugin_manager_free (manager);

	set_plugin_settings (NULL, NULL);
}

gint
main (gint argc,
      gchar **argv)
{
	g_setenv ("GSETTINGS_BACKEND", "memory", TRUE);

	g_test_init (&argc, &argv, NULL);

	g_test_add_func ("/libebackend/plugin-manager/metadata-validation", test_metadata_validation);
	g_test_add_func ("/libebackend/plugin-manager/dependency-ordering", test_dependency_ordering);
	g_test_add_func ("/libebackend/plugin-manager/unresolved-dependency", test_unresolved_dependency_skip);
	g_test_add_func ("/libebackend/plugin-manager/enabled-disabled", test_enabled_disabled_behavior);

	return g_test_run ();
}
