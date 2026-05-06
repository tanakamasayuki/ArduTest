# ArduTest

[English README](README.md)

ArduTest は、Arduino スケッチ上で実機テストと観測処理を書くための小さな Arduino 側ライブラリです。

連携先は `pytest-embedded-arduino-cli` が提供する experimental な `arduino_test` fixture を想定しています。
Arduino 側ライブラリは test 定義と protocol event 送信を担当し、pytest 側は build/upload、同期、config、requirement 評価、結果収集、pytest assertion への反映を担当します。

## 現在の状態

ArduTest は experimental です。
最初の release では、Arduino Uno クラスのボードと host Arduino core の smoke test で動く、小さな protocol-compatible core を対象にします。

実装済みの core 機能:

- `TEST_CASE`
- `ASSERT_TRUE`, `ASSERT_FALSE`, `ASSERT_EQ`, `ASSERT_NE`
- `ArduTest.begin()` / `ArduTest.poll()`
- log、整数/double metric、text artifact、binary artifact
- `ARDUTEST_REQUIRE` / `ARDUTEST_REQUIRE_CONFIG` による capability/config metadata
- host protocol から設定される小さな config store
- ArduTest protocol version `1`

未実装または今後変更される可能性がある機能:

- pytest report 連携の拡充
- runtime skip semantics の安定化

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

各 `TEST_CASE` が 1 つの ArduTest test になります。pytest 側は `LIST` で test 一覧を取得し、`RUN` で実行します。

test body では assertion を使います。

- `ASSERT_TRUE(condition)`
- `ASSERT_FALSE(condition)`
- `ASSERT_EQ(expected, actual)`
- `ASSERT_NE(expected, actual)`

test から観測情報を pytest 側へ渡したい場合は、log、metric、artifact を使います。

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

requirement と required config は `TEST_CASE` の外に書きます。これらは pytest が test 実行前に使う metadata です。

```cpp
TEST_CASE(test_needs_config) {
  int sampleRate = ArduTest.configInt("sample_rate");
  ASSERT_EQ(1000, sampleRate);
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

`ARDUTEST_REQUIRE` は host 側が満たすべき capability を宣言します。`ARDUTEST_REQUIRE_CONFIG` は pytest が実行前に送るべき config 値を宣言します。

すべての sketch に必要な setup は次です。

```cpp
void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
```

`ArduTest.begin()` は `Serial` 上で protocol service を開始します。`ArduTest.poll()` は、`LIST` や `RUN` などの pytest command を受信するために繰り返し呼び出します。

### pytest test

pytest 側では、固定値や test-local な値を `set_capability()` / `set_config()` で渡してから `run()` を呼びます。

```python
def test_board(arduino_test):
    arduino_test.set_capability("measurement.current")
    arduino_test.set_config("sample_rate", 1000)

    arduino_test.run()
```

ArduTest 側が `failed` または `error` を返した場合、`arduino_test.run()` は pytest test を失敗にします。

実行する PC、接続先、実機環境、secret に依存する値は、環境変数、`.env`、CI variables で渡すのを推奨します。

```dotenv
ARDUINO_TEST_CAP_MEASUREMENT_CURRENT=true
ARDUINO_TEST_CONFIG_SAMPLE_RATE=1000
```

fixture API と環境変数の両方が同じ値を提供した場合は、fixture API が優先されます。

## pytest から使う

sketch の `sketch.yaml` で ArduTest を宣言します。
Arduino Libraries Index から使う場合は、再現性のために library version を固定してください。

```yaml
libraries:
  - ArduTest (0.2.0)
```

Library Manager に登録される前のローカル開発では、local checkout を参照できます。

```yaml
libraries:
  - dir: ../../
```

より具体的なユーザー向けの例は [`tests/user_usage_runner`](tests/user_usage_runner) を参照してください。

## protocol の注意点

`LOG`、`ARTIFACT_TEXT`、`FAIL`、`ERROR` は length-prefixed UTF-8 payload を使います。
`ARTIFACT_BINARY` は length-prefixed raw binary payload を使います。
payload bytes は newline 終端ではなく、payload の直後に次の `AT ...` message が続く場合があります。

device 側 config store の default は、Uno でも扱いやすいように小さくしています。

- 最大件数: `ARDUTEST_MAX_CONFIGS = 4`
- config name 長: `ARDUTEST_CONFIG_NAME_LENGTH = 31` bytes
- config value 長: `ARDUTEST_CONFIG_VALUE_LENGTH = 48` bytes

これらは `ArduTest.h` を include する前に compile-time define で変更できますが、大きくすると小容量ボードの RAM 使用量が増えます。

## Library Manager 登録準備

この repository は Arduino 1.5 library layout に従います。

- repository root に `library.properties`
- 公開 header/source は `src/` 配下
- サンプルは `examples/` 配下
- syntax highlight 用の `keywords.txt`

release tag は `library.properties` の version と合わせます。
例えば `version=0.2.0` に対して tag は `v0.2.0` とします。
最初の GitHub Release を公開したあと、Arduino Library Manager index へ登録申請できます。

## ドキュメント

- 全体仕様: [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino ライブラリ仕様: [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)

## ライセンス

MIT。詳細は [`LICENSE`](LICENSE) を参照してください。
