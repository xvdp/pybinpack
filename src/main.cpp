/*
rectangle bin packer
xvdp 2018 

pybind11 bindings BinPacking algorithms
*/

#include <pybind11/pybind11.h>
#include "BinPackManager.h"

using namespace std;
namespace py = pybind11;


PYBIND11_MODULE(_pybinpack, m){
    m.doc() = R"pbdoc(
        pybinpack
        ---------
        .. currentmodule:: _pybinpack
        .. autosummary:: 
           :toctree: _generate

           mul
    )pbdoc";

    py::class_<Binnit>binnit(m, "Binnit");

    binnit.def(py::init<const int &, const int &>(),  py::arg("width")=0,  py::arg("height")=0 )

        .def("Pack", &Binnit::Pack, R"pbdoc(Bin Packing main function; 
    Returns a flat int array of size: 7*num rectangles, with info
        0: bin index
        1: image index
        2: rotation 
        3: position x
        4: position y
        5: width
        6: height
    **Arguments:**
        *arr*:        2d list or ndarray of width heights, [[w0,h0],...,[wn,hn]]
        *method*:     [0]: default, MaxRectsBinPack; 
            (1: Guillotine, 2: Shelf, 3: ShelfNextFit, 4: Skyline)
        *overflow*:   [1.1]: default; if no binsize
                (1) bin of area = overflow * sum rect areas
        *heuristic*:  [0]: per method, to determine position of next rect
            MaxRectsBinPack
            {"RectBestShortSideFit", "RectBestLongSideFit", "RectBestAreaFit",
             "RectBottomLeftRule", "RectContactPointRule"}
            GuillotineBinPack
             {"RectBestAreaFit", "RectBestShortSideFit", "RectBestLongSideFit",
              "RectWorstAreaFit", "RectWorstShortSideFit", "RectWorstLongSideFit"}
            ShelfBinPack
            {"ShelfNextFit", "ShelfFirstFit", "ShelfBestAreaFit", "ShelfWorstAreaFit",
             "ShelfBestHeightFit", "ShelfBestWidthFit", "ShelfWorstWidthFit"}
            ShelfNextBinPack: N/A
            SkylineBinPack
            {"LevelBottomLeft", "LevelMinWasteFit"}
        *allow_flip*: [true]: Only implented for MaxRectsBinPack.
        *split_method*: [0] GuillotineBinPack options for splitting the data
            {"SplitShorterLeftoverAxis", "SplitLongerLeftoverAxis", "SplitMinimizeArea",
            "SplitMaximizeArea", "SplitShorterAxis", "SplitLongerAxis"}
        *verbose*: [false]

    **Examples**
        >>> import pybinpack as pb
        >>> b = pb.Binnit(8640, 15360) # init bin of size 16K - 
        >>> pack = b.Pack(size_arr, heuristic=0, allow_flip=False, verbose=True)
        >>> print(b.bins, b.width, b.height)

        >>> b = pb.Binnit() # init one bin of area 105% of sum area of rectangles
        >>> pack = b.Pack(size_arr, overflow=1.05, heuristic=0, allow_flip=True,
                            verbose=True)
        )pbdoc",
        py::arg("arr"), py::arg("method")=0, py::arg("overflow")=1.1,
             py::arg("heuristic")=0, py::arg("split_method")=0, py::arg("allow_flip")=true, py::arg("verbose")=false )

        .def("DataSet", &Binnit::DataSet, R"pbdoc(Bin Packing simplified; 
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
            >>> pack = b.Dataset(size_arr, verbose=True)
            >>> print(b.bins, b.width, b.height)

        )pbdoc",
        py::arg("arr"), py::arg("verbose")=false )
        .def_readonly("bins", &Binnit::m_bins, "number of bins")
        .def_readonly("width", &Binnit::m_bin_width, "width of the bins; flipped! actually height")
        .def_readonly("height", &Binnit::m_bin_height, "height of the bins; flipped! actually width");

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


        , R"pbdoc(
        multiplies int, float, double; or arrays thereof
        Example:
            >>> import pybind_ex
            >>> import numpy as np
            >>> a = np.array([12,3])
            >>> b = np.array([1.,3.])
            >>> c = np.array([2.,2.])
            >>> pybind_ex.v_mul(a,b,c)
            array([24., 18.])
        )pbdoc");
        */

    m.attr("__version__") = "0.0.5";
}