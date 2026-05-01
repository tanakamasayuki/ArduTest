#include <ArduTest.h>

TEST_CASE(test_true_passes) {
  ArduTest.log("test_true_passes");
  ASSERT_TRUE(true);
}

TEST_CASE(test_metric_and_artifact) {
  ArduTest.reportMetric("example_value", 42L);
  ArduTest.attachText("note.txt", "hello from ArduTest");
  ASSERT_EQ(42, 42);
}

void setup() {
  Serial.begin(115200);
  ArduTest.begin();
}

void loop() {
  ArduTest.poll();
}
