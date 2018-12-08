/*
pybinpack
xvdp 2018 
*/
#include <cassert> 
#include <string>
#include <math.h>
#include <boost/algorithm/string.hpp>    
#include "BinPackManager.h"

using namespace std;

Binnit::Binnit (int width, int height) {
    m_bin_width = width;
    m_bin_height = height;
}

void Binnit::get_rectangles(py::array_t<int> arr)
{
    /*  input 2d list or 2d array:
            build rectsize vector
            get total area
            approximate binsize needed

        collect 2d 2chanel ndarray or list and build vector
    */
    py::buffer_info buf1 = arr.request();
    int numel = buf1.size;
    auto shape = buf1.shape;

    assert(int(shape.size()) == int(2));// expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]
    assert(int(shape[1]) == int(2));    // expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]

    py::print("array has " ,shape[0], " elements, of ",shape[1], "dimensions");

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

void Binnit::approximate_bin_size(float overflow){
    /*if bin size is automatic assume single bin*/

    if (m_bin_height * m_bin_height < m_minimum_logical_bin){

        int approx_size = int(sqrt(double(m_area)) * overflow);
        m_bin_height = approx_size;
        m_bin_width = approx_size;

        bool m_fixedbin = false;
    }
}

py::array_t<int> Binnit::Pack(py::array_t<int> arr, int method, float overflow, int heuristic, int split_method, bool verbose){
    /* wrapper to all BinPack Methods*/

    //TODO expose
    bool _useWasteMap = true;   // ShelfBinPack, Skyline
    bool _merge = true;         // GuillotineBinPack
    bool _allow_flip = true;    // MaxRectsBinPack

    Bins.clear();
    get_rectangles(arr);
    approximate_bin_size(overflow);

   //m_numrect = 0;
    std::string heuristic_name;

    std::vector<std::vector<int>> misfits;
    misfits.clear();

    switch (method){
        case(0):{ //MaxRectsBinPack

            rbp::MaxRectsBinPack bin;
            if (heuristic >= bin.enum_name.size()){
                py::print("heuristic allowable range: 0-", bin.enum_name.size()-1, ":", bin.enum_name);
                py::array_t<int> binned(0);
                return binned;
            }
            if (verbose)
                py::print("chosen method: MaxRectsBinPack, heuristic:", bin.enum_name[heuristic]);


            bin.Init(m_bin_width, m_bin_height, _allow_flip);
            rbp::MaxRectsBinPack::FreeRectChoiceHeuristic _heuristic = rbp::MaxRectsBinPack::FreeRectChoiceHeuristic(heuristic);
            heuristic_name = bin.enum_name[int(heuristic)];
            
            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, i);
                if (packedRect.height <= 0)
                    misfits.push_back({m_rects[i].width, m_rects[i].height});
            }
            
            Bins.push_back(bin);
            break;
        }
        case(1):{ //GuillotineBinPack

            rbp::GuillotineBinPack bin;
            if (heuristic >= bin.enum_name.size()){
                py::print("heuristic allowable range: 0-", bin.enum_name.size()-1, ":", bin.enum_name);
                py::array_t<int> binned(0);
                return binned;
            }
            if (split_method >= bin.enum_split_name.size()){
                py::print("split method allowable range: 0-", bin.enum_split_name.size()-1, ":", bin.enum_split_name);
                py::array_t<int> binned(0);
                return binned;
            }

            if (verbose){
                py::print("chosen method: GuillotineBinPack, heuristic:", bin.enum_name[heuristic], ", split method:", bin.enum_split_name[split_method]);
            }
            bin.Init(m_bin_width, m_bin_height);
  
            rbp::GuillotineBinPack::FreeRectChoiceHeuristic _heuristic = rbp::GuillotineBinPack::FreeRectChoiceHeuristic(heuristic);
            rbp::GuillotineBinPack::GuillotineSplitHeuristic _split = rbp::GuillotineBinPack::GuillotineSplitHeuristic(split_method);
            heuristic_name = bin.enum_name[heuristic] + " "+bin.enum_split_name[split_method];

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _merge, _heuristic, _split, i);
                if (packedRect.height <= 0)
                    misfits.push_back({m_rects[i].width, m_rects[i].height});
            }
            Bins.push_back(bin);
            break;
        }
        case(2):{ //ShelfBinPack

            rbp::ShelfBinPack bin;
            if (heuristic >= bin.enum_name.size()){
                py::print("heuristic allowable range: 0-", bin.enum_name.size()-1, ":", bin.enum_name);
                py::array_t<int> binned(0);
                return binned;
            }
            if (verbose)
                py::print("chosen method: ShelfBinPack, heuristic:", bin.enum_name[heuristic]);

            bin.Init(m_bin_width, m_bin_height, _useWasteMap);

            rbp::ShelfBinPack::ShelfChoiceHeuristic _heuristic = rbp::ShelfBinPack::ShelfChoiceHeuristic(heuristic);
            heuristic_name = bin.enum_name[heuristic];

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, i);
                if (packedRect.height <= 0)
                    misfits.push_back({m_rects[i].width, m_rects[i].height});
                else
                    bin.usedRectangles.push_back(packedRect);
            }

            Bins.push_back(bin);
            break;
        }
        case(3):{//ShelfNextFitBinPack

            rbp::ShelfNextFitBinPack bin;
            if (verbose)
                py::print("chosen method: ShelfNextFitBinPack");

            bin.Init(m_bin_width, m_bin_height);

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, i);
                if (packedRect.height <= 0)
                    misfits.push_back({m_rects[i].width, m_rects[i].height});
                else
                    bin.usedRectangles.push_back(packedRect);
            }
            Bins.push_back(bin);
            break;
        }
        case(4):{//SkylineBinPack

            rbp::SkylineBinPack bin;
            if (heuristic >= bin.enum_name.size()){
                py::print("heuristic allowable range: 0-", bin.enum_name.size()-1, ":", bin.enum_name);
                py::array_t<int> binned(0);
                return binned;
            }
            if (verbose)
                py::print("chosen method: SkylineBinPack, heuristic:", bin.enum_name[heuristic]);

            bin.Init(m_bin_width, m_bin_height, _useWasteMap);

            rbp::SkylineBinPack::LevelChoiceHeuristic _heuristic = rbp::SkylineBinPack::LevelChoiceHeuristic(heuristic);
            heuristic_name = bin.enum_name[heuristic];

            for (int i = 0; i < m_rects.size(); i++){
                rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, _heuristic, i);
                if (packedRect.height <= 0)
                    misfits.push_back({m_rects[i].width, m_rects[i].height});
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
    if (misfits.size() > 0){
        py::print("Number of rectangles that dont fit:", misfits.size());
    }
    return pyout(verbose);

}

