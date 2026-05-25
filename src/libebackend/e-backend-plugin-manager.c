#include "e-backend-plugin-manager.h"

typedef struct _EBackendPluginEntry {
	EBackendPluginMetadata metadata;
	gchar **dependencies;
	EBackendPluginInitFunc init_func;
	EBackendPluginShutdownFunc shutdown_func;
	gpointer user_data;
	gboolean initialized;
	gboolean visiting;
} EBackendPluginEntry;

struct _EBackendPluginManager {
	GPtrArray *entries;
	GPtrArray *initialized_entries;
};

static gchar **
backend_plugin_dependencies_dup (const gchar * const *dependencies)
{
	guint ii;
	guint length = 0;
	gchar **copy;

	if (!dependencies)
		return NULL;

	while (dependencies[length])
		length++;

	copy = g_new0 (gchar *, length + 1);

	for (ii = 0; ii < length; ii++)
		copy[ii] = g_strdup (dependencies[ii]);

	return copy;
}

static void
backend_plugin_entry_free (gpointer data)
{
	EBackendPluginEntry *entry = data;

	if (!entry)
		return;

	g_free ((gchar *) entry->metadata.name);
	g_free ((gchar *) entry->metadata.version);
	g_free ((gchar *) entry->metadata.description);
	g_strfreev (entry->dependencies);
	g_free (entry);
}

static EBackendPluginEntry *
backend_plugin_manager_find_entry (EBackendPluginManager *manager,
                                   const gchar *name)
{
	guint ii;

	g_return_val_if_fail (manager != NULL, NULL);
	g_return_val_if_fail (name != NULL, NULL);

	for (ii = 0; ii < manager->entries->len; ii++) {
		EBackendPluginEntry *entry = g_ptr_array_index (manager->entries, ii);

		if (g_strcmp0 (entry->metadata.name, name) == 0)
			return entry;
	}

	return NULL;
}

GQuark
e_backend_plugin_manager_error_quark (void)
{
	return g_quark_from_static_string ("e-backend-plugin-manager-error-quark");
}

const gchar *
e_backend_plugin_kind_to_string (EBackendPluginKind kind)
{
	switch (kind) {
	case E_BACKEND_PLUGIN_KIND_ADDRESS_BOOK:
		return "address-book";
	case E_BACKEND_PLUGIN_KIND_CALENDAR:
		return "calendar";
	case E_BACKEND_PLUGIN_KIND_MAIL:
		return "mail";
	case E_BACKEND_PLUGIN_KIND_SOURCE_REGISTRY:
		return "source-registry";
	case E_BACKEND_PLUGIN_KIND_OTHER:
		return "other";
	case E_BACKEND_PLUGIN_KIND_UNKNOWN:
	default:
		return "unknown";
	}
}

gboolean
e_backend_plugin_metadata_is_valid (const EBackendPluginMetadata *metadata,
                                    GError **error)
{
	if (!metadata) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
			"Plugin metadata is missing");
		return FALSE;
	}

	if (!metadata->name || !*metadata->name) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
			"Plugin metadata does not define a plugin name");
		return FALSE;
	}

	if (!metadata->version || !*metadata->version) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
			"Plugin '%s' does not define a version",
			metadata->name);
		return FALSE;
	}

	if (metadata->kind <= E_BACKEND_PLUGIN_KIND_UNKNOWN ||
	    metadata->kind > E_BACKEND_PLUGIN_KIND_OTHER) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
			"Plugin '%s' has invalid plugin kind",
			metadata->name);
		return FALSE;
	}

	return TRUE;
}

EBackendPluginManager *
e_backend_plugin_manager_new (void)
{
	EBackendPluginManager *manager;

	manager = g_new0 (EBackendPluginManager, 1);
	manager->entries = g_ptr_array_new_with_free_func (backend_plugin_entry_free);
	manager->initialized_entries = g_ptr_array_new ();

	return manager;
}

void
e_backend_plugin_manager_free (EBackendPluginManager *manager)
{
	if (!manager)
		return;

	e_backend_plugin_manager_shutdown (manager);

	g_ptr_array_unref (manager->initialized_entries);
	g_ptr_array_unref (manager->entries);
	g_free (manager);
}

