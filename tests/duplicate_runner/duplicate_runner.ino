#include <ArduTest.h>

static void duplicate_one()
{
  ASSERT_TRUE(true);
}

static void duplicate_two()
{
  ASSERT_TRUE(true);
}

static ArduTestCase duplicate_case_one;
static ArduTestCase duplicate_case_two;
static ArduTestRegistrar duplicate_registrar_one(&duplicate_case_one, "duplicate_name", duplicate_one);
static ArduTestRegistrar duplicate_registrar_two(&duplicate_case_two, "duplicate_name", duplicate_two);

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