py::array_t<int> Binnit::pyout(bool verbose){
    /*
        converts bins, indices, used rectangles to numpy array
        TODO: return as multidim array or dictionary
    */
    int _num_rects = 0;
    for (int i = 0; i < Bins.size(); i++)
        _num_rects += Bins[i].usedRectangles.size();

    py::array_t<int> binned(_num_rects*6);

    for (int i = 0; i < Bins.size(); i++)
    {
        for (int j = 0; j < _num_rects; j++){ 

            binned.mutable_at(j*6) = i;  //bin index
            binned.mutable_at(j*6+1) = Bins[i].usedRectangles[j].index; //rectangle index
            binned.mutable_at(j*6+2) =  Bins[i].usedRectangles[j].x;
            binned.mutable_at(j*6+3) =  Bins[i].usedRectangles[j].y;
            binned.mutable_at(j*6+4) =  Bins[i].usedRectangles[j].width;
            binned.mutable_at(j*6+5) =  Bins[i].usedRectangles[j].height;
        }
    }
    return binned;
}


/////
// TODO Deprectate Binnit::Pack handles this and more
///
// py::array_t<int> Binnit::pack(py::array_t<int> arr, float overflow)
// {
//     //std::vector<rbp::MaxRectsBinPack> Bins;
//     // TODO array of bins
//     // TODO different bin packing types

//     Bins.clear();

//     get_rectangles(arr);
//     approximate_bin_size(overflow);

//     rbp::MaxRectsBinPack bin;

// 	py::print("Initializing bin to ", m_bin_width, m_bin_height);
// 	bin.Init(m_bin_width, m_bin_height);

//     m_numrect = 0;

//     for (int i = 0; i < m_rects.size(); i++){
// 		rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::FreeRectChoiceHeuristic(0);
//         rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, heuristic, i);

//         // std::string heuristic_name = bin.enum_name[int(heuristic)];
//         // py::print(int(heuristic), heuristic_name);

//         /* Test success or failure. TODO: 
//             if failure 
//                 if fixed size bin: 
//                     initialize new bin, record dims of rect that didnt fit,
//                 else
//                     enarge bin
//         */
// 		if (packedRect.height <= 0)
//             py::print(" No room for rectangle [", i, "] of size [", m_rects[i].width, ",", m_rects[i].height, "]"); 
//             //bin.Resize(width, height +x)
//         else
//             m_numrect += 1;
// 	}

//     Bins.push_back(bin);
//     return pyout();

// }

////
// TODO either deprecate or make use of this
//

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


