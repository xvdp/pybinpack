/*
pybinpack
xvdp 2018 

changelist
: created parent class 'BinPack' to hold public 'usedRectangles'
: enum name array for reporting
: added index for reference
: merged node with rect


adapted from:
* @file ShelfNextFitBinPack.h
	@author Jukka Jyl�nki

	@brief Implements the naive Shelf Next Fit bin packer algorithm.

	This algorithm is not recommended for real use at all - its only advantage is that it
	consumes only a constant amount of memory, whereas the other packers in this library
	use at least a linear amount of memory.

	This work is released to Public Domain, do whatever you want with it.
*/
#pragma once

#include <vector>
#include "Rect.h"

namespace rbp {

	class ShelfNextFitBinPack  : public BinPack
	{
		public:
			// struct Node
			// {
			// 	int x;
			// 	int y;
			// 	int width;
			// 	int height;
			// 	int index;

			// 	bool flipped;
			// };

			void Init(int width, int height);

			Rect Insert(int width, int height, const int &index=0);

			/// Computes the ratio of used surface area.
			float Occupancy() const;

		private:
			int binWidth;
			int binHeight;

			int currentX;
			int currentY;
			int shelfHeight;

			unsigned long usedSurfaceArea;
	};

}
