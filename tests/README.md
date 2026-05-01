# ArduTest tests

This directory contains integration tests for running the ArduTest library through `pytest-embedded-arduino-cli`.

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

If the pytest plugin repository and the ArduTest repository are checked out under the same parent directory, run these tests from the pytest plugin repository:

```sh
uv run --env-file .env pytest --profile=host ../ArduTest/tests/
uv run --env-file .env pytest --profile=uno --port=/dev/ttyUSB0 ../ArduTest/tests/
```
