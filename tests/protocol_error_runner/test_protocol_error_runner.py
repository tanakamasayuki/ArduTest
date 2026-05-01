import re


def expect_error(dut, code: str, payload: str) -> None:
    dut.expect_exact(f"AT < ERROR {code} {len(payload)}")
    if payload:
        dut.expect_exact(payload)


def set_config(dut, name: str, value: str) -> None:
    dut.write(f"AT > SET_CONFIG {name} {len(value)}\n{value}")


def run_pass(dut, name: str) -> None:
    dut.write(f"AT > RUN {name}\n")
    dut.expect_exact(f"AT < RUNNING {name}")
    dut.expect_exact(f"AT < RESULT {name} passed")


def test_protocol_error_and_config_boundaries(dut):
    dut.expect_exact("AT < READY")

    max_name = "n" * 31
    max_value = "1" * 48
    set_config(dut, max_name, max_value)
    dut.write("AT > CLEAR_CONFIG\n")

    dut.write("AT > NOPE\n")
    expect_error(dut, "unknown_command", "AT > NOPE")

    dut.write("AT > RUN missing_test\n")
    expect_error(dut, "unknown_test", "missing_test")

    dut.write("AT > SET_CONFIG missing_length\n")
    expect_error(dut, "invalid_config", "missing_length")

    dut.write("AT > SET_CONFIG  0\n")
    expect_error(dut, "invalid_config", "invalid_name")

    dut.write("AT > SET_CONFIG bad_length nope\n")
    expect_error(dut, "invalid_config", "invalid_length")

    value_too_large = "x" * 49
    set_config(dut, "too_large", value_too_large)
    expect_error(dut, "invalid_config", "value_too_large")

    dut.write(("AT > " + "X" * 75 + "\n"))
    expect_error(dut, "unknown_command", "line_too_long")

    set_config(dut, "k1", "123")
    set_config(dut, "k2", "2")
    set_config(dut, "k3", "3")
    set_config(dut, "k4", "4")
    run_pass(dut, "test_config_value")

    set_config(dut, "k5", "5")
    expect_error(dut, "invalid_config", "store_full")

    set_config(dut, "k1", "123")
    run_pass(dut, "test_config_value")

    dut.write("AT > CLEAR_CONFIG\n")
    run_pass(dut, "test_config_default_after_clear")

    dut.write("AT > LIST\n")
    dut.expect_exact("AT < TEST test_config_value")
    dut.expect_exact("AT < TEST test_config_default_after_clear")
    dut.expect_exact("AT < END_LIST")
