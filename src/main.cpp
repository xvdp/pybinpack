/*
rectangle bin packer
xvdp 2018 

pybind11 bindings to RectangleBinPack
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

    py::class_<Binnit>binnit(m, "Binnit");

    binnit.def(py::init<const int &, const int &>(),  py::arg("width")=0,  py::arg("height")=0 )

        .def("Pack", &Binnit::Pack, "Bin Packing", py::arg("arr"), py::arg("method")=0, py::arg("overflow")=1.1,
             py::arg("heuristic")=0, py::arg("split_method")=0, py::arg("allow_flip")=true, py::arg("verbose")=false );
    /*
        .def("SetMethod", py::overload_cast<enum Binnit::Method> (&Binnit::SetMethod), "Specifies between the various methods for binpacking available" )
        .def("SetMethod", py::overload_cast<int> (&Binnit::SetMethod), "Specifies between the various methods for binpacking available" )
        .def("SetMethod", py::overload_cast<const string&> (&Binnit::SetMethod), "Specifies between the various methods for binpacking available" );

    py::enum_<Binnit::Method>(binnit, "Method")
        .value("MaxRects", Binnit::Method::MaxRects)
        .value("Guillotine", Binnit::Method::Guillotine)
        .value("Shelf", Binnit::Method::Shelf)
        .value("ShelfNextFit", Binnit::Method::ShelfNextFit)
        .value("Skyline", Binnit::Method::Skyline);
        */

    m.attr("__version__") = "0.0.4.1";
}