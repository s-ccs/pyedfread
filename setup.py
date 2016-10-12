# -*- coding: utf-8 -*-
'''
Setup file for pyedf. Builds cython extension needed for using
the C EDF access API.

To build locally call
    python setup.py build_ext --inplace

'''

import sys
from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import numpy

if not 'darwin' in sys.platform:
    args = {'include_dirs':[numpy.get_include(),
           'include/'],
        'library_dirs':['lib/'],
        'libraries':['edfapi'],
        'extra_compile_args':['-fopenmp'],
        'extra_link_args':['-fopenmp']}

    ext_module = Extension(
        "pyedfread.edfread",
        ['pyedfread/edfread.pyx'],
        **args
    )
    ext_data = Extension(
        "pyedfread.edf_data",
        ['pyedfread/edf_data.pyx'],
        **args
    )
else:
    args = {'include_dirs':[numpy.get_include(), '/Library/Frameworks/edfapi.framework/Headers'],
        'extra_link_args':['-F/Library/Frameworks/', '-framework', 'edfapi'], 'extra_compile_args':["-w"]}

    ext_module = Extension(
        "pyedfread.edfread",
        ['pyedfread/edfread.pyx'],
        **args
    )
    ext_data = Extension(
        "pyedfread.edf_data",
        ['pyedfread/edf_data.pyx'],
        **args
    )

setup(name='pyedfread',
    version='0.1',
    description='Read SR-Research EDF files with python.',
    author='Niklas Wilming',
    cmdclass = {'build_ext': build_ext},
    packages=['pyedfread'],
    ext_modules = [ext_data,ext_module],
    scripts=['pyedfread/read_edf']
)
