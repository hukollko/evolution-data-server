/*
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "evolution-data-server-config.h"

#include <libebackend/libebackend.h>

/* Module Entry Points */
void e_module_load (GTypeModule *type_module);
void e_module_unload (GTypeModule *type_module);

static EBackendPluginManager *demo_manager = NULL;
static gboolean demo_initialized = FALSE;

static gboolean
backend_plugin_demo_init (gpointer user_data,
			  GError **error)
{
	(void) user_data;
	(void) error;

	demo_initialized = TRUE;
	g_message ("backend-plugin-demo initialized");

	return TRUE;
}

static void
backend_plugin_demo_shutdown (gpointer user_data)
{
	(void) user_data;

	demo_initialized = FALSE;
	g_message ("backend-plugin-demo shut down");
}

G_MODULE_EXPORT void
e_module_load (GTypeModule *type_module)
{
	EBackendPluginMetadata metadata = {
		"backend-plugin-demo",
		"0.1",
		E_BACKEND_PLUGIN_KIND_SOURCE_REGISTRY,
		"Demo backend plugin for lab5 plugin manager",
		NULL
	};
	GError *error = NULL;

	(void) type_module;

	demo_manager = e_backend_plugin_manager_new ();

	if (!e_backend_plugin_manager_add (
		demo_manager,
		&metadata,
		backend_plugin_demo_init,
		backend_plugin_demo_shutdown,
		NULL,
		&error)) {
		g_warning ("Failed to register backend-plugin-demo: %s", error ? error->message : "unknown error");
		g_clear_error (&error);
		return;
	}

	if (!e_backend_plugin_manager_initialize (demo_manager, &error)) {
		g_warning ("Failed to initialize backend-plugin-demo: %s", error ? error->message : "unknown error");
		g_clear_error (&error);
	}
}

G_MODULE_EXPORT void
e_module_unload (GTypeModule *type_module)
{
	(void) type_module;

	if (demo_manager)
		e_backend_plugin_manager_free (demo_manager);

	demo_manager = NULL;
	demo_initialized = FALSE;
}
