pybinpack
=========

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
    This is an incomplete project, built with the purpose of understanding a. how pybind works, b. test speed of binning vs python version of binpacker.
    Extending and completing it should be quite simple. 


1. Clone
    ```
    $ git clone https://gitlab.com/xkunglu/pybinpack --recursive
    ```
Will install numpy and jupyter and download pybind11 if not found

2. build
    ```
    $ cd pybinpack
    $ ./install.sh # this will run python setup.py install and run quick test
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


