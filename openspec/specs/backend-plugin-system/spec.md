## ADDED Requirements

### Requirement: Backend plugin metadata

The system SHALL define a backend plugin metadata model containing at least plugin name, version, supported data type, dependencies, and description.

#### Scenario: Valid plugin metadata is provided

- GIVEN a plugin exposes all required metadata fields
- WHEN the plugin manager inspects the plugin
- THEN the plugin metadata is accepted

#### Scenario: Required metadata is missing

- GIVEN a plugin does not expose required metadata fields
- WHEN the plugin manager validates the plugin
- THEN the plugin is rejected or skipped with a diagnostic message

### Requirement: Plugin discovery and lifecycle

The system SHALL support discovery, initialization, and deinitialization of backend plugins through a shared manager API.

#### Scenario: Enabled plugin is discovered

- GIVEN a plugin exists in the configured plugin directory
- AND the plugin is enabled
- WHEN the relevant service starts
- THEN the plugin manager discovers and initializes the plugin

#### Scenario: Service shuts down

- GIVEN a plugin was initialized
- WHEN the service shuts down
- THEN the plugin manager deinitializes the plugin safely

### Requirement: Dependency handling

The system SHALL handle dependencies between plugins.

#### Scenario: Dependency is available

- GIVEN plugin B depends on plugin A
- AND plugin A is available and enabled
- WHEN plugins are initialized
- THEN plugin A is initialized before plugin B

#### Scenario: Dependency is missing

- GIVEN plugin B depends on plugin A
- AND plugin A is missing or disabled
- WHEN plugins are initialized
- THEN plugin B is skipped without breaking unrelated plugins

### Requirement: GSettings enable and disable control

The system SHALL provide additive GSettings-based configuration for enabling and disabling plugins.

#### Scenario: Plugin is disabled

- GIVEN a plugin is listed as disabled in settings
- WHEN plugin discovery runs
- THEN the plugin is skipped

#### Scenario: No explicit disable setting exists

- GIVEN no setting disables a plugin
- WHEN plugin discovery runs
- THEN existing behavior remains compatible and permissive

### Requirement: Existing backend compatibility

The system SHALL not break existing backend loading behavior for current address book, calendar, and Camel providers.

#### Scenario: Existing backends remain available

- GIVEN existing CardDAV, CalDAV, IMAP, POP, and local backends
- WHEN the plugin manager feature is present
- THEN existing backend/provider resolution paths continue to work unchanged
