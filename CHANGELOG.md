# Changelog / 変更履歴

## Unreleased
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
