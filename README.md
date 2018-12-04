pybinpack
=========
author @xvdp 2018

* Rectangle Bin packer using pybind11
* Required Python > 3.4, GCC

Based on RectangleBinPack by Jukka Jylänki
https://github.com/juj/RectangleBinPack 

Modifications by xvdp 2018
* compile with pybind11 for use in python
* added bin indexing 
* added bin heuristics

* wip or tbs: 
    * binpack algorithms = currentlyu sing maxrectbinpack - needs to be exposed
    * implementation of multiple fixed size bins
    * implementation of order sort bin
    * multiprocessing

Caveat:
    This is an incomplete project, built with the purpose of understanding:

        a. how pybind11 works. 

        running verdict. pybind is much more user friendly than boost or swig. It exposes much python indide cpp and seems to deal relatively weel with cpp inside python.
        It took me about one day to go thru most of the communication and setup, so it has an easy entry bar. 
        Connections to Eigen, Boost and STL in cpp, and numpy in python make this a very useful framework.


        b. Test speed of binning vs python version of binpacker.

        - Preliminary results measured against all python version https://github.com/secnot/rectpack (on a 300 image dataset Hymenopter ). Both projects are sequential, cpp RectangleBinPack computes ~ 10e5 faster than py rectpack.


    Extending and completing it should be quite simple. 


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
    ```
    $ python
    >>> import pybinpack as pb
    # rectangle array can be 2d numpy array of sizes or 
    >>> ar=[[120,120],[134,162],[120,120],[134,162],[120,120],[134,162],[120,120],[134,162],[134,162],[120,120],[134,162],[5,5],[150,150],[5,5]]
    >>> width = 0
    >>> height = 0
    # if (width * height) <100 Bin height and width is automatically computed to sqrt(total rectangle area * 1.2)
    >>> b = pb.Binnit(width, height)
    >>> out = b.pack(ar)
    # out is a np array [bin, index, x, y, width, height, ... ]
    ```


