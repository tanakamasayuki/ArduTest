# ArduTest

[日本語 README](README.ja.md)

ArduTest is a small Arduino-side library for writing real-device tests and observation code in Arduino sketches.

It is intended to work with the experimental `arduino_test` fixture provided by `pytest-embedded-arduino-cli`.
The Arduino library defines tests and emits protocol events; the pytest side handles build/upload, synchronization, config, requirement evaluation, result collection, and pytest assertions.

## Status

ArduTest is experimental.
The first release targets a small protocol-compatible core that works on Arduino Uno-class boards and host Arduino core smoke tests.

Implemented core features:

- `TEST_CASE`
- `ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_NE`
- `ArduTest.begin()` and `ArduTest.poll()`
- logs, integer/double metrics, text artifacts, and binary artifacts
- required capability/config metadata with `ARDUTEST_REQUIRE` and `ARDUTEST_REQUIRE_CONFIG`
- small config store populated by the host protocol
- ArduTest protocol version `1`

Not yet implemented or still subject to change:

- richer pytest report integration
- stable runtime skip semantics

## Quick Start

### Arduino sketch

```cpp
#include <ArduTest.h>

TEST_CASE(test_example) {
  ArduTest.log("running test_example");
  ASSERT_TRUE(true);
}

void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
```

Each `TEST_CASE` becomes one ArduTest test. The pytest side discovers these tests with `LIST` and runs them with `RUN`.

Use assertions inside the test body:

- `ASSERT_TRUE(condition)`
- `ASSERT_FALSE(condition)`
- `ASSERT_EQ(expected, actual)`
- `ASSERT_NE(expected, actual)`

Use logs, metrics, and artifacts when the test should expose observation data to pytest:

```cpp
TEST_CASE(test_observability) {
  const uint8_t bytes[] = {0x41, 0x54, 0x0a, 0x01};

  ArduTest.log("collecting sample");
  ArduTest.reportMetric("sample_rate", 1000);
  ArduTest.attachText("sample_rate.txt", "1000");
  ArduTest.attachBinary("sample.bin", "application/octet-stream", bytes, sizeof(bytes));
  ASSERT_TRUE(true);
}
```

Requirements and required config are written outside the `TEST_CASE` body. They are metadata used by pytest before the test is run:

```cpp
TEST_CASE(test_needs_config) {
  int sampleRate = ArduTest.configInt("sample_rate");
  ASSERT_EQ(1000, sampleRate);
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

`ARDUTEST_REQUIRE` declares a capability that the host must provide. `ARDUTEST_REQUIRE_CONFIG` declares a config value that pytest must send before running the test.

The required setup in every sketch is:

```cpp
void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
```

`ArduTest.begin()` starts the protocol service on `Serial`. `ArduTest.poll()` must run repeatedly so the library can receive pytest commands such as `LIST` and `RUN`.

### pytest test

On the pytest side, provide fixed test-local values with `set_capability()` and `set_config()`, then call `run()`:

```python
def test_board(arduino_test):
    arduino_test.set_capability("measurement.current")
    arduino_test.set_config("sample_rate", 1000)

    arduino_test.run()
```

`arduino_test.run()` fails the pytest test automatically if any ArduTest test reports `failed` or `error`.

For values that depend on the machine, board, lab setup, or secrets, prefer environment variables, `.env`, or CI variables:

```dotenv
ARDUINO_TEST_CAP_MEASUREMENT_CURRENT=true
ARDUINO_TEST_CONFIG_SAMPLE_RATE=1000
```

The fixture API takes precedence over environment variables when both provide the same value.

## Use From pytest

Declare ArduTest in the sketch's `sketch.yaml`.
For reproducible tests, pin the library version when using the Arduino Libraries Index:

```yaml
libraries:
  - ArduTest (0.2.0)
```

During local development before the library is available from Library Manager, a sketch can refer to a local checkout:

```yaml
libraries:
  - dir: ../../
```

For a more complete user-facing example, see [`tests/user_usage_runner`](tests/user_usage_runner).

## Protocol Notes

`LOG`, `ARTIFACT_TEXT`, `FAIL`, and `ERROR` use length-prefixed UTF-8 payloads.
`ARTIFACT_BINARY` uses a length-prefixed raw binary payload.
The payload bytes are not newline-terminated; the next `AT ...` message may start immediately after the payload.

The default device-side config store is intentionally small:

- max entries: `ARDUTEST_MAX_CONFIGS = 4`
- config name length: `ARDUTEST_CONFIG_NAME_LENGTH = 31` bytes
- config value length: `ARDUTEST_CONFIG_VALUE_LENGTH = 48` bytes

These values can be overridden with compile-time defines before including `ArduTest.h`, but larger limits increase RAM usage on small boards.

## Library Manager Preparation

This repository follows the Arduino 1.5 library layout:

- `library.properties` at the repository root
- public headers and sources under `src/`
- examples under `examples/`
- optional syntax highlighting in `keywords.txt`

Release tags should match `library.properties` versions, for example `v0.2.0` for `version=0.2.0`.
After the first GitHub release is published, the repository can be submitted to the Arduino Library Manager index.

## Documentation

- Overall spec (Japanese): [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino library spec (Japanese): [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)

## License

MIT (see [`LICENSE`](LICENSE)).
