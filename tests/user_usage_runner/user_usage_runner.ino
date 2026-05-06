#include <ArduTest.h>

// en: Define one ArduTest test with TEST_CASE. The pytest side runs these tests through arduino_test.run().
// ja: TEST_CASE で ArduTest のテストを定義します。pytest 側は arduino_test.run() でこれらを実行します。
TEST_CASE(test_smoke)
{
  // en: Logs are collected by pytest and are useful when a passing test still needs diagnostic context.
  // ja: log は pytest 側で収集されます。成功したテストにも診断用の文脈を残したいときに使えます。
  ArduTest.log("smoke test ran");
  ASSERT_TRUE(true);
}

// en: This test needs a host-provided capability and config value before it can run.
// ja: このテストは、実行前に host 側から capability と config 値を渡す必要があります。
TEST_CASE(test_required_settings)
{
  // en: configInt reads the value sent by arduino_test.set_config("sample_rate", 1000).
  // ja: configInt は arduino_test.set_config("sample_rate", 1000) で送られた値を読み取ります。
  const int sampleRate = ArduTest.configInt("sample_rate");
  const uint8_t bytes[] = {0x41, 0x54, 0x0a, 0x01};

  ArduTest.log("required settings available");
  // en: Metrics are kept with the ArduTest result for optional assertions or reporting.
  // ja: metric は ArduTest の結果に紐づけて保持され、必要に応じて assert や report に使えます。
  ArduTest.reportMetric("sample_rate", sampleRate);
  // en: Text and binary artifacts are saved by pytest under the configured artifact directory.
  // ja: text / binary artifact は pytest 側で設定済みの artifact directory に保存されます。
  ArduTest.attachText("sample_rate.txt", ArduTest.config("sample_rate"));
  ArduTest.attachBinary("sample.bin", "application/octet-stream", bytes, sizeof(bytes));
  ASSERT_EQ(1000, sampleRate);
}
// en: REQUIRE declares an environment capability. pytest satisfies it with set_capability().
// ja: REQUIRE は環境 capability を宣言します。pytest 側では set_capability() で満たします。
ARDUTEST_REQUIRE(test_required_settings, "measurement.current");
// en: REQUIRE_CONFIG declares a required config value. pytest sends it with set_config().
// ja: REQUIRE_CONFIG は必須 config 値を宣言します。pytest 側では set_config() で送ります。
ARDUTEST_REQUIRE_CONFIG(test_required_settings, "sample_rate");

void setup()
{
  Serial.begin(115200);
  delay(1000);
  // en: begin starts the ArduTest protocol service on Serial.
  // ja: begin は Serial 上で ArduTest protocol service を開始します。
  ArduTest.begin();
}

void loop()
{
  // en: poll lets ArduTest receive pytest commands such as LIST and RUN.
  // ja: poll により、ArduTest は LIST や RUN などの pytest command を受信できます。
  ArduTest.poll();
}
