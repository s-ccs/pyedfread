"""Read eye-tracking information from EyeLink EDF files."""


import sys
if sys.platform.startswith("win32"):
    import os
    import platform
    srr_basedir = r"C:\Program Files (x86)\SR Research\EyeLink"
    if platform.architecture()[0] == "64bit":
        arch_dir = 'x64'
    else:
        arch_dir = ''
    # due to a change in python 3.8, the searchpath is not automatically added anymore
    os.add_dll_directory(os.path.join(srr_basedir, 'libs', arch_dir))
           
from pyedfread.parse import read_edf
from pyedfread.edf_read import read_preamble, read_messages, read_calibration
