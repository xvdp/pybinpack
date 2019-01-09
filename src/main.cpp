/*
rectangle bin packer
xvdp 2018 

pybind11 bindings BinPacking
*/

#include <pybind11/pybind11.h>
#include "BinPackManager.h"

using namespace std;
namespace py = pybind11;


PYBIND11_MODULE(pybinpack, m){
    m.doc() = R"pbdoc(
        pybinpack
        ---------
        .. currentmodule:: pybinpack
        .. autosummary:: 
           :toctree: _generate

           mul
    )pbdoc";

    py::class_<Binnit>binnit(m, "Binnit");

    binnit.def(py::init<const int &, const int &>(),  py::arg("width")=0,  py::arg("height")=0 )

        .def("pack", &Binnit::pack, R"pbdoc(Bin Packing simplified; 
        Returns a flat int array of size: 6*num rectangles, with info
            0: bin index
            1: image index
            2: position x
            3: position y
            5: width
            6: height
        Arguments:
            arr:        2d list or ndarray of width heights, [[w0,h0],...,[wn,hn]]
            verbose: [false]

            Example
            >>> import pybinpack as pb
            >>> b = pb.Binnit(8640, 15360) # init bin of size 16K - 
            >>> pack = b.pack(size_arr, verbose=True)
            >>> print(b.bins, b.width, b.height)

        )pbdoc",
        py::arg("arr"), py::arg("verbose")=false )
        .def_readonly("bins", &Binnit::m_bins, "number of bins")
        .def_readonly("width", &Binnit::m_bin_width, "width of the bins; flipped! actually height")
        .def_readonly("height", &Binnit::m_bin_height, "height of the bins; flipped! actually width");


    m.attr("__version__") = "0.0.6.cpp";
}