# Backend Plugin System (Lab 5)

This document describes the minimal demonstrable backend plugin system added for the lab.

## Scope

- Added a shared plugin manager API in `src/libebackend`.
- Added a demo module in `src/modules/backend-plugin-demo`.
- Kept existing CardDAV, CalDAV, IMAP, POP, local, and Camel provider resolution paths unchanged.

## Plugin API

Public API is declared in `src/libebackend/e-backend-plugin-manager.h`.

### Metadata fields

`EBackendPluginMetadata` contains:

- `name`: stable plugin name (required)
- `version`: plugin version string (required)
- `kind`: `EBackendPluginKind` (required)
- `description`: human-readable description (optional)
- `dependencies`: `NULL`-terminated plugin names list (optional)

Validation entry point:

- `e_backend_plugin_metadata_is_valid()`

### Lifecycle

Manager lifecycle:

1. Create manager: `e_backend_plugin_manager_new()`
2. Register plugins: `e_backend_plugin_manager_add()`
3. Initialize plugins: `e_backend_plugin_manager_initialize()`
4. Shutdown/free: `e_backend_plugin_manager_shutdown()` and `e_backend_plugin_manager_free()`

Plugin callbacks:

- init: `EBackendPluginInitFunc`
- shutdown: `EBackendPluginShutdownFunc`

### Dependency behavior

- Dependencies are initialized before dependents.
- Missing dependency causes the dependent plugin to fail initialization.
- Other unrelated plugins still continue to initialize.
- Dependency cycles are detected and reported.

## GSettings enable/disable control

Schema: `org.gnome.evolution-data-server`

Keys:

- `enabled-backend-plugins` (`as`)
- `disabled-backend-plugins` (`as`)

Behavior:

- If `enabled-backend-plugins` is empty, default is permissive (plugins enabled).
- `disabled-backend-plugins` always takes precedence.
- If a dependency is disabled, dependent plugin is treated as having an unavailable dependency.

## Demo plugin

Location:

- `src/modules/backend-plugin-demo/module-backend-plugin-demo.c`

What it demonstrates:

- plugin metadata declaration
- plugin registration through the manager
- init callback with log message
- shutdown callback with log message

The demo plugin does not call external network APIs.

## Build and install demo plugin

Example in Linux/WSL:

```bash
cmake -S . -B _build -G Ninja
cmake --build _build
cmake --install _build
```

Installed module name:

- `module-backend-plugin-demo` in module directory configured by `moduledir`.

## Verification commands

Focused checks (Linux/WSL preferred):

```bash
ctest --test-dir _build -R e-backend-plugin-manager-test --output-on-failure
ctest --test-dir _build -R e-source-registry-test --output-on-failure
```

## Windows limitation note

Native Windows build/test environment for this repository is not expected to be reliable.
If tests cannot be executed locally on Windows, run the above commands in WSL/Linux.

## Limitations and future work

- Discovery from arbitrary plugin directories is not yet implemented in this minimal step.
- Runtime hot reload/unload is not provided.
- Integration is intentionally minimal and does not rewrite existing backend/provider loading paths.
- Future work can add richer diagnostics and dedicated integration tests around module discovery.
