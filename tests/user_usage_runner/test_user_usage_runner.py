from pathlib import Path


def _artifact_path(request, test_name, filename):
    artifact_dir = Path(request.config.getoption("arduino_test_artifact_dir"))
    if not artifact_dir.is_absolute():
        artifact_dir = Path(request.config.rootpath) / artifact_dir
    return artifact_dir / test_name / filename


def test_user_facing_ardutest_flow(request, arduino_test):
    text_artifact = _artifact_path(request, "test_required_settings", "sample_rate.txt")
    binary_artifact = _artifact_path(request, "test_required_settings", "sample.bin")
    text_artifact.unlink(missing_ok=True)
    binary_artifact.unlink(missing_ok=True)

    tests = arduino_test.list_tests()

    assert [test.name for test in tests] == ["test_smoke", "test_required_settings"]
    assert tests[1].requirements == ("measurement.current",)
    assert tests[1].required_configs == ("sample_rate",)

    smoke = arduino_test.run("test_smoke")[0]

    assert smoke.status == "passed"
    assert smoke.logs == ["smoke test ran"]

    skipped = arduino_test.run("test_required_settings")[0]

    assert skipped.status == "skipped"
    assert skipped.skip_reason == "missing capability: measurement.current"
    assert not text_artifact.exists()
    assert not binary_artifact.exists()

    arduino_test.set_capability("measurement.current")
    arduino_test.set_config("sample_rate", 1000)

    configured = arduino_test.run("test_required_settings")[0]

    assert configured.status == "passed"
    assert configured.logs == ["required settings available"]
    assert configured.metrics == {"sample_rate": [1000]}
    assert configured.artifacts == {"sample_rate.txt": "1000"}
    assert text_artifact.read_text(encoding="utf-8") == "1000"
    assert binary_artifact.read_bytes() == b"AT\n\x01"
