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

## Current Status

This repository currently contains the Arduino library skeleton and draft specifications. The Arduino-side API is intentionally small and Uno-friendly. The current implementation is only a temporary smoke-test skeleton; the future `arduino_test` integration for `pytest-embedded-arduino-cli` should be designed against the protocol specification.

## Documentation

- Overall spec (Japanese): [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino library spec (Japanese): [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)
- Protocol spec (Japanese): [`../../ARDUTEST_PROTOCOL_SPEC.ja.md`](../../ARDUTEST_PROTOCOL_SPEC.ja.md)
- Example: [`examples/Basic`](examples/Basic)

## License

MIT (see [`LICENSE`](LICENSE)).
