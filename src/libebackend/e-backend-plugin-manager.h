#if !defined (__LIBEBACKEND_H_INSIDE__) && !defined (LIBEBACKEND_COMPILATION)
#error "Only <libebackend/libebackend.h> should be included directly."
#endif

#ifndef E_BACKEND_PLUGIN_MANAGER_H
#define E_BACKEND_PLUGIN_MANAGER_H

#include <glib.h>

G_BEGIN_DECLS

typedef enum {
	E_BACKEND_PLUGIN_KIND_UNKNOWN = 0,
	E_BACKEND_PLUGIN_KIND_ADDRESS_BOOK,
	E_BACKEND_PLUGIN_KIND_CALENDAR,
	E_BACKEND_PLUGIN_KIND_MAIL,
	E_BACKEND_PLUGIN_KIND_SOURCE_REGISTRY,
	E_BACKEND_PLUGIN_KIND_OTHER
} EBackendPluginKind;

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

typedef struct _EBackendPluginManager EBackendPluginManager;

typedef gboolean (*EBackendPluginInitFunc)	(gpointer user_data,
					 GError **error);
typedef void (*EBackendPluginShutdownFunc)	(gpointer user_data);

#define E_BACKEND_PLUGIN_MANAGER_ERROR \
	(e_backend_plugin_manager_error_quark ())

GQuark		e_backend_plugin_manager_error_quark
					(void) G_GNUC_CONST;
const gchar *	e_backend_plugin_kind_to_string	(EBackendPluginKind kind);
gboolean	e_backend_plugin_metadata_is_valid	(const EBackendPluginMetadata *metadata,
					 GError **error);
EBackendPluginManager *
		e_backend_plugin_manager_new		(void);
void		e_backend_plugin_manager_free		(EBackendPluginManager *manager);
gboolean	e_backend_plugin_manager_add		(EBackendPluginManager *manager,
					 const EBackendPluginMetadata *metadata,
					 EBackendPluginInitFunc init_func,
					 EBackendPluginShutdownFunc shutdown_func,
					 gpointer user_data,
					 GError **error);
gboolean	e_backend_plugin_manager_initialize	(EBackendPluginManager *manager,
					 GError **error);
void		e_backend_plugin_manager_shutdown	(EBackendPluginManager *manager);

G_END_DECLS

#endif /* E_BACKEND_PLUGIN_MANAGER_H */
