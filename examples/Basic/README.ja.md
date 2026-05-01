# Basic

ArduTest の最小サンプルです。

この example は 2 つの test を定義します。

- `test_true_passes`: log を出力して pass する
- `test_metric_and_artifact`: metric と text artifact を送信して pass する

`pytest-embedded-arduino-cli` の `arduino_test` fixture から実行するか、最小 sketch 構造の参考として使えます。

```cpp
void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
```
