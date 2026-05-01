# ArduTest Arduino ライブラリ仕様（ドラフト）

## 1. 概要

ArduTest は、Arduino CLI 対応ボード上で実機テストおよび評価ロジックを記述するための Arduino ライブラリである。

本ライブラリはテスト定義、assertion、ログ、成果物、メトリクス、requirement、config 参照を提供し、`pytest-embedded-arduino-cli` の Arduino CLI 連携拡張である `arduino_test` と連携して実行制御と結果収集を行う。

本仕様では Arduino 側ライブラリの公開 API、実行モデル、状態管理、`pytest-embedded-arduino-cli` 側との責務境界を定義する。通信プロトコルの詳細なメッセージ形式は [`../../ARDUTEST_PROTOCOL_SPEC.ja.md`](../../ARDUTEST_PROTOCOL_SPEC.ja.md) で定義する。

---

## 2. 設計方針

### 2.1 基本方針

- Arduino Uno で動作可能な最小機能をコアとする
- ESP32 等の高機能ボードでは拡張機能を利用可能とする
- ボード固有機能に依存しない API を基本とする
- テスト定義の記述量を少なく保つ
- Uno でも最低限の実機テストを記述できるようにする
- Unity 等の既存テストフレームワークと競合する高度な汎用テスト機能は目指さない
- ESP32 等では、用途に応じて Unity 等の既存フレームワークと ArduTest のどちらを使ってもよい

### 2.2 非目的

- PC 上での単体テスト実行
- 高度な matcher / fixture / mock 機構を備えた汎用テストフレームワーク化
- `pytest-embedded-arduino-cli` 側の設定管理
- 外部測定機器の直接制御
- 詳細な通信プロトコル仕様

---

## 3. 対象環境

### 3.1 標準対象

- Arduino CLI でビルドおよび書き込み可能なボード
- `Stream` 互換の通信路を持つ環境
- 初期標準ボード: Arduino Uno

### 3.2 拡張対象

- ESP32
- 画像や大きなバイナリ成果物を扱える RAM / Flash を持つボード
- 複数シリアルやネットワーク通信を持つボード

---

## 4. 基本利用

