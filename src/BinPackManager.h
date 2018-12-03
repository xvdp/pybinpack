/*
xvdp 2018
*/
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "Rect.h"
#include "MaxRectsBinPack.h"

using namespace std;
namespace py = pybind11;

std::vector<rbp::RectSize> get_rects(py::array_t<int> arr);
std::vector<std::vector<int>> get_vec(py::array_t<int> arr);
void simple_pack(py::array_t<int> arr, int binWidth=16384, int binHeight=8192);


class Binnit{

    public:

        Binnit(int width, int height);
        // read input list pack in vector
        // compute area per rectangle and total area

        std::vector<rbp::MaxRectsBinPack> Bins;

        void pre_process(py::array_t<int> arr);
        // simple heuristic:
        // if bin_width * bin_height <= minimum_logical_bin: single bin estimated size
        // else estimate number of bins. 
        void approximate_bin();

        py::array_t<int> pack(py::array_t<int> arr);


    private:


        std::vector<rbp::RectSize> m_rects;
        std::vector<int> m_areas;
        int m_area;

        int m_minimum_logical_bin = 10e4; // if smaller than 100x100 binnning doesnt make much sense
        int m_bins = 1;
        int m_bin_width = 0;
        int m_bin_height = 0;

        bool m_unpacked = true;
        // if fixed bin is true init a new bin for overflows
        // else expand bin on overflow, contract bin posprocess
        bool m_fixedbin = true;

        int m_numrect = 0;

        py::array_t<int> pyout();


};