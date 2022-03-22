"""Test reading from different sections of EDF data."""

from pkg_resources import resource_filename
import pytest
import edfread as edf


@pytest.fixture(scope="session")
def edf_file():
    filepath = resource_filename("edfread", "data/SUB001.EDF")
    return filepath


def test_read_preamble(edf_file):
    """Test reading the preamble section."""
    preamble = edf.read_preamble(edf_file)
    expected = """** DATE: Tue Jun 27 02:04:46 2017
** TYPE: EDF_FILE BINARY EVENT SAMPLE TAGGED
** VERSION: EYELINK II 1
** SOURCE: EYELINK CL
** EYELINK II CL v5.04 Sep 25 2014
** CAMERA: Eyelink GL Version 1.2 Sensor=AI7
** SERIAL NUMBER: CLG-BBF30
** CAMERA_CONFIG: BBF30200.SCD
"""
    assert preamble == expected


def test_read_messages(edf_file):
    """Read reading messages."""
    messages = edf.read_messages(edf_file)
    expected = [
        'DISPLAY_COORDS 0 0 1919 1079',
        '',
        'RECCFG CR 1000 2 1 R',
        'ELCLCFG MTABLER',
        'GAZE_COORDS 0.00 0.00 1919.00 1079.00',
        'THRESHOLDS R 85 220',
        'ELCL_WINDOW_SIZES 176 188 0 0',
        'ELCL_PROC CENTROID (3)',
        'ELCL_PCR_PARAM 5 3.0',
        'ELCL_PCR_PARAM 5 3.0',
        '!MODE RECORD CR 1000 2 1 R'
    ]
    assert messages[:len(expected)] == expected
