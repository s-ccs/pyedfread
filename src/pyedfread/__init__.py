"""Read eye-tracking information from EyeLink EDF files."""

from pyedfread.parse import read_edf
from pyedfread.edf_read import read_preamble, read_messages, read_calibration
