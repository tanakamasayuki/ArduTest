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

## 現在の状態

現時点では Arduino ライブラリの骨格とドラフト仕様を含みます。Arduino 側 API は Uno でも扱いやすいように小さく保っています。既存実装は仮の smoke test 用骨格であり、`pytest-embedded-arduino-cli` の `arduino_test` と連携する実装はプロトコル仕様を優先して設計します。

## ドキュメント

- 全体仕様: [`ArduTest_spec.ja.md`](ArduTest_spec.ja.md)
- Arduino ライブラリ仕様: [`ArduTest_library_spec.ja.md`](ArduTest_library_spec.ja.md)
- 通信プロトコル仕様: [`../../ARDUTEST_PROTOCOL_SPEC.ja.md`](../../ARDUTEST_PROTOCOL_SPEC.ja.md)
- サンプル: [`examples/Basic`](examples/Basic)

## ライセンス

MIT。詳細は [`LICENSE`](LICENSE) を参照してください。
