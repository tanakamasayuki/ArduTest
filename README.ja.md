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
- log、整数/double metric、text artifact
- `ARDUTEST_REQUIRE` / `ARDUTEST_REQUIRE_CONFIG` による capability/config metadata
- host protocol から設定される小さな config store
- ArduTest protocol version `1`

未実装または今後変更される可能性がある機能:

- binary artifact
- host 側での artifact ファイル保存
- pytest report 連携の拡充
- 重複 test name の報告
- runtime skip semantics の安定化

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

requirement と required config は metadata として公開できます。
これにより host 側は test 実行前に skip 判定できます。

```cpp
TEST_CASE(test_needs_config) {
  ASSERT_TRUE(ArduTest.config("sample_rate")[0] != '\0');
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

## pytest から使う

sketch の `sketch.yaml` で ArduTest を宣言します。
Arduino Libraries Index から使う場合は、再現性のために library version を固定してください。

```yaml
libraries:
  - ArduTest (0.1.0)
```

その sketch を `pytest-embedded-arduino-cli` から実行します。

```python
def test_board(arduino_test):
    arduino_test.run()
```

Library Manager に登録される前のローカル開発では、local checkout を参照できます。

```yaml
libraries:
  - dir: ../../
```

## protocol の注意点

`LOG`、`ARTIFACT_TEXT`、`FAIL`、`ERROR` は length-prefixed payload を使います。
payload bytes は newline 終端ではなく、payload の直後に次の `AT ...` message が続く場合があります。

## Library Manager 登録準備

この repository は Arduino 1.5 library layout に従います。

- repository root に `library.properties`
- 公開 header/source は `src/` 配下
- サンプルは `examples/` 配下
- syntax highlight 用の `keywords.txt`

release tag は `library.properties` の version と合わせます。
例えば `version=0.1.0` に対して tag は `v0.1.0` とします。
最初の GitHub Release を公開したあと、Arduino Library Manager index へ登録申請できます。

## ドキュメント

- 全体仕様: [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino ライブラリ仕様: [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)

## ライセンス

MIT。詳細は [`LICENSE`](LICENSE) を参照してください。
