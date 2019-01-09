pybinpack
=========
@xvdp 2018
![Max Rects BinPack single bin](https://github.com/xvdp/pybinpack/blob/master/examples/0_MRBP_h0_b1_o1.png)

* Rectangle Bin packer using pybind11
* Required Python > 3.4, GCC

Rectangle packing based on RectangleBinPack by Jukka Jylänki
https://github.com/juj/RectangleBinPack 

Modifications by xvdp 2018
* compile with pybind11 for use in python
* added rectangle indexing within bin (to address image fitting within bin packing)
* added simple heuristics to determine minimum bin size for a dataset
* for what looks like the more efficient method (MaxRectsBinPack) implemented multiple fix sized bins
* on this branch / removed inefficient methods

1. Clone
    ```
    $ git clone https://gitlab.com/xkunglu/pybinpack --recursive
    $ git checkout --track origin/cpp_only #this branch
    ```
Required packages: numpy, jupyter, matplotlib and pybind. None of them have to be installed, this project will install and or build as required.
jupyter has to be installed manually, it threw some errors for me.


2. Build
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

 3. Examples
 
    https://github.com/xvdp/pybinpack/blob/master/examples/BinPackExample.ipynb
    Some Images from notebook:
Multiple same size bins  
![Image](https://github.com/xvdp/pybinpack/blob/master/examples/3_MRBP_h0_b11.png)
Single Bin, only rectangles shown
![Image](https://github.com/xvdp/pybinpack/blob/master/examples/4_MRBP_h1_b1_o105.png)

    
    
