#include <ArduTest.h>

TEST_CASE(test_config_value)
{
  ASSERT_EQ(123, ArduTest.configInt("k1"));
}

TEST_CASE(test_config_default_after_clear)
{
  ASSERT_EQ(7, ArduTest.configInt("k1", 7));
}

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
