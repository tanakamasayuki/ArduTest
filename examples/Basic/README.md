# Basic

Minimal ArduTest sketch.

The example defines two tests:

- `test_true_passes` emits a log line and passes.
- `test_metric_and_artifact` reports a metric, attaches a text artifact, and passes.

Run it through `pytest-embedded-arduino-cli` and the `arduino_test` fixture, or use it as a reference for the minimum sketch structure:

```cpp
void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
```
