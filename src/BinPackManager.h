/*
pybinpack
xvdp 2018 

changelist
: General accessor class Binnit

TODO: 
    multiple bins
    fix method, 2,3,4 = return empty arrays
*/
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "Rect.h"
#include "MaxRectsBinPack.h"
#include "GuillotineBinPack.h"
#include "ShelfBinPack.h"
#include "ShelfNextFitBinPack.h"
#include "SkylineBinPack.h"

using namespace std;
namespace py = pybind11;


class Binnit{

    public:

        Binnit(int width=0, int height=0);
        // read input list pack in vector
        // compute area per rectangle and total area

        enum Method{
            MaxRects = 0,
            Guillotine = 1,
            Shelf = 2,
            ShelfNextFit = 3,
            Skyline = 4
            };


        void SetMethod(enum Method method);//, int heuristic=0);        
        void SetMethod(int method);//, int heuristic=0);        
        void SetMethod(const string& method);//, int heuristic=0);        


        std::vector<rbp::BinPack> Bins;

        // gets list or numpy array and writes is as m_rects
        void get_rectangles(py::array_t<int> arr);

        // simple heuristic:
        // if bin_width * bin_height <= minimum_logical_bin: single bin estimated size
        // else estimate number of bins. 
        void approximate_bin_size(float overflow=1.1);

        py::array_t<int> pack(py::array_t<int> arr, float overflow=1.1);

        py::array_t<int> Pack(py::array_t<int> arr, int method=0, float overflow=1.1, int heuristic=0, int split_method=0, bool verbose=false);

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

        py::array_t<int> pyout(bool verbose=false);

};