# AGENTS.md

## Repository

This repository is `evolution-data-server`, a large GNOME C/C++ project built with CMake. Work in small, focused changes and avoid broad rewrites.

The current student task is documented in:

- docs/lab5/backend-plugin-system-task.md

Do not start implementing the task without first reading that file and producing a plan.

## Local environment

- Host OS: Windows.
- Shell: PowerShell.
- Repository path: C:\Users\Hukollko\projects\lab5_project.
- Native Windows builds are not expected to be reliable for this project.
- Prefer Linux or WSL for real build and test runs.

## High-value files to inspect first

Before changing code, inspect the relevant files from this list:

- docs/lab5/backend-plugin-system-task.md
- README
- CMakeLists.txt
- .gitlab-ci.yml
- .editorconfig
- cmake/
- tests/
- relevant src/*/CMakeLists.txt

For the backend plugin system task, focus especially on:

- src/libebackend/
- src/addressbook/libedata-book/
- src/calendar/libedata-cal/
- src/camel/
- src/services/
- docs/
- tests/

## Build notes

The project uses CMake. Prefer out-of-tree builds and do not create build artifacts in the source root.

Typical Linux/WSL build commands:

- cmake -S . -B _build -G Ninja
- cmake --build _build

If dependencies are missing, do not guess large rewrites. Report the missing dependency and suggest the smallest next step.

## Tests

Tests are wired through CMake/CTest. Prefer focused tests before running large suites.

Common commands:

- ctest --test-dir _build --output-on-failure
- ctest --test-dir _build -R <test-name> --output-on-failure

When adding new functionality, add or update the narrowest relevant tests first.

## Project structure

- src/ contains the main libraries, services, modules, tools, and server code.
- src/libebackend/ is the expected place for shared backend infrastructure.
- src/addressbook/libedata-book/ contains address book backend integration.
- src/calendar/libedata-cal/ contains calendar backend integration.
- src/camel/ contains mail/provider infrastructure.
- src/services/ contains service processes that may need dynamic loading integration.
- tests/ contains CMake-based test suites.
- cmake/ contains project-specific CMake modules and custom targets.
- po/ contains translation infrastructure.
- docs/ contains documentation build configuration.

## Style

- Follow .editorconfig.
- C/C++ files use tab indentation with width 8.
- Keep lines around 100 characters where practical.
- Preserve the existing local style in touched files.
- Do not reformat unrelated files.

## Workflow

- Work on branch lab5-agent.
- Keep changes small and easy to review.
- Before changing code, inspect the nearest CMakeLists.txt and existing tests for the affected area.
- For the lab task, first produce a plan, then implement the smallest demonstrable version.
- Do not modify unrelated subsystems.
- Before committing, run git status and inspect the staged diff with git diff --cached.

## Security

- Do not read, print, copy, summarize, or expose secrets from .env, *.key, *.pem, credentials.json, token.json, SSH keys, API keys, database dumps, or production configs.
- Do not include real tokens, passwords, private server addresses, personal data, or commercial information in code, tests, commits, logs, or documentation.
- If a secret is needed, use placeholders and example files.
- Do not request access to C:\*; only work inside the current repository.
