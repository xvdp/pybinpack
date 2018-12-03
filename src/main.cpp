/*
rectangle bin packer
xvdp 2018 
pybind11 wrapper to RectangleBinPack
*/
#include <stdio.h>
#include <pybind11/pybind11.h>

#include "BinPackManager.h"

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

    m.def("get_rects", &get_rects, "numpy 2d array to array of Rect.h");
    m.def("get_vec", &get_vec, "numpy 2d array to vector 2d array");
    m.def("simple_pack", &simple_pack, "executes bin packing", py::arg("arr"), py::arg("binWidth")=16384, py::arg("binHeight")=8192);

    // class example
    py::class_<Binnit>(m, "Binnit")
        .def(py::init<const int &, const int &>())
        .def("pre_process", &Binnit::pre_process)
        .def("pack", &Binnit::pack);
  

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}