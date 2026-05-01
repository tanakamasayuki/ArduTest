# ArduTest 実機テスト基盤 要件定義（ドラフト）

## 1. 概要

本仕様は、Arduino CLI 対応ボード上で動作する実機テストおよび評価基盤を定義する。

本基盤は以下の2要素で構成される。

- Arduino 側ライブラリ（ArduTest）
- `pytest-embedded-arduino-cli` 向け pytest 拡張（fixture: `arduino_test`）

ArduTest は Arduino 上で動作するテストおよび評価ロジックを実装し、  
`pytest-embedded-arduino-cli` 側の `arduino_test` はその実行・環境提供・結果収集を行う。

---

## 2. 名称

本仕様では以下の名称を用いる。

- Arduino 側ライブラリ: `ArduTest`
- `pytest-embedded-arduino-cli` 側 fixture: `arduino_test`

これらの名称は現時点では仮称であり、今後変更される可能性がある。

---

## 3. 目的と位置づけ

### 3.1 目的

本基盤は以下を主目的とする。

- Arduino Uno でも最低限の実機テストを可能にする
- 実機上でのみ観測可能な情報の収集
- テスト結果に紐づく成果物（artifact）の取得
- メトリクスの記録およびトレンド分析
- 環境依存テストの制御（skip / config）

---

### 3.2 非目的

以下は本基盤の主目的ではない。

- PC上での単体テストの代替
- 高度な汎用ユニットテストフレームワークの提供

ESP32 等で一般的なテストを行う場合は、用途に応じて Unity 等の既存フレームワークを使ってもよいし、ArduTest を使ってもよい。

---

### 3.3 位置づけ

```
Unity:
  - pass/fail中心のテスト

ArduTest:
  - Uno でも使える最低限の実機テスト
  - 実機評価・観測・成果物収集
```

---

## 4. 対象範囲

### 4.1 対象ボード

- 標準機: Arduino Uno
- 高機能機: ESP32
- その他 Arduino CLI 対応ボード

---

### 4.2 方針

- Uno で動作可能な最小機能をコアとする
- 高機能ボードでは拡張機能を利用可能とする
- 特定ボードに依存しない設計とする

---

## 5. システム構成

```
pytest-embedded-arduino-cli
  ├─ dut（既存）
  └─ arduino_test fixture（Arduino CLI 連携拡張）

Arduino
  └─ ArduTest ライブラリ
```

---

## 6. pytest-embedded-arduino-cli 側

### 6.1 基本利用

```python
def test_board(arduino_test):
    arduino_test.run()
```

---

### 6.2 責務

`arduino_test` fixture は、`pytest-embedded-arduino-cli` を拡張して以下を担当する。

- 初期同期
- テスト一覧取得
- requirement評価
- capability提供
- config提供
- 実行制御
- reset管理
- ログ・成果物収集
- pytest結果反映

---

### 6.3 dut との関係

- `arduino_test` は `pytest-embedded-arduino-cli` の Arduino CLI 連携を前提とする
- `dut` は内部的に利用可能
- 既存の `dut` 利用テストには影響を与えない
- 本拡張は `pytest-embedded-arduino-cli` 上に追加される高水準 fixture として動作する

---

## 7. Arduino 側（ArduTest）

### 7.1 基本構造

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

---

### 7.2 提供機能

- テスト定義（TEST_CASE）
- assertion（ASSERT_TRUE）
- ログ出力
- 成果物添付
- requirement宣言
- config利用

テスト名は `arduino_test` から実行対象を指定し、結果を紐付けるために一意である必要がある。重複したテスト名は許可しない。

---

## 8. 実行モデル

### 8.1 フロー

```
1. 接続確立
2. 初期同期
3. テスト一覧取得
4. 実行計画決定
5. テスト実行
6. 結果収集
```

---

### 8.2 初期同期

以下を交換する。

- テスト一覧
- requirement
- config
- capability
- 実行状態

---

## 9. Requirement / Capability

### 9.1 方針

- Arduino 側が requirement を宣言
- `arduino_test` 側が実行可否を決定
- 未満足は skip

---

### 9.2 例

```cpp
REQUIRE("network");
REQUIRE_CONFIG("ssid");
```

---

### 9.3 特徴

- 任意の文字列ラベル
- ユーザー定義可能
- `arduino_test` 側で解釈

---

## 10. Config

### 10.1 用途

環境依存値を提供する。

```cpp
REQUIRE_CONFIG("ssid");
REQUIRE_CONFIG("password");
```

---

### 10.2 提供方法

- 環境変数
- 設定ファイル

---

## 11. 成果物（Artifact）

### 11.1 テキスト

```cpp
ArduTest.attachText("log.txt", text);
```

---

### 11.2 バイナリ（拡張）

```cpp
ArduTest.attachBinary("image.bmp", "image/bmp", data, length);
```

---

### 11.3 特徴

- `arduino_test` 側でファイル保存
- テストと紐付け
- ログとは分離

---

## 12. メトリクス

```cpp
ArduTest.reportMetric("boot_time_ms", value);
```

用途：

- トレンド分析
- 比較
- 可視化

※ pass/fail判定は必須ではない

---

## 13. 外部機材（拡張）

### 13.1 電流測定

```cpp
REQUIRE("measurement.current");
```

初期 backend:

- PPK2

---

### 13.2 方針

- Arduino 側は抽象 requirement のみ宣言
- `arduino_test` 側で backend を選択

---

## 14. 環境設定

### 14.1 capability

環境変数から取得する。

例：

```
ARDUINO_TEST_CAP_network=true
```

---

### 14.2 要件

`arduino_test` は以下を出力する。

- 必要 capability
- 未設定項目
- 設定方法のヒント

---

## 15. リセット

```python
arduino_test.reset()
```

- `arduino_test` 側が制御
- 状態復帰可能

---

## 16. コア機能（Uno）

- TEST_CASE
- ASSERT_TRUE
- log
- attachText
- config
- requirement
- sync
- skip
- reset

---

## 17. 拡張機能

- attachBinary
- 画像
- 電流測定
- トレンド
- 外部信号検証

---

## 18. 非対象

- 通信プロトコル詳細
- メッセージ形式
- 内部実装
- 高度なテストフレームワーク機能

---

## 19. 結論

```
ArduTest:
  Arduino上でテスト・評価を定義

arduino_test:
  pytest-embedded-arduino-cli 上で実行・収集

特徴:
  - シンプル
  - 拡張可能
  - 実機評価に強い
```
