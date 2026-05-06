#include <ArduTest.h>

TEST_CASE(test_smoke)
{
  ArduTest.log("smoke test ran");
  ASSERT_TRUE(true);
}

TEST_CASE(test_required_settings)
{
  const int sampleRate = ArduTest.configInt("sample_rate");
  const uint8_t bytes[] = {0x41, 0x54, 0x0a, 0x01};

  ArduTest.log("required settings available");
  ArduTest.reportMetric("sample_rate", sampleRate);
  ArduTest.attachText("sample_rate.txt", ArduTest.config("sample_rate"));
  ArduTest.attachBinary("sample.bin", "application/octet-stream", bytes, sizeof(bytes));
  ASSERT_EQ(1000, sampleRate);
}
ARDUTEST_REQUIRE(test_required_settings, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_required_settings, "sample_rate");

void setup()
{
  Serial.begin(115200);
  delay(1000);
  ArduTest.begin();
}

void loop()
{
  ArduTest.poll();
}
