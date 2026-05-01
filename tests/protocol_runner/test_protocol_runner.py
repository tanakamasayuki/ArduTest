def test_ardutest_protocol_list_and_run(monkeypatch, arduino_test):
    monkeypatch.setenv("ARDUINO_TEST_CAP_MEASUREMENT_CURRENT", "true")
    monkeypatch.setenv("ARDUINO_TEST_CONFIG_SAMPLE_RATE", "1000")

    tests = arduino_test.list_tests()

    assert [test.name for test in tests] == [
        "test_true_passes",
        "test_metric_and_artifact",
    ]
    assert tests[1].requirements == ("measurement.current",)
    assert tests[1].required_configs == ("sample_rate",)

    results = arduino_test.run("test_metric_and_artifact")

    assert [result.name for result in results] == ["test_metric_and_artifact"]
    assert results[0].metrics == {"sample_rate": [1000], "example_value": [42]}
    assert results[0].artifacts == {"note.txt": "hello from ArduTest"}
