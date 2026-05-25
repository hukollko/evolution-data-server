## 1. Research and preparation

- [x] Read the lab task in `docs/lab5/backend-plugin-system-task.md`.
- [x] Inspect existing module and backend loading mechanisms.
- [x] Select the safe minimal Variant A strategy.

## 2. Core API

- [x] Add plugin metadata definitions in `src/libebackend`.
- [x] Add plugin manager API in `src/libebackend`.
- [x] Add validation for required metadata fields.
- [x] Add dependency ordering and unresolved dependency handling.
- [x] Add lifecycle calls for initialization and shutdown.

## 3. Configuration

- [x] Add additive GSettings keys for enabled/disabled plugin names.
- [x] Make defaults compatible with existing behavior.
- [x] Ensure disabled plugins are skipped without affecting existing modules.

## 4. Demo integration

- [x] Add a mock/demo plugin module.
- [x] Integrate the demo with an existing safe module-loading path.
- [x] Avoid changing CardDAV, CalDAV, IMAP, POP, and local backend resolution paths.

## 5. Tests and verification

- [x] Add focused tests for metadata validation.
- [x] Add tests for dependency ordering.
- [x] Add tests for unresolved dependency skip behavior.
- [x] Add tests or a verification scenario for enabled/disabled plugin behavior.
- [x] Run the narrowest relevant test commands.
- [x] Document any tests that cannot be run locally on Windows.

## 6. Documentation

- [x] Document the plugin API.
- [x] Document metadata fields.
- [x] Document lifecycle functions.
- [x] Document how to build and install a demo plugin.
- [x] Document limitations and future work.

## 7. Final checks

- [ ] Verify `git status`.
- [x] Inspect staged diff before commit.
- [x] Confirm existing backend paths are not rewritten.
- [x] Confirm the change remains minimal and demonstrable for the lab.

