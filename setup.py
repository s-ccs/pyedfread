# -*- coding: utf-8 -*-
"""
Setup file for pyedf. Builds cython extension needed for using
the C EDF access API.

To build locally call
    python setup.py build_ext --inplace

"""

import sys
#from distutils.core import setup
#from distutils.extension import Extension
from Cython.Distutils import build_ext
from setuptools import setup,find_packages, Extension
import numpy


if sys.platform.startswith("darwin"):
    args = {
        "include_dirs": [
            numpy.get_include(),
            "/Library/Frameworks/edfapi.framework/Headers",
        ],
        "extra_link_args": ["-F/Library/Frameworks/", "-framework", "edfapi"],
        "extra_compile_args": ["-w"],
    }
elif sys.platform.startswith("win32"):
    import os
    import platform

    srr_basedir = r"C:\Program Files (x86)\SR Research\EyeLink"
    if platform.architecture()[0] == "64bit":
        arch_dir = 'x64'
        lib_names = ['edfapi64']
    else:
        arch_dir = ''
        lib_names = ['edfapi']
    args = {'include_dirs': [numpy.get_include(), os.path.join(srr_basedir, 'Includes'),os.path.join(srr_basedir, 'Includes',"eyelink")],
            'library_dirs': [os.path.join(srr_basedir, 'libs', arch_dir)],
            'libraries': lib_names
            }

else:  # linux, unix, cygwin
    args = {
        "include_dirs": [numpy.get_include(), "include/", '/usr/include/EyeLink/'],
        "library_dirs": ["lib/"],
        "libraries": ["edfapi"],
        "extra_compile_args": ["-fopenmp"],
        "extra_link_args": ["-fopenmp"],
    }

print(args)
ext_module = Extension("pyedfread.edf_read", ["src/pyedfread/edf_read.pyx"], **args)

ext_data = Extension("pyedfread.edf_data", ["src/pyedfread/edf_data.pyx"], **args)

setup(
    cmdclass={'build_ext': build_ext},
    ext_modules=[ext_data, ext_module],
    packages = find_packages(where='src'),
    package_dir = {"": "src"},
)
