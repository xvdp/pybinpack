pybinpack
=========
@xvdp 2018

* Rectangle Bin packer using pybind11
* Required Python > 3.4, GCC

Rectangle packing based on RectangleBinPack by Jukka Jylänki
https://github.com/juj/RectangleBinPack 

Modifications by xvdp 2018
* compile with pybind11 for use in python
* added bin indexing 
* added single bin heuristics

* wip or tbs: 
    * implementation of multiple fixed size bins
    * multiprocessing

Caveat:
    This is an incomplete project, built with the purpose of understanding:

        a. how pybind11 works; I like it

        b. Test speed of binning vs python version of binpacker.

        - Preliminary results measured against all python version https://github.com/secnot/rectpack (on a 300 image dataset Hymenoptere).
        Both projects are sequential, cpp RectangleBinPack computes ~ 10e5 faster than py rectpack.

1. Clone
    ```
    $ git clone https://gitlab.com/xkunglu/pybinpack --recursive
    ```
Required packages: numpy, jupyter, matplotlib and pybind. None of them have to be installed, this project will install and or build as required.


2. build
    ```
    $ cd pybinpack
    $ ./install.sh # this will run python setup.py install and run quick test. Since I only tested on a few setups, python 3.5-3.7 Ubuntu 16 and 18 on virgin conda envionments, build could fail.
    ```
 3. example

    BinPackExample.ipynb included
    


