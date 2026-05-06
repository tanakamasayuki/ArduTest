from pathlib import Path


def _artifact_path(request, test_name, filename):
    artifact_dir = Path(request.config.getoption("arduino_test_artifact_dir"))
    if not artifact_dir.is_absolute():
        artifact_dir = Path(request.config.rootpath) / artifact_dir
    return artifact_dir / test_name / filename


def test_ardutest_protocol_list_run_and_save_artifact(monkeypatch, request, arduino_test):
    monkeypatch.setenv("ARDUINO_TEST_CAP_MEASUREMENT_CURRENT", "true")
    monkeypatch.setenv("ARDUINO_TEST_CONFIG_SAMPLE_RATE", "1000")
    artifact_path = _artifact_path(request, "test_metric_and_artifact", "note.txt")
    artifact_path.unlink(missing_ok=True)

    tests = arduino_test.list_tests()

    assert [test.name for test in tests] == [
        "test_true_passes",
        "test_metric_and_artifact",
    ]
    assert tests[1].requirements == ("measurement.current",)
    assert tests[1].required_configs == ("sample_rate",)
    assert not artifact_path.exists()

    results = arduino_test.run("test_metric_and_artifact")

    assert [result.name for result in results] == ["test_metric_and_artifact"]
    assert results[0].metrics == {"sample_rate": [1000], "example_value": [42]}
    assert results[0].artifacts == {"note.txt": "hello from ArduTest"}
    assert artifact_path.read_text(encoding="utf-8") == "hello from ArduTest"
