import re


def _run_and_expect_failure(dut, test_name: str, payload: bytes) -> None:
    dut.write(f"AT > RUN {test_name}\n")
    dut.expect_exact(f"AT < RUNNING {test_name}")
    dut.expect(re.compile(rb"AT < FAIL " + test_name.encode() + rb" [^ ]+ \d+ " + str(len(payload)).encode() + rb"\r?\n"))
    dut.expect_exact(payload.decode())
    dut.expect_exact(f"AT < RESULT {test_name} failed")


def test_assertions_report_fail_events(dut):
    dut.expect_exact("AT < READY")

    dut.write("AT > LIST\n")
    dut.expect_exact("AT < TEST test_assert_true_fails")
    dut.expect_exact("AT < TEST test_assert_false_fails")
    dut.expect_exact("AT < TEST test_assert_eq_fails")
    dut.expect_exact("AT < TEST test_assert_ne_fails")
    dut.expect_exact("AT < END_LIST")

    _run_and_expect_failure(dut, "test_assert_true_fails", b"false")
    _run_and_expect_failure(dut, "test_assert_false_fails", b"!(true)")
    _run_and_expect_failure(dut, "test_assert_eq_fails", b"1=1 2=2")
    _run_and_expect_failure(dut, "test_assert_ne_fails", b"3 != 3")
