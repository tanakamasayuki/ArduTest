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
- logs, integer/double metrics, and text artifacts
- required capability/config metadata with `ARDUTEST_REQUIRE` and `ARDUTEST_REQUIRE_CONFIG`
- small config store populated by the host protocol
- ArduTest protocol version `1`

Not yet implemented or still subject to change:

- binary artifacts
- persisted artifact files on the host
- richer pytest report integration
- duplicate test name reporting
- stable runtime skip semantics

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
  ASSERT_TRUE(ArduTest.config("sample_rate")[0] != '\0');
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

## Use From pytest

Declare ArduTest in the sketch's `sketch.yaml`.
For reproducible tests, pin the library version when using the Arduino Libraries Index:

```yaml
libraries:
  - ArduTest (0.1.0)
```

Then run the sketch through `pytest-embedded-arduino-cli`:

```python
def test_board(arduino_test):
    arduino_test.run()
```

During local development before the library is available from Library Manager, a sketch can refer to a local checkout:

```yaml
libraries:
  - dir: ../../
```

## Protocol Notes

`LOG`, `ARTIFACT_TEXT`, `FAIL`, and `ERROR` use length-prefixed payloads.
The payload bytes are not newline-terminated; the next `AT ...` message may start immediately after the payload.

## Library Manager Preparation

This repository follows the Arduino 1.5 library layout:

- `library.properties` at the repository root
- public headers and sources under `src/`
- examples under `examples/`
- optional syntax highlighting in `keywords.txt`

Release tags should match `library.properties` versions, for example `v0.1.0` for `version=0.1.0`.
After the first GitHub release is published, the repository can be submitted to the Arduino Library Manager index.

## Documentation

- Overall spec (Japanese): [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino library spec (Japanese): [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)

## License

MIT (see [`LICENSE`](LICENSE)).
