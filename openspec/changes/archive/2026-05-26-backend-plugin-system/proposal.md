## Why

Evolution Data Server already has dynamic loading mechanisms for address book backends, calendar backends, source registry modules, and Camel providers, but they are not exposed as one minimal, documented plugin layer with metadata, dependency handling, lifecycle control, and enable/disable configuration.

This change introduces a safe, demonstrable backend plugin system for the lab without rewriting existing CardDAV, CalDAV, IMAP, POP, or local backend resolution paths.

## What Changes

- Add a small generic plugin metadata and manager layer in `src/libebackend`.
- Support discovery of plugin-like modules from a configured directory.
- Support plugin metadata: name, version, supported data type, dependencies, and description.
- Add lifecycle handling for discovery, validation, initialization, skip, and shutdown/deinitialization.
- Add dependency ordering and safe skipping of plugins with unresolved dependencies.
- Add additive GSettings keys for enabling and disabling plugins.
- Add a mock/demo plugin module for verification.
- Add documentation describing the plugin API and demo plugin.
- Add focused tests or verification steps for metadata, dependency handling, and enabled/disabled plugin behavior.

No breaking changes are intended.

## Capabilities

### New Capabilities

- `backend-plugin-system`: Minimal extensible plugin layer for backend-related modules, including metadata, discovery, lifecycle, dependency handling, and enable/disable configuration.

### Modified Capabilities

None.

## Impact

Affected areas:

- `src/libebackend/` for the shared plugin manager and metadata API.
- `src/modules/` for a mock/demo plugin module.
- `src/services/evolution-source-registry/` or existing module loading integration points for the initial demonstration.
- `data/org.gnome.evolution-data-server.gschema.xml.in` for additive GSettings configuration.
- `docs/` or `docs/lab5/` for plugin API documentation.
- `tests/` for focused verification.

Compatibility constraints:

- Do not change existing address book, calendar, or Camel provider resolution behavior.
- Do not rewrite existing backend factory hash keys or module filename/type contracts.
- Do not require full runtime hot unload.
- Existing CardDAV, CalDAV, IMAP, POP, and local backends must continue to work unchanged.
