# ArduTest tests

このディレクトリは `pytest-embedded-arduino-cli` から ArduTest library を検証するための integration test です。

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

pytest plugin repository と ArduTest repository を同じ親ディレクトリに checkout している場合、pytest plugin 側から次のように実行できます。

```sh
uv run --env-file .env pytest --profile=host ../ArduTest/tests/
uv run --env-file .env pytest --profile=uno --port=/dev/ttyUSB0 ../ArduTest/tests/
```
