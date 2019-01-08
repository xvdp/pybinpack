"""
xvdp 2018
"""
import os
import re
import sys
import platform
import subprocess

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        #self.debug = True

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        cfg = 'Debug' if self.debug else 'Release'
        #print('\n\n\n',cfg,'\n\n\n')
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2**32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)

def readversion():
    with open('src/main.cpp', 'r') as _fi:
        version = _fi.read().split('version__"')[1].split('"')[1]

    with open('pybinpack/version.py', 'w') as _fi:
        _fi.write("version='"+version+"'")
    
    return version


SETUP_ARGS = dict(
    name='pybinpack',
    version=readversion(),
    author='xvdp',
    author_email='xvdpahlen@gmail.com',
    description='rectangle binpack with pybind11',
    long_description='',
    install_requires=['numpy', 'pillow', 'scipy', 'matplotlib'],
    ext_modules=[CMakeExtension('pybinpack._pybinpack')],
    cmdclass=dict(build_ext=CMakeBuild),
    zip_safe=False,
    packages=['pybinpack']
)

try:
    setup(**SETUP_ARGS)
except subprocess.CalledProcessError:
    print('Failed to build extension!')
    del SETUP_ARGS['ext_modules']
    setup(**SETUP_ARGS)


# has to be manually installed; 'jupyter', matplotlib, scipy