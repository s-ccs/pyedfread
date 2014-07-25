# -*- coding: utf-8 -*-
'''
Setup file for pyedf. Builds cython extension needed for using
the C EDF access API.

To build locally call
    python setup.py build_ext --inplace

'''

from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
from Cython.Build import cythonize
import numpy
ext_module = Extension(
        "edfread", 
        ['edfread.pyx'],
        include_dirs=[numpy.get_include(), 
            'include/'],
        library_dirs=['lib/'],
        libraries=['edfapi'],
        extra_compile_args=['-fopenmp'],
        extra_link_args=['-fopenmp'],
)

ext_data = Extension(
        "edf_data", 
        ['edf_data.pyx'],
        include_dirs=[numpy.get_include(), 
            'include/'],
        library_dirs=['lib/'],
        libraries=['edfapi'],
        extra_compile_args=['-fopenmp'],
        extra_link_args=['-fopenmp'],
)
setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [ext_data,ext_module],
)
