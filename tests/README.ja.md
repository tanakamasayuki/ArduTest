# ArduTest tests

このディレクトリは `pytest-embedded-arduino-cli` から ArduTest library を検証するための integration test です。

## ユーザー向けの使い方

- `user_usage_runner`
  - 小さな `.ino` sketch とシンプルな pytest test で推奨フローを示します。
  - Arduino sketch 側で test case、requirement、config、log、metric、text artifact、binary artifact の書き方を示します。
  - pytest 側は `set_capability()` / `set_config()` で固定値を渡し、`arduino_test.run()` を呼ぶだけにしています。

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
