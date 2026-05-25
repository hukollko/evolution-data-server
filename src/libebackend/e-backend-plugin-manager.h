#ifndef E_BACKEND_PLUGIN_MANAGER_H
#define E_BACKEND_PLUGIN_MANAGER_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * EBackendPluginKind:
 * @E_BACKEND_PLUGIN_KIND_UNKNOWN: Unknown or invalid plugin kind.
 * @E_BACKEND_PLUGIN_KIND_ADDRESS_BOOK: Address book backend plugin.
 * @E_BACKEND_PLUGIN_KIND_CALENDAR: Calendar backend plugin.
 * @E_BACKEND_PLUGIN_KIND_MAIL: Mail backend plugin.
 * @E_BACKEND_PLUGIN_KIND_SOURCE_REGISTRY: Source registry/service plugin.
 * @E_BACKEND_PLUGIN_KIND_OTHER: Other backend-related plugin.
 *
 * Describes the data/backend area supported by a backend plugin.
 */
typedef enum {
E_BACKEND_PLUGIN_KIND_UNKNOWN = 0,
E_BACKEND_PLUGIN_KIND_ADDRESS_BOOK,
E_BACKEND_PLUGIN_KIND_CALENDAR,
E_BACKEND_PLUGIN_KIND_MAIL,
E_BACKEND_PLUGIN_KIND_SOURCE_REGISTRY,
E_BACKEND_PLUGIN_KIND_OTHER
} EBackendPluginKind;

/**
 * EBackendPluginMetadata:
 * @name: Stable plugin name.
 * @version: Plugin version string.
 * @kind: Backend/data type supported by the plugin.
 * @description: Human-readable plugin description.
 * @dependencies: NULL-terminated list of plugin names this plugin depends on.
 *
 * Static metadata exposed by a backend plugin.
 */
typedef struct _EBackendPluginMetadata {
const gchar *name;
const gchar *version;
EBackendPluginKind kind;
const gchar *description;
const gchar * const *dependencies;
} EBackendPluginMetadata;

typedef enum {
E_BACKEND_PLUGIN_MANAGER_ERROR_INVALID_METADATA,
E_BACKEND_PLUGIN_MANAGER_ERROR_MISSING_DEPENDENCY,
E_BACKEND_PLUGIN_MANAGER_ERROR_DEPENDENCY_CYCLE
} EBackendPluginManagerError;

#define E_BACKEND_PLUGIN_MANAGER_ERROR (e_backend_plugin_manager_error_quark ())

GQuarke_backend_plugin_manager_error_quark(void);

const gchar *e_backend_plugin_kind_to_string(EBackendPluginKind kind);

gbooleane_backend_plugin_metadata_is_valid(const EBackendPluginMetadata *metadata,
 GError                     **error);

G_END_DECLS

#endif /* E_BACKEND_PLUGIN_MANAGER_H */
