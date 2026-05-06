#include <ArduTest.h>

TEST_CASE(test_true_passes)
{
  ArduTest.log("running test_true_passes");
  ASSERT_TRUE(true);
}

TEST_CASE(test_metric_and_artifact)
{
  const uint8_t binary[] = {0x00, 0x01, 0x0a, 0xff};

  ASSERT_EQ(1000, ArduTest.configInt("sample_rate"));
  ArduTest.reportMetric("sample_rate", ArduTest.configInt("sample_rate"));
  ArduTest.reportMetric("example_value", 42L);
  ArduTest.attachText("note.txt", "hello from ArduTest");
  ArduTest.attachBinary("data.bin", "application/octet-stream", binary, sizeof(binary));
  ASSERT_EQ(42, 42);
}
ARDUTEST_REQUIRE(test_metric_and_artifact, "measurement.current");
ARDUTEST_REQUIRE_CONFIG(test_metric_and_artifact, "sample_rate");

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
