/*
xvdp 2018
*/
#include <math.h>
#include "BinPackManager.h"


Binnit::Binnit (int width=0, int height=0) {
    m_bin_width = width;
    m_bin_height = height;
}


void Binnit::pre_process(py::array_t<int> arr)
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

    approximate_bin();
}

void Binnit::approximate_bin(){
    /*if bin size is automatic assume single bin*/

    if (m_bin_height * m_bin_height < m_minimum_logical_bin){

        int approx_size = int(sqrt(double(m_area)) * 1.2);
        m_bin_height = approx_size;
        m_bin_width = approx_size;

        bool m_fixedbin = false;
    }
}

py::array_t<int> Binnit::pack(py::array_t<int> arr)
{
    //std::vector<rbp::MaxRectsBinPack> Bins;
    // TODO array of bins
    // TODO different bin packing types

    //first go

    pre_process(arr);


    rbp::MaxRectsBinPack bin;
	py::print("Initializing bin to ", m_bin_width, m_bin_height);
	bin.Init(m_bin_width, m_bin_height);

    m_numrect = 0;

    for (int i = 0; i < m_rects.size(); i++){
        
		rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBestShortSideFit; // This can be changed individually even for each rectangle packed.
		rbp::Rect packedRect = bin.Insert(m_rects[i].width, m_rects[i].height, heuristic, i);

        /* Test success or failure. TODO: 
            if failure 
                if fixed size bin: 
                    initialize new bin, record dims of rect that didnt fit,
                else
                    enarge bin
        */
		if (packedRect.height <= 0)
            py::print(" No room for a this size rectangle\n"); //init new bin 
            //bin.Resize(width, height +x)
        else
            m_numrect += 1;

	}
	// py::print("Done. All rectangles packed.\n");

    //for(size_t i = 0; i < usedRectangles.size(); ++i)
    // for(auto const& d: bin.usedRectangles) 
    //     py::print(" x", d.x, "y",  d.y, "w", d.width, "h", d.height, "index", d.index);

    Bins.push_back(bin);
    return pyout();

}

py::array_t<int> Binnit::pyout(){

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
    py::print(_num_rects, binned.size());
    return binned;
}



std::vector<rbp::RectSize> get_rects(py::array_t<int> arr)
{
    /*  input 2d list or 2d array, output vector of rectsexit
        collect 2d 2chanel ndarray or list and build vector
    */
    py::buffer_info buf1 = arr.request();
    int numel = buf1.size;
    auto shape = buf1.shape;

    assert(int(shape.size()) == int(2));// expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]
    assert(int(shape[1]) == int(2));    // expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]

    py::print("array has " ,shape[0], " elements, of ",shape[1], "dimensions");

    vector<rbp::RectSize> vec;
  
    for (int i = 0; i < shape[0]; i++){
        //py::print("element" ,i, ":", arr.mutable_at(i,0), arr.mutable_at(i,1));
        rbp::RectSize newNode;
        newNode.width = arr.mutable_at(i,0);
        newNode.height = arr.mutable_at(i,1);
        newNode.index = i;

        vec.push_back(newNode);
    }
    return vec;
}




std::vector<std::vector<int>> get_vec(py::array_t<int> arr)
{
    /*  input 2d list or 2d array, output vector of rects
        collect 2d 2chanel ndarray or list and build vector
    */
    py::buffer_info buf1 = arr.request();
    int numel = buf1.size;
    auto shape = buf1.shape;

    assert(int(shape.size()) == int(2));// expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]
    assert(int(shape[1]) == int(2));    // expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]

    py::print("array has " ,shape[0], " elements, of ",shape[1], "dimensions");

   std::vector<std::vector<int>> vec;
  
    for (int i = 0; i < shape[0]; i++){
        std::vector<int> rect = {arr.mutable_at(i,0), arr.mutable_at(i,1)};
        vec.push_back(rect);
    }
    return vec;
}



void simple_pack(py::array_t<int> arr, int binWidth, int binHeight){ 
    /*
    simple pack, runs a packer of fixed size which fails if too many rects to pack
    */

    rbp::MaxRectsBinPack bin;
    // alternates:
    // a. estimate height and width by the closest squareroot to ~110% of the area
    // b. compute number 

	py::print("Initializing bin to ", binWidth, binHeight);
	bin.Init(binWidth, binHeight);


    py::buffer_info buf1 = arr.request();
    int numel = buf1.size;
    auto shape = buf1.shape;

    assert(int(shape.size()) == int(2));// expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]
    assert(int(shape[1]) == int(2));    // expects a 2d array with of 2 channels, e.g [[1,2],[3,4],[5,6]]

    py::print("array has ", shape[0], " elements, of ", shape[1], "dimensions");


    for (int i = 0; i < shape[0]; i++){

		rbp::MaxRectsBinPack::FreeRectChoiceHeuristic heuristic = rbp::MaxRectsBinPack::RectBestShortSideFit; 
		rbp::Rect packedRect = bin.Insert(arr.mutable_at(i, 0), arr.mutable_at(i, 1), heuristic, i);


		// Test success or failure.
		if (packedRect.height > 0)
			py::print("Packed to (x,y)=(", packedRect.x, packedRect.y, "), (w,h)=(", packedRect.width,  packedRect.height, ") Free space left", 100.f - bin.Occupancy()*100.f, packedRect.index, i);
		else
			py::print(" No room for a this size rectangle\n");
	}
	py::print("Done. All rectangles packed.\n");

    //for(size_t i = 0; i < usedRectangles.size(); ++i)
    for(auto const& d: bin.usedRectangles) 
        py::print(" x", d.x, "y",  d.y, "w", d.width, "h", d.height, "index", d.index);
}
