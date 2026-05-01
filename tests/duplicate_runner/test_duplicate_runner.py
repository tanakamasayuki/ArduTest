def test_duplicate_test_name_reports_protocol_error(dut):
    dut.expect_exact("AT < READY")

    dut.write("AT > LIST\n")
    dut.expect_exact("AT < ERROR duplicate_test 14")
    dut.expect_exact("duplicate_name")
