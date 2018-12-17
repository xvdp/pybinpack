/*
pybinpack
xvdp 2018 

changelist
: General accessor class Binnit
: Multiple fixed size bins for MaxRectsBinpack

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

        std::vector<rbp::BinPack> Bins;

        int m_bins = 0;         // number of bins
        int m_bin_width = 0;    // width of bins
        int m_bin_height = 0;   // height of bins

        py::array_t<int> Pack(py::array_t<int> arr, int method=0, float overflow=1.1, int heuristic=0, 
                              int split_method=0, bool allow_flip=true, bool verbose=false);

    private:

        std::vector<rbp::RectSize> m_rects; //rectangles to pack
        std::vector<int> m_areas;
        bool m_verbose;

        int m_area;             //total rectangles area

        std::vector<std::vector<int>> m_misfits; //rectangles that do not fit in given bins

        int m_minimum_logical_bin = 10e4; // if smaller than 100x100 binnning doesnt make much sense
        bool m_fixedbin = true; // if input size smaller than minimum_logical, estimate with approximate_bin_size

        // input from python: gets list or numpy array and writes is as m_rects
        void get_rectangles(py::array_t<int> arr);
        // output to python
        py::array_t<int> pyout();
    
        // if bin_width * bin_height <= minimum_logical_bin: single bin estimated size
        void approximate_bin_size(float overflow=1.1); //estimate size of bin

        // main fill bin for MaxRects method
        void fill_bin(rbp::MaxRectsBinPack& bin, int heuristic, bool allow_flip);

        bool check_enum(const vector<string> &e, const int &i);

        // Implement or deprecate
        // enum Method{
        //     MaxRects = 0,
        //     Guillotine = 1,
        //     Shelf = 2,
        //     ShelfNextFit = 3,
        //     Skyline = 4
        //     };

        // void SetMethod(enum Method method);//, int heuristic=0);        
        // void SetMethod(int method);//, int heuristic=0);        
        // void SetMethod(const string& method);//, int heuristic=0);        

};