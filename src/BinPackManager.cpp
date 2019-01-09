/*
pybinpack
xvdp 2018 
*/
#include <cassert> 
#include <string>
#include <vector>
#include <math.h>
#include "BinPackManager.h"

using namespace std;

Binnit::Binnit (int width, int height) {
    m_bin_width = width;
    m_bin_height = height;
}
/////
// 
// input 2d list or 2d numpy array 
//
void Binnit::get_rectangles(py::array_t<int> arr)
{
    py::buffer_info buf1 = arr.request();
    int numel = buf1.size;
    auto shape = buf1.shape;

    assert(int(shape.size()) == int(2));// expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]
    assert(int(shape[1]) == int(2));    // expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]

    if (m_verbose)
        py::print("::get_rectangles()\nArray has ", shape[0], " elements, of ", shape[1], "dimensions");

    m_rects.clear();
    m_areas.clear();
    m_area = 0;
  
    for (int i = 0; i < shape[0]; i++){
        //py::print("element" ,i, ":", arr.mutable_at(i,0), arr.mutable_at(i,1));
        rbp::RectSize newNode;
        newNode.width = arr.mutable_at(i, 0);
        newNode.height = arr.mutable_at(i, 1);
        newNode.index = i;

        m_rects.push_back(newNode);
        int area = newNode.width * newNode.height;
        m_areas.push_back(area);
        m_area += area;
    }
}
/////
// 
// estimate bin needed if area smaller than 100x100. : (int) (sqrt(sumarea) * overflow)
//
void Binnit::approximate_bin_size(float overflow)
{
    if (m_verbose)
        py::print("::approximate_bin_size()\nInput bin width: ", m_bin_width, "\nInput bin height ", m_bin_height);

    if (m_bin_height * m_bin_width < m_minimum_logical_bin){
        int approx_size = int(sqrt(double(m_area)) * overflow);
        m_bin_height = approx_size;
        m_bin_width = approx_size;
        m_fixedbin = false;
    }

    if (m_fixedbin)
        py::print("\tUse input dimensions: ", m_fixedbin);
    else
        py::print("\tOutput bin height ", m_bin_height, "\n\tOutput bin width ", m_bin_height);
}

bool Binnit::check_enum(const vector<string> &e, const int &i)
{
    if ( e.size() < i < 0)
        return true;

    py::print("heuristic allowable range: 0-", e.size()-1, ":", e);
    return false;
}