gboolean
e_backend_plugin_manager_add (EBackendPluginManager *manager,
                              const EBackendPluginMetadata *metadata,
                              EBackendPluginInitFunc init_func,
                              EBackendPluginShutdownFunc shutdown_func,
                              gpointer user_data,
                              GError **error)
{
	EBackendPluginEntry *entry;

	g_return_val_if_fail (manager != NULL, FALSE);

	if (!e_backend_plugin_metadata_is_valid (metadata, error))
		return FALSE;

	if (backend_plugin_manager_find_entry (manager, metadata->name)) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
			"Plugin '%s' is already registered",
			metadata->name);
		return FALSE;
	}

	entry = g_new0 (EBackendPluginEntry, 1);
	entry->metadata.name = g_strdup (metadata->name);
	entry->metadata.version = g_strdup (metadata->version);
	entry->metadata.kind = metadata->kind;
	entry->metadata.description = g_strdup (metadata->description);
	entry->dependencies = backend_plugin_dependencies_dup (metadata->dependencies);
	entry->metadata.dependencies = (const gchar * const *) entry->dependencies;
	entry->init_func = init_func;
	entry->shutdown_func = shutdown_func;
	entry->user_data = user_data;

	g_ptr_array_add (manager->entries, entry);

	return TRUE;
}

static gboolean
backend_plugin_manager_initialize_entry (EBackendPluginManager *manager,
                                         EBackendPluginEntry *entry,
                                         GError **error)
{
	guint ii;

	if (entry->initialized)
		return TRUE;

	if (entry->visiting) {
		g_set_error (
			error,
			E_BACKEND_PLUGIN_MANAGER_ERROR,
			E_BACKEND_PLUGIN_MANAGER_ERROR_DEPENDENCY_CYCLE,
			"Dependency cycle detected at plugin '%s'",
			entry->metadata.name);
		return FALSE;
	}

	entry->visiting = TRUE;

	for (ii = 0; entry->dependencies && entry->dependencies[ii]; ii++) {
		EBackendPluginEntry *dependency;

		dependency = backend_plugin_manager_find_entry (manager, entry->dependencies[ii]);

		if (!dependency) {
			entry->visiting = FALSE;
			g_set_error (
				error,
				E_BACKEND_PLUGIN_MANAGER_ERROR,
				E_BACKEND_PLUGIN_MANAGER_ERROR_MISSING_DEPENDENCY,
				"Plugin '%s' depends on missing plugin '%s'",
				entry->metadata.name,
				entry->dependencies[ii]);
			return FALSE;
		}

		if (!backend_plugin_manager_initialize_entry (manager, dependency, error)) {
			entry->visiting = FALSE;
			return FALSE;
		}
	}

	entry->visiting = FALSE;

	if (entry->init_func && !entry->init_func (entry->user_data, error))
		return FALSE;

	entry->initialized = TRUE;
	g_ptr_array_add (manager->initialized_entries, entry);

	return TRUE;
}

gboolean
e_backend_plugin_manager_initialize (EBackendPluginManager *manager,
                                     GError **error)
{
	guint ii;
	gboolean success = TRUE;

	g_return_val_if_fail (manager != NULL, FALSE);

	for (ii = 0; ii < manager->entries->len; ii++) {
		EBackendPluginEntry *entry = g_ptr_array_index (manager->entries, ii);
		GError *local_error = NULL;

		if (!backend_plugin_manager_initialize_entry (manager, entry, &local_error)) {
			success = FALSE;

			if (error && !*error)
				g_propagate_error (error, local_error);
			else
				g_clear_error (&local_error);
		}
	}

	return success;
}

void
e_backend_plugin_manager_shutdown (EBackendPluginManager *manager)
{
	gint ii;

	if (!manager)
		return;

	for (ii = manager->initialized_entries->len - 1; ii >= 0; ii--) {
		EBackendPluginEntry *entry;

		entry = g_ptr_array_index (manager->initialized_entries, ii);

		if (entry->initialized && entry->shutdown_func)
			entry->shutdown_func (entry->user_data);

		entry->initialized = FALSE;
	}

	g_ptr_array_set_size (manager->initialized_entries, 0);
}
