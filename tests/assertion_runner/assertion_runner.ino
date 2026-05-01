#include <ArduTest.h>

TEST_CASE(test_assert_true_fails)
{
  ASSERT_TRUE(false);
}

TEST_CASE(test_assert_false_fails)
{
  ASSERT_FALSE(true);
}

TEST_CASE(test_assert_eq_fails)
{
  ASSERT_EQ(1, 2);
}

TEST_CASE(test_assert_ne_fails)
{
  ASSERT_NE(3, 3);
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
