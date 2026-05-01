import re


def test_ardutest_protocol_over_dut(dut):
    dut.expect_exact("AT < READY")

    dut.write("AT > HELLO 1\n")
    dut.expect(re.compile(rb"AT < HELLO 1 ArduTest \d+\.\d+\.\d+"))

    dut.write("AT > LIST\n")
    dut.expect_exact("AT < TEST test_true_passes")
    dut.expect_exact("AT < TEST test_metric_and_artifact")
    dut.expect_exact("AT < END_LIST")

    dut.write("AT > RUN test_true_passes\n")
    dut.expect_exact("AT < RUNNING test_true_passes")
    dut.expect_exact("AT < LOG test_true_passes 24")
    dut.expect_exact("running test_true_passes")
    dut.expect_exact("AT < RESULT test_true_passes passed")

    dut.write("AT > RUN test_metric_and_artifact\n")
    dut.expect_exact("AT < RUNNING test_metric_and_artifact")
    dut.expect_exact("AT < METRIC test_metric_and_artifact example_value 42")
    dut.expect_exact("AT < ARTIFACT_TEXT test_metric_and_artifact note.txt text/plain 19")
    dut.expect_exact("hello from ArduTest")
    dut.expect_exact("AT < RESULT test_metric_and_artifact passed")
