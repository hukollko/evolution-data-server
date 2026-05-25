## Overview

The safest implementation is a minimal plugin-manager layer in `src/libebackend` that demonstrates the required concepts while reusing existing Evolution Data Server module infrastructure.

The existing project already has several dynamic mechanisms:

- address book and calendar backends are loaded as modules;
- source registry modules are loaded by service/module infrastructure;
- Camel providers use their own `.urls` and provider module system.

Because these systems are production-used and internally different, this change must not force full unification in one step.

## Chosen approach

Use the minimal Variant A strategy from the exploration stage.

Architecture:

- `src/libebackend/`
  - backend plugin metadata
  - backend plugin manager
  - dependency ordering
  - enable/disable filtering
  - lifecycle calls
- demo/source-registry-style plugin

This demonstrates the lab requirements without touching fragile existing backend resolution paths.

## Plugin metadata

Each plugin should expose metadata through a small public structure/API:

- plugin name;
- version;
- supported data type;
- dependencies;
- description.

The first implementation can keep metadata simple and static.

## Plugin lifecycle

The lifecycle should support:

1. discovery;
2. metadata validation;
3. enabled/disabled filtering;
4. dependency ordering;
5. initialization;
6. safe shutdown/deinitialization.

Plugins with missing dependencies should be skipped with a warning instead of breaking the whole service.

## GSettings

Additive GSettings keys should be used for enable/disable control.

The default behavior should be permissive: existing behavior remains enabled unless a plugin is explicitly disabled.

## Integration strategy

Initial integration should use Source Registry/module-style loading or a demo module under `src/modules/`.

Out of scope for the first implementation:

- rewriting address book backend factory loading;
- rewriting calendar backend factory loading;
- rewriting Camel provider loading;
- changing CardDAV, CalDAV, IMAP, POP, or local behavior;
- promising true runtime hot unload.

## Compatibility

The implementation must preserve:

- backend factory registration behavior;
- module filename and type-name contracts used by subprocess factories;
- Camel provider `.urls` behavior;
- existing service startup behavior.

## Testing strategy

Focused tests should cover:

- valid plugin metadata;
- missing required metadata;
- dependency ordering;
- unresolved dependency skip;
- disabled plugin skip;
- enabled plugin load;
- compatibility smoke checks for existing backend/provider paths.
