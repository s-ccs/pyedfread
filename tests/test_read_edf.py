"""
Test pyedfread by comparing against edf2asc output.

Only compares readout of samples at the moment.
"""

from pkg_resources import resource_filename
import pytest
import numpy as np
import pyedfread as edf


@pytest.fixture(scope="session")
def edf_data():
    """Sample of read-in EDF data."""
    edf_file = resource_filename("pyedfread", "data/SUB001.EDF")
    samples, events, messages = edf.read_edf(edf_file)
    data = {"samples": samples, "events": events, "messages": messages}
    return data


def test_messages(edf_data):
    """Compare messages to known values."""
    # have one extra sample compared to original and one extra message with
    # display coordinates
    messages = edf_data["messages"]
    np.testing.assert_array_equal(
        messages["sample"].to_numpy()[1:], np.arange(6, 301, 5)
    )
    # original assumed shape of (58, 5)
    assert messages.shape == (60, 31)
    # original looked for this sample at index 57
    assert messages.loc[58, 'trialid_time'] == 502911


def test_events(edf_data):
    """Compare events to known values."""
    events = edf_data["events"]
    assert events.shape == (485, 30)
    np.testing.assert_allclose(events["gavx"].mean(), 420.842680343156)
    np.testing.assert_allclose(events["gavy"].mean(), 205.318144115959)


def test_validate_against_edf2asc(edf_data):
    """Compare EDF parsing to EDF2ASC output."""
    samples = edf_data["samples"]
    samples = samples.loc[:, ("time", "gx_right", "gy_right", "pa_right")]
    samples = samples.round(1).set_index("time")

    # check the first n samples
    n = 100
    asc_file = resource_filename("pyedfread", "data/SUB001.asc")
    with open(asc_file) as asc:
        for i, line in enumerate(asc):
            if i == n:
                break
            x = (
                line.strip()
                .replace("...", "")
                .replace("\t", "")
                .replace("  ", " ")
                .split(" ")
            )
            if len(x) == 4:
                try:
                    # SAMPLE, X, Y, PA
                    x = [float(n) for n in x]
                    row = samples.loc[x[0], :]
                    np.testing.assert_allclose(x[1:], row.to_numpy())
                except ValueError:
                    pass


def test_ignore_samples():
    """Test option to ignore samples in EDF file."""
    edf_file = resource_filename('pyedfread', 'data/SUB001.EDF')
    samples, events, messages = edf.read_edf(edf_file, ignore_samples=True)
    assert samples.shape[0] == 0
