# ArduTest

[English README](README.md)

ArduTest は、Arduino スケッチ上で実機テストと観測処理を定義するための小さな Arduino 側ライブラリです。

連携先は `pytest-embedded-arduino-cli` 向けの `arduino_test` fixture 拡張を想定しています。Arduino 側ライブラリはテスト定義と実行時フックを提供し、pytest 側は書き込み、同期、capability、config、結果収集、成果物保存を担当します。

## 方針

- Arduino Uno クラスのボードでも最低限の実機テストを可能にする
- Arduino CLI 対応ボードで使える小さな API に保つ
- ログ、テキスト成果物、メトリクス、requirement、config のフックを提供する
- 高度な汎用ユニットテストフレームワーク化は目指さない

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
  const char* sampleRate = ArduTest.config("sample_rate");
  ArduTest.log(sampleRate);
  ASSERT_TRUE(sampleRate[0] != '\0');
}

ARDUTEST_REQUIRE(test_needs_config, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_needs_config, "sample_rate");
```

pytest fixture から実行する例:

```python
def test_board(arduino_test):
    arduino_test.run()
```

## 現在の状態

core line protocol、test 一覧取得、単一 test 実行、requirement、required config、log、metric、text artifact、assertion failure は host と Uno の smoke test ができる程度まで実装済みです。
API はまだ実験的で、Uno でも扱いやすい小さな構成に保っています。
binary artifact、artifact のファイル保存、pytest report 連携の拡充、重複 test name の報告は未実装です。

`LOG`、`ARTIFACT_TEXT`、`FAIL`、`ERROR` は length-prefixed payload を使います。
payload bytes は newline 終端ではなく、payload の直後に次の `AT ...` message が続く場合があります。

## ドキュメント

- 全体仕様: [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino ライブラリ仕様: [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)
- 通信プロトコル仕様: [`../../ARDUTEST_PROTOCOL_SPEC.ja.md`](../../ARDUTEST_PROTOCOL_SPEC.ja.md)
- サンプル: [`examples/Basic`](examples/Basic)

## ライセンス

MIT。詳細は [`LICENSE`](LICENSE) を参照してください。
