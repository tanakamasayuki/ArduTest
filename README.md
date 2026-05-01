# ArduTest

[日本語 README](README.ja.md)

ArduTest is a small Arduino-side library for defining real-device tests and observations in Arduino sketches.

It is intended to work with an `arduino_test` fixture extension for `pytest-embedded-arduino-cli`. The Arduino library provides the test definitions and runtime hooks; the pytest side is expected to handle flashing, synchronization, environment capabilities, configuration, result collection, and artifacts.

## Goals

- Make minimal real-device tests practical on Arduino Uno-class boards.
- Keep the core API small and portable across Arduino CLI compatible boards.
- Provide hooks for logs, text artifacts, metrics, requirements, and configuration.
- Avoid becoming a large general-purpose unit test framework.

## Quick Start

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

Requirements and required config can be exposed as metadata so the host can decide skips before running a test:

```cpp
TEST_CASE(test_needs_config) {
  const char* sampleRate = ArduTest.config("sample_rate");
  ArduTest.log(sampleRate);
  ASSERT_TRUE(sampleRate[0] != '\0');
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

Run through the pytest fixture:

```python
def test_board(arduino_test):
    arduino_test.run()
```

## Current Status

The core line protocol, test listing, single-test execution, requirements, required config, logs, metrics, text artifacts, and assertion failures are implemented enough for host and Uno smoke tests.
The API is still experimental and intentionally small and Uno-friendly.
Binary artifacts, persisted artifact files, richer pytest report integration, and duplicate-name reporting are still pending.

`LOG`, `ARTIFACT_TEXT`, `FAIL`, and `ERROR` use length-prefixed payloads.
The payload bytes are not newline-terminated; the next `AT ...` message may start immediately after the payload.

## Documentation

- Overall spec (Japanese): [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino library spec (Japanese): [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)
- Protocol spec (Japanese): [`../../ARDUTEST_PROTOCOL_SPEC.ja.md`](../../ARDUTEST_PROTOCOL_SPEC.ja.md)
- Example: [`examples/Basic`](examples/Basic)

## License

MIT (see [`LICENSE`](LICENSE)).
