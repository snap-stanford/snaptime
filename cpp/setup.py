#!/usr/bin/env python

from distutils.core import setup, Extension
import os
import subprocess

os.environ['CXX'] = '/usr/bin/g++'
os.environ['CC'] = '/usr/bin/g++'
cwd = os.path.dirname(os.path.realpath(__file__))

fillData_module = Extension('_fillData',
                        sources = [cwd+'/fillData.i', cwd+'/fillData.cpp',cwd+'/create_data.cpp'],
                        include_dirs = [cwd+'/../../../../snap/snap-core/',cwd+'/../../../../snap/glib-core/'],
                        extra_objects = [cwd+'/../../../../snap/snap-core/Snap.o'],
                        swig_opts = ['-c++','-I '+cwd+'/fillData.i'],
                        extra_compile_args = ['-Wall','-O2','-pg','-std=c++11', '-fopenmp', '-ftree-vectorize','-fPIC'],
                        extra_link_args = ['-lrt','-lgomp'] #required for older glibc versions
                           )

setup (name = 'fillData',
       version = '0.1',
       author      = "Abhijit Sharang",
       ext_modules = [fillData_module],
       py_modules = ["fillData"],
       )

