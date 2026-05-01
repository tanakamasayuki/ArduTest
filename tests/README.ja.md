# ArduTest tests

このディレクトリは、`pytest-embedded-arduino-cli` から ArduTest ライブラリを動かすための一時的なテスト runner 置き場です。

`protocol_dut_runner` は、`dut.write(...)` / `dut.expect_exact(...)` で ArduTest protocol を直接確認します。

`protocol_runner` は、`arduino_test` fixture から `HELLO` / `LIST` / `RUN` の最小 protocol 経由で実行します。

例:

```sh
uv run pytest env/ArduTest/tests/protocol_dut_runner --profile uno --port=/dev/ttyACM0
uv run pytest env/ArduTest/tests/protocol_runner --profile uno --port=/dev/ttyACM0
```

`env/` 配下なので、このテスト runner は通常の git 管理対象には含めません。
