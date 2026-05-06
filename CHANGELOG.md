# Changelog / 変更履歴

## Unreleased
- (EN) Expand README usage documentation for what to write in the Arduino sketch and pytest test, including requirements, config, logs, metrics, text artifacts, and binary artifacts.
- (JA) Arduino sketch と pytest test に何を書くかを、requirement、config、log、metric、text artifact、binary artifact を含めて README に詳しく追記。
- (EN) Add `tests/user_usage_runner` to separate user-facing pytest usage examples from ArduTest library/protocol coverage.
- (JA) ユーザー向け pytest 利用例と ArduTest library / protocol coverage を分けるため、`tests/user_usage_runner` を追加。
- (EN) Update pytest integration tests to use `arduino_test.set_config()` and `set_capability()` for fixed ArduTest values.
- (JA) pytest integration test の固定 ArduTest 値を `arduino_test.set_config()` / `set_capability()` で渡すように変更。
- (EN) Add `ArduTest.attachBinary()` for `ARTIFACT_BINARY` raw binary artifacts.
- (JA) `ARTIFACT_BINARY` の raw binary artifact を送信する `ArduTest.attachBinary()` を追加。
- (EN) Extend host integration coverage to verify text and binary artifacts are saved by pytest.
- (JA) text / binary artifact が pytest 側で保存されることを確認する host integration test を追加。

## 0.2.1
- (EN) Report duplicate test names as `ERROR duplicate_test` during LIST.
- (JA) 重複 test name を LIST 時に `ERROR duplicate_test` として報告するように変更。
- (EN) Add integration coverage for assertion failures, duplicate tests, protocol errors, and config store boundaries.
- (JA) assertion failure、重複 test、protocol error、config store 境界の integration test を追加。

## 0.2.0
- (EN) Initial experimental ArduTest library release.
- (JA) experimental な ArduTest ライブラリの初期リリース。
- (EN) Add protocol version 1 support for HELLO, LIST, SET_CONFIG, CLEAR_CONFIG, RUN, RUN_ALL, RESET_STATE, logs, metrics, text artifacts, assertion failures, and results.
- (JA) protocol version 1 として HELLO、LIST、SET_CONFIG、CLEAR_CONFIG、RUN、RUN_ALL、RESET_STATE、log、metric、text artifact、assertion failure、result に対応。
- (EN) Add TEST_CASE, ASSERT_TRUE, ASSERT_FALSE, ASSERT_EQ, ASSERT_NE, ARDUTEST_REQUIRE, and ARDUTEST_REQUIRE_CONFIG.
- (JA) TEST_CASE、ASSERT_TRUE、ASSERT_FALSE、ASSERT_EQ、ASSERT_NE、ARDUTEST_REQUIRE、ARDUTEST_REQUIRE_CONFIG を追加。
- (EN) Add Basic example and pytest integration runners for host and real-device smoke tests.
- (JA) Basic example と、host / 実機 smoke test 用の pytest integration runner を追加。
