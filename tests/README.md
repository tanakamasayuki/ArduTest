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
