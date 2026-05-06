# ArduTest tests

This directory contains integration tests for running the ArduTest library through `pytest-embedded-arduino-cli`.

## User-facing usage

- `user_usage_runner`
  - Shows the recommended user flow through a small `.ino` sketch and a simple pytest test.
  - The Arduino sketch demonstrates test cases, requirements, config, logs, metrics, text artifacts, and binary artifacts.
  - The pytest side only provides fixed values with `set_capability()` / `set_config()` and calls `arduino_test.run()`.

Use this runner first when you want to understand how a project test should be written.

## ArduTest library and protocol coverage

- `protocol_dut_runner`
  - Checks the ArduTest protocol directly with `dut.write(...)` and `dut.expect_exact(...)`.
- `protocol_runner`
  - Runs through the `arduino_test` fixture using the `HELLO` / `LIST` / `RUN` protocol flow.
- `assertion_runner`
  - Checks that assertion failures emit `FAIL` events and `RESULT failed`.
- `duplicate_runner`
  - Checks that duplicate test names emit `ERROR duplicate_test`.
- `protocol_error_runner`
  - Checks unknown command/test handling, SET_CONFIG boundaries, config store limits, and CLEAR_CONFIG.

These runners are primarily for ArduTest implementation coverage. They intentionally inspect protocol details that normal user tests should not need to assert.

If the pytest plugin repository and the ArduTest repository are checked out under the same parent directory, run these tests from the pytest plugin repository:

```sh
uv run --env-file .env pytest --profile=host ../ArduTest/tests/
uv run --env-file .env pytest --profile=uno --port=/dev/ttyUSB0 ../ArduTest/tests/
```

## Updating the pytest plugin dependency

`tests/uv.lock` pins the exact `pytest-embedded-arduino-cli` version used when running from the ArduTest `tests/` project.

Do not delete `uv.lock` as the normal update procedure. Deleting it forces uv to resolve every dependency again, which may upgrade unrelated packages and make the change harder to review.

To update only the pytest plugin to the latest version allowed by `tests/pyproject.toml`:

```sh
cd tests
uv lock --upgrade-package pytest-embedded-arduino-cli
uv sync
```

To pin a specific released version, first update `tests/pyproject.toml`, then refresh the lock file:

```sh
cd tests
uv add "pytest-embedded-arduino-cli>=1.2,<1.3"
uv sync
```

For local development of the pytest plugin, run from the `pytest-embedded-arduino-cli` checkout instead. That uses the local plugin source tree and does not require changing ArduTest's lock file:

```sh
cd ../pytest-embedded-arduino-cli
uv run pytest ../ArduTest/tests/
```
