#include "e-backend-plugin-manager.h"

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
                                    GError                     **error)
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
