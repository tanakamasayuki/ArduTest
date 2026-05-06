# ArduTest tests

このディレクトリは `pytest-embedded-arduino-cli` から ArduTest library を検証するための integration test です。

## ユーザー向けの使い方

- `user_usage_runner`
  - `arduino_test` fixture を使った推奨フローを示します。
  - test 一覧取得、単純な test 実行、requirement/config 不足による skip、`set_capability()`、`set_config()`、metric、text artifact、binary artifact を確認します。

project 側の pytest test をどう書くかを知りたい場合は、まずこの runner を見る想定です。

## ArduTest library / protocol の検証

- `protocol_dut_runner`
  - `dut.write(...)` / `dut.expect_exact(...)` で ArduTest protocol を直接確認します。
- `protocol_runner`
  - `arduino_test` fixture から `HELLO` / `LIST` / `RUN` の protocol 経由で実行します。
- `assertion_runner`
  - assertion failure が `FAIL` event と `RESULT failed` になることを確認します。
- `duplicate_runner`
  - 重複 test name が `ERROR duplicate_test` になることを確認します。
- `protocol_error_runner`
  - unknown command/test、SET_CONFIG の境界、config store 上限、CLEAR_CONFIG を確認します。

これらの runner は主に ArduTest 実装の coverage のためのものです。通常のユーザー test では assert しなくてよい protocol detail も意図的に確認します。

pytest plugin repository と ArduTest repository を同じ親ディレクトリに checkout している場合、pytest plugin 側から次のように実行できます。

```sh
uv run --env-file .env pytest --profile=host ../ArduTest/tests/
uv run --env-file .env pytest --profile=uno --port=/dev/ttyUSB0 ../ArduTest/tests/
```