/////
// fill_bin fills bins and creates as many new bins as necessary
// implemented only for MaxRectsBinPack: TODO Template everything
//
void Binnit::fill_bin(rbp::MaxRectsBinPack& bin, int heuristic, bool allow_flip)
{
    m_bins = 0;
    assert(check_enum(rbp::MaxRectsBinPack::enum_name, heuristic));

    while (m_rects.size()){

        // vector.push_back copies data
        if (m_bins){
            Bins.push_back(bin);
            if (!m_fixedbin) { // single bin, log rects that dont fit and exit
                for (int i = 0; i < m_rects.size(); i++)
                    m_misfits.push_back({m_rects[i].width, m_rects[i].height});
                return; 
            }
        }
        // > MaxRectsBinPack specific code
        bin.Init(m_bin_width, m_bin_height, allow_flip);
        rbp::MaxRectsBinPack::FreeRectChoiceHeuristic _heuristic = rbp::MaxRectsBinPack::FreeRectChoiceHeuristic(heuristic); // <

        m_bins += 1;
        // set min size that doesnt fit
        bin.max_height = m_bin_height + 1;
        bin.max_width = m_bin_width + 1;

        int pack_index = 0;
        for (int i = 0; i < m_rects.size(); i++){

            // if rectangle cannot fit at all within bin, pop it out
            if (m_rects[i].height > m_bin_height || m_rects[i].width > m_bin_width || (!allow_flip &&(m_rects[i].width > m_bin_height || m_rects[i].height > m_bin_width))){
                m_misfits.push_back({m_rects[i].width, m_rects[i].height});
                py::print("Rectangle index", m_rects[i].index, "of size ", m_rects[i].width, m_rects[i].height, " does not fit in available space of", m_bin_width, m_bin_height );
                m_rects.erase(m_rects.begin() + i);
                i--;
            } 

            // handle skipping rectangles
            if (m_rects[i].height < bin.max_height || m_rects[i].width < bin.max_width || m_rects[i].height < bin.max_width || m_rects[i].width < bin.max_height){

                // > MaxRectsBinPack specific code
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, m_rects[i].index); // <

                if (packedRect.height > 0){
                    m_rects.erase(m_rects.begin() + i);
                    i--;
                }
                else{
                    // defer rectangle to next bin, update min size
                    bin.max_height = std::min(bin.max_height, m_rects[i].height);
                    bin.max_width = std::min(bin.max_width, m_rects[i].width);
                }
            }
        }
        // append last bin
        if (m_rects.size() == 0){
            Bins.push_back(bin);
        }
    }
}
/////
//
// simpler method, maxrectsbinpack, no flipping allowed, returns, array size 6, num rects
//
py::array_t<int> Binnit::pack(py::array_t<int> arr, bool verbose)
{   

    m_verbose = verbose;
    Bins.clear();
    get_rectangles(arr);
    assert (m_bin_height * m_bin_width >= m_minimum_logical_bin);

    rbp::MaxRectsBinPack bin;
    fill_bin(bin, 0, false); // MaxRectsBinPack, no rotation

    return pyout(false); //include_rotation false: resulting array: (6, num_rect)
}
/////
// 
// main
//
py::array_t<int> Binnit::Pack(py::array_t<int> arr, int method, float overflow, int heuristic, int split_method, bool allow_flip, bool verbose){
    /* wrapper to all BinPack Methods*/

    //TODO expose
    bool _useWasteMap = true;   // ShelfBinPack, Skyline
    bool _merge = true;         // GuillotineBinPack

    m_verbose = verbose;

    Bins.clear();
    get_rectangles(arr);
    approximate_bin_size(overflow);

    m_misfits.clear();

    if (method != 0 && m_fixedbin){
        py::print("*******************");
        py::print("WARNING: Only MaxRectsBinPack (method 0) has multiple fixed size bins implemented");
        py::print("... Results will be incomplete, only one bin of fixed size will be packed.");
        py::print("*******************");
    }

    switch (method){
        case(0):{ //MaxRectsBinPack
        
            rbp::MaxRectsBinPack bin;
            if (m_verbose)
                py::print("chosen method: MaxRectsBinPack, heuristic:", bin.enum_name[heuristic]);

            fill_bin(bin, heuristic, allow_flip);
            break;
        }
        case(1):{ //GuillotineBinPack

            rbp::GuillotineBinPack bin;
            assert(check_enum(rbp::GuillotineBinPack::enum_name, heuristic));
            assert(check_enum(rbp::GuillotineBinPack::enum_split_name, split_method));

            if (m_verbose)
                py::print("chosen method: GuillotineBinPack, heuristic:", bin.enum_name[heuristic], ", split method:", bin.enum_split_name[split_method]);
            
            m_bins = 1; // only single bin algo implemented
            bin.Init(m_bin_width, m_bin_height);
  
            rbp::GuillotineBinPack::FreeRectChoiceHeuristic _heuristic = rbp::GuillotineBinPack::FreeRectChoiceHeuristic(heuristic);
            rbp::GuillotineBinPack::GuillotineSplitHeuristic _split = rbp::GuillotineBinPack::GuillotineSplitHeuristic(split_method);

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _merge, _heuristic, _split, i);
                if (packedRect.height <= 0)
                    m_misfits.push_back({m_rects[i].width, m_rects[i].height});
            }
            Bins.push_back(bin);
            break;
        }
        case(2):{ //ShelfBinPack

            rbp::ShelfBinPack bin;
            assert(check_enum(rbp::ShelfBinPack::enum_name, heuristic));
        
            if (m_verbose)
                py::print("chosen method: ShelfBinPack, heuristic:", bin.enum_name[heuristic]);

            m_bins = 1; // only single bin algo implemented
            bin.Init(m_bin_width, m_bin_height, _useWasteMap);

            rbp::ShelfBinPack::ShelfChoiceHeuristic _heuristic = rbp::ShelfBinPack::ShelfChoiceHeuristic(heuristic);

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, i);
                if (packedRect.height <= 0)
                    m_misfits.push_back({m_rects[i].width, m_rects[i].height});
                else
                    bin.usedRectangles.push_back(packedRect);
            }

            Bins.push_back(bin);
            break;
        }
        case(3):{//ShelfNextFitBinPack

            rbp::ShelfNextFitBinPack bin;
            if (m_verbose)
                py::print("chosen method: ShelfNextFitBinPack");

            m_bins = 1; // only single bin algo implemented
            bin.Init(m_bin_width, m_bin_height);

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, i);
                if (packedRect.height <= 0)
                    m_misfits.push_back({m_rects[i].width, m_rects[i].height});
                else
                    bin.usedRectangles.push_back(packedRect);
            }
            Bins.push_back(bin);
            break;
        }
        case(4):{//SkylineBinPack

            rbp::SkylineBinPack bin;
            assert(check_enum(rbp::SkylineBinPack::enum_name, heuristic));
 
            if (m_verbose)
                py::print("chosen method: SkylineBinPack, heuristic:", bin.enum_name[heuristic]);

            m_bins = 1; // only single bin algo implemented
            bin.Init(m_bin_width, m_bin_height, _useWasteMap);

            rbp::SkylineBinPack::LevelChoiceHeuristic _heuristic = rbp::SkylineBinPack::LevelChoiceHeuristic(heuristic);

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, i);
                if (packedRect.height <= 0)
                    m_misfits.push_back({m_rects[i].width, m_rects[i].height});
                else
                    packedRect.index = i;
                    bin.usedRectangles.push_back(packedRect);
            }
            Bins.push_back(bin);
            break;
        }
        default:{
            py::print("methods:, available range: 0-4");
        }
    }
    if (m_misfits.size() > 0){
        py::print("Number of rectangles that dont fit:", m_misfits.size());
    }
    return pyout();

}
/////
// 
// return Binned data as flat numpy array
//  [0]   : bin index
//  [1]   : input rectangle index
//  [2]   : is rotated 0/1 # Errata: does not return correct value
//  [3,4] : position
//  [5,6] : area
//
py::array_t<int> Binnit::pyout(bool include_rotation){

    int _num_rects = 0;
    for (int i = 0; i < Bins.size(); i++)
        _num_rects += Bins[i].usedRectangles.size();

    if (m_verbose){
        py::print("Number of Bins:\t", Bins.size());
        py::print("Number of Rectangles:\t", _num_rects);
    }

    py::array_t<int> binned(_num_rects * (6 + int(include_rotation)));
    int k = 0;
    for (int i = 0; i < Bins.size(); i++)
    {
        for (int j = 0; j < Bins[i].usedRectangles.size(); j++){
            binned.mutable_at(k++) = i;
            binned.mutable_at(k++) = Bins[i].usedRectangles[j].index;
            if (include_rotation)
                binned.mutable_at(k++) = int(Bins[i].usedRectangles[j].flipped);
            binned.mutable_at(k++) =  Bins[i].usedRectangles[j].x;
            binned.mutable_at(k++) =  Bins[i].usedRectangles[j].y;
            binned.mutable_at(k++) =  Bins[i].usedRectangles[j].width;
            binned.mutable_at(k++) =  Bins[i].usedRectangles[j].height;
        }
    }
    return binned;
}



