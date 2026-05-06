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

## pytest plugin 依存の更新手順

`tests/uv.lock` は、ArduTest の `tests/` project から実行するときに使う `pytest-embedded-arduino-cli` の正確な version を固定します。

通常の更新手順として `uv.lock` を削除するのは避けます。削除すると全依存が再解決されるため、関係ない package まで更新され、差分確認が難しくなります。

`tests/pyproject.toml` で許可されている範囲内で pytest plugin だけを最新にする場合:

```sh
cd tests
uv lock --upgrade-package pytest-embedded-arduino-cli
uv sync
```

特定の release 範囲へ固定したい場合は、先に `tests/pyproject.toml` を更新してから lock file を更新します。

```sh
cd tests
uv add "pytest-embedded-arduino-cli>=1.2,<1.3"
uv sync
```

pytest plugin をローカル開発中に確認したい場合は、ArduTest 側の lock file は変更せず、`pytest-embedded-arduino-cli` の checkout から実行します。

```sh
cd ../pytest-embedded-arduino-cli
uv run pytest ../ArduTest/tests/
```
