def test_user_facing_ardutest_flow(arduino_test):
    arduino_test.set_capability("measurement.current")
    arduino_test.set_config("sample_rate", 1000)

    arduino_test.run()