////
// TODO either deprecate or make use of this
//
//#include <boost/algorithm/string.hpp> 

// void Binnit::SetMethod(int method){
//     assert(5 > method > -1);
//     Binnit::SetMethod(Binnit::Method(method));
// }

// void Binnit::SetMethod(enum Binnit::Method method){//}, int heuristic){
//     py::print(method, int(method));
// }
// void Binnit::SetMethod(const string& method){//, int heuristic){

//     vector<string> structures = {"MaxRects", "Guilliotine", "Shelf", "ShelfNextFit", "Skyline"};
//     vector<string> stru = {"max", "gui", "she", "nex", "sky"};
//     const string method_l = boost::algorithm::to_lower_copy(method);
//     int chosen = -1;

//     for (int i = 0; i <stru.size(); i++){
//         if (method_l.find(stru[i]) != string::npos){
//             chosen = i;
//             break;
//         }
//     }
//     if (chosen >= 0){
//         py::print(chosen);
//         Binnit::SetMethod(chosen);
//     }
//     else{
//         py::print("arg 'method' has to be either one of ", structures, "(",0,"-", structures.size()-1, ")");
//     }
// }

/*template <class T, typename... args>
T Init(T height, )*/

// template <class T>
// T Init(int width, int height, bool b_arg){
//     T bin;
//     bin.Init(width, height, b_arg);
//     return bin;
// }



