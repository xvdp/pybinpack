pybinpack
=========
@xvdp 2018

* Rectangle Bin packer using pybind11
* Required Python > 3.4, GCC

Rectangle packing based on RectangleBinPack by Jukka Jylänki
https://github.com/juj/RectangleBinPack 

Modifications by xvdp 2018
* compile with pybind11 for use in python
* added rectangle indexing within bin (to address image fitting within bin packing)
* added simple heuristics to determine minimum bin size for a dataset
* for what looks like the more efficient method (MaxRectsBinPack) implemented multiple fix sized bins


This project was built with the purpose of understanding:

    a. how pybind11 works; It's great; starting from example project, includes, compiles and links almost flawlessly.

    b. Test speed of binning vs python version of binpacker.
        - Measured against all python version https://github.com/secnot/rectpack (on a 300 image dataset Hymenoptere).
        Both projects are sequential, cpp RectangleBinPack computes ~ 10e5 faster than py rectpack.

    c. because I dont like looking at all the empty area when looking at images and datasets

1. Clone
    ```
    $ git clone https://gitlab.com/xkunglu/pybinpack --recursive
    ```
Required packages: numpy, jupyter, matplotlib and pybind. None of them have to be installed, this project will install and or build as required.
jupyter has to be installed manually, it threw some errors for me.


2. build
    ```
    $ cd pybinpack
    $ ./install.sh # this will run python setup.py install and run quick test. 
    ```
    Only tested on Ubuntu 16 and 18, with python 3.5 and 3.7 on Conda environments.

    Caveats. 
    * On conda, when making code modifications, sometimes the build goes toconda root environment instead of current environment; 
    not sure why.
    To fix that problem manually uninstall from conda root with `pip uninstall pybinpack` then run ./install.sh again.
    * conda install doesn't really work with current setup-haven't dug up why, thats why I pip install.

    * bin.width and bin.height are reversed

 3. example

    BinPackExample.ipynb included
    


