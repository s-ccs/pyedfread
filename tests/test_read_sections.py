"""Test reading from different sections of EDF data."""

from pkg_resources import resource_filename
import pytest
import pyedfread as edf


@pytest.fixture(scope="session")
def edf_file():
    filepath = resource_filename("pyedfread", "data/SUB001.EDF")
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