```cpp
#include <ArduTest.h>

TEST_CASE(test_example) {
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

### 4.1 最小スケッチ要件

利用者のスケッチは以下を満たす必要がある。

- `ArduTest.h` を include する
- `TEST_CASE` で 1 件以上のテストを定義する
- `setup()` で通信路を初期化する
- `setup()` で `ArduTest.begin()` を呼ぶ
- `loop()` で `ArduTest.poll()` を継続的に呼ぶ

---

## 5. 公開 API

### 5.1 ヘッダ

```cpp
#include <ArduTest.h>
```

### 5.2 初期化

```cpp
ArduTest.begin();
ArduTest.begin(Stream& stream);
```

`begin()` は ArduTest の内部状態を初期化し、`arduino_test` からの制御コマンドを受け付け可能にする。

引数なしの `begin()` は既定通信路として `Serial` を使用する。`Stream&` を指定した場合は、その通信路を `arduino_test` との制御および結果送信に使用する。

### 5.3 ポーリング

```cpp
ArduTest.poll();
```

`poll()` は `arduino_test` からの入力を処理し、必要に応じてテストを実行する。`loop()` から繰り返し呼び出されることを前提とする。

`poll()` は長時間ブロックしない実装を基本とする。ただしテスト関数実行中は、そのテスト関数の処理時間に依存してブロックしてよい。

### 5.4 テスト定義

```cpp
TEST_CASE(test_name) {
  // test body
}
```

`TEST_CASE` はテスト関数を定義し、ArduTest のテスト一覧に登録する。

要件:

- `test_name` は C++ 識別子として有効であること
- `arduino_test` に公開されるテスト名は既定で `test_name` の文字列表現とする
- 同一スケッチ内で `arduino_test` に公開されるテスト名は一意であること
- テスト関数は引数なし、戻り値なしとする
- テスト関数内で assertion、ログ、成果物、メトリクスを送信できる

重複したテスト名は、実行対象指定と結果の紐付けが曖昧になるため許可しない。重複を検出できる場合は `ArduTest.begin()` または初期同期時にエラーとして報告する。ビルド時に同一 C++ 識別子として検出される場合は、通常のコンパイルエラーとして扱ってよい。

### 5.5 Assertion

```cpp
ASSERT_TRUE(condition);
ASSERT_FALSE(condition);
ASSERT_EQ(expected, actual);
ASSERT_NE(expected, actual);
```

Assertion はテストの pass / fail を決定する。

コア機能では `ASSERT_TRUE` を必須とし、その他の assertion は段階的に追加してよい。

Assertion 失敗時の動作:

- 現在のテストを failed として記録する
- 失敗した式、ファイル名、行番号を結果に含める
- 当該 assertion 以降のテスト処理は実行しないことを基本とする

### 5.6 ログ

```cpp
ArduTest.log("message");
```

ログは現在実行中のテストに紐づく補助情報として `arduino_test` へ送信される。

要件:

- ログ API は `ArduTest.log` のみとする
- ログは pass / fail 判定とは独立する
- 実行中のテストがない場合のログはセッションログとして扱う
- Uno では短いテキストログを前提とする
- `printf` 形式のログ API は、Uno での RAM 使用量や AVR 環境の formatting 差を避けるため提供しない

### 5.7 成果物

```cpp
ArduTest.attachText("log.txt", text);
ArduTest.attachBinary("image.bmp", "image/bmp", data, length);
```

成果物はログとは分離されたファイルとして `arduino_test` 側に保存される。

コア機能:

- `attachText`

拡張機能:

- `attachBinary`

要件:

- 成果物は現在実行中のテストに紐づく
- ファイル名は相対パスとして扱う
- `arduino_test` 側が保存先ディレクトリを決定する
- Arduino 側はファイルシステムへの保存を必須としない

### 5.8 メトリクス

```cpp
ArduTest.reportMetric("boot_time_ms", value);
```

メトリクスは数値の観測値として `arduino_test` へ送信される。

要件:

- メトリクス名は任意の文字列ラベルとする
- 値は整数または浮動小数点数を扱えることが望ましい
- メトリクス送信自体は pass / fail 判定を行わない
- 閾値判定は `arduino_test` 側または利用者テストで行う

### 5.9 Requirement

```cpp
REQUIRE("network");
REQUIRE("measurement.current");
REQUIRE_CONFIG("ssid");
```

Requirement はテスト実行に必要な環境条件を Arduino 側から宣言する。

Requirement は host 側が実行前に skip 判定できる必要があるため、初期同期のテスト metadata として取得できる形で登録する。テスト関数の実行中にだけ `REQUIRE()` を呼ぶ方式は、実行前 skip には使えない。

要件:

- requirement は任意の文字列ラベルとする
- capability の評価は `arduino_test` 側が行う
- 満たされない requirement を持つテストは `arduino_test` 側で skip される
- Arduino 側は外部機材や環境変数を直接解釈しない

### 5.10 Config 参照

```cpp
const char* ssid = ArduTest.config("ssid");
int timeout = ArduTest.configInt("timeout_ms", 1000);
bool enabled = ArduTest.configBool("feature_enabled", false);
```

Config は `arduino_test` 側から Arduino 側へ提供される環境依存値である。

要件:

- config 値は文字列を基本とする
- 型付き API は文字列からの変換を行う補助 API とする
- 未提供の config 参照時は既定値を返せること
- `REQUIRE_CONFIG` で必須 config を宣言できること

---

## 6. テストライフサイクル

### 6.1 状態

ArduTest は最低限以下の状態を持つ。

```text
Idle
Syncing
Ready
Running
Finished
Error
```

### 6.2 起動時

1. 利用者スケッチが `Serial.begin()` 等で通信路を初期化する
2. `ArduTest.begin()` がテスト一覧と内部状態を初期化する
3. `ArduTest.poll()` が `arduino_test` からの同期要求を待つ

### 6.3 初期同期

初期同期では Arduino 側から `arduino_test` へ以下の情報を提供する。

- ライブラリ名
- プロトコルバージョン
- テスト一覧
- 各テストの requirement
- 各テストの必須 config
- 現在状態

`arduino_test` 側から Arduino 側へ以下の情報が提供される。

- capability 評価結果
- config 値
- 実行対象テスト
- 実行コマンド

### 6.4 実行

`arduino_test` 側が実行対象テストを指定し、Arduino 側は指定されたテストのみを実行する。

各テストの実行順序は `arduino_test` 側が決定する。Arduino 側の登録順は一覧表示時の既定順として使用できる。

### 6.5 結果

各テストは以下のいずれかの結果を持つ。

- `passed`
- `failed`
- `skipped`
- `error`

`skipped` は原則として `arduino_test` 側で決定される。Arduino 側で実行中に継続不能な条件が判明した場合は `error` または明示的 skip API の追加を検討する。

---

## 7. pytest-embedded-arduino-cli 側との責務境界

### 7.1 ArduTest の責務

- テスト定義の登録
- テスト一覧の提供
- requirement / config requirement の宣言
- config 値の保持と参照
- 指定されたテストの実行
- assertion 結果の送信
- ログ、成果物、メトリクスの送信
- 実行状態の管理

### 7.2 arduino_test 側の責務

- `pytest-embedded-arduino-cli` によるボードへの書き込み
- 接続確立
- 初期同期
- capability の評価
- config の提供
- 実行対象テストの決定
- skip 判定
- reset 制御
- ログ、成果物、メトリクスの保存
- pytest 結果への反映

---

## 8. コア機能

Arduino Uno での動作を必須とするコア機能は以下とする。

- `TEST_CASE`
- `ASSERT_TRUE`
- `ArduTest.begin`
- `ArduTest.poll`
- `ArduTest.log`
- `ArduTest.attachText`
- `REQUIRE`
- `REQUIRE_CONFIG`
- `ArduTest.config`
- 初期同期
- `arduino_test` 側からの単一テスト実行
- pass / fail / error 結果送信

---

## 9. 拡張機能

以下は初期実装では必須としない。

- `ASSERT_FALSE`
- `ASSERT_EQ`
- `ASSERT_NE`
- `ArduTest.attachBinary`
- 大容量成果物の分割送信
- 画像成果物
- 複数通信路
- 明示的 skip API
- テストごとの timeout
- ボード固有 capability
- 外部測定機器との連携

---

## 10. 制約

### 10.1 メモリ

Uno 対応のため、コア実装は RAM 使用量を抑える。

指針:

- テスト一覧や静的文字列は可能な限り Flash 側に置く
- 大きな一時バッファを持たない
- バイナリ成果物は拡張機能として扱う

### 10.2 通信

- 通信路は `Stream` 抽象を基本とする
- 通信速度は利用者スケッチが設定する
- 初期標準例では `Serial` 115200 bps を用いる
- 通信途絶時の復旧は `arduino_test` 側 reset と組み合わせる

### 10.3 実行時間

- テスト関数の実行時間は利用者責任とする
- timeout 判定は `arduino_test` 側で行うことを基本とする
- Arduino 側は可能な範囲で状態を送信する

---

## 11. エラー扱い

以下の場合、Arduino 側は `error` として扱う。

- 不明な実行コマンドを受け取った
- 存在しないテスト名が指定された
- config 受信に失敗した
- 成果物送信中に継続不能な状態になった
- 内部状態と要求された操作が矛盾した

Assertion 失敗は `failed` であり、`error` ではない。

---

## 12. 互換性

### 12.1 API 互換性

公開 API は初期安定版以降、後方互換を維持する。

破壊的変更が必要な場合は、ライブラリのメジャーバージョンを上げる。

### 12.2 プロトコル互換性

Arduino 側は同期時にプロトコルバージョンを `arduino_test` 側へ通知する。

`arduino_test` 側は非対応プロトコルの場合、実行を中止し、明確なエラーを出す。

---

## 13. 実装ファイル構成案

```text
ArduTest/
  library.properties
  src/
    ArduTest.h
    ArduTest.cpp
    ArduTestAssertions.h
    ArduTestProtocol.h
  examples/
    Basic/
      Basic.ino
```

### 13.1 `ArduTest.h`

利用者が include する公開ヘッダ。

### 13.2 `ArduTest.cpp`

初期化、ポーリング、テスト実行、状態管理を実装する。

### 13.3 `ArduTestAssertions.h`

Assertion macro と失敗処理を実装する。

### 13.4 `ArduTestProtocol.h`

`arduino_test` との通信に使う内部プロトコル定義を配置する。

---

## 14. 未決事項

- テスト登録を静的初期化で行うか、明示登録にするか
- requirement / config requirement の metadata 登録 API
- assertion 失敗時に即 return する実装方法
- 文字列を RAM / Flash のどちらで扱うか
- config 値の最大長
- 成果物名の制約
- 明示的 skip API を Arduino 側に持たせるか
- Unity との併用 API を提供するか
