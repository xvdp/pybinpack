/*
xvdp 2018
add index to Rects; to utilize the binning afterwards
add flipped: for MaxRectBinPack and ShelfNextBinPack
add BinPack parent class to hold results

* @author Jukka Jyl�nki
*/

#pragma once

#include <vector>
#include <cassert>
#include <cstdlib>

#ifdef _DEBUG
/// debug_assert is an assert that also requires debug mode to be defined.
#define debug_assert(x) assert(x)
#else
#define debug_assert(x)
#endif


namespace rbp {

    struct RectSize
    {
        int width;
        int height;
        int index;
    };

    struct Rect
    {
        int x;
        int y;
        int width;
        int height;
        int index;

		bool flipped;
		Rect(): x(0), y(0), width(0), height(1), index(1), flipped(false){ }
    };

	/// Parent Class to all algorithms / patch to handle common data
	/// Algorithm differentiation ought to be done with templates, but that would require rewrite of everyghn
	class BinPack{
		public:
			// for export
			std::vector<Rect> usedRectangles;
			// when filling multiple bins encode size of smallest rectangle that could not be handled
			int max_width;
			int max_height;

	};


/// Performs a lexicographic compare on (rect short side, rect long side).
/// @return -1 if the smaller side of a is shorter than the smaller side of b, 1 if the other way around.
///   If they are equal, the larger side length is used as a tie-breaker.
///   If the rectangles are of same size, returns 0.
int CompareRectShortSide(const Rect &a, const Rect &b);

/// Performs a lexicographic compare on (x, y, width, height).
int NodeSortCmp(const Rect &a, const Rect &b);

/// Returns true if a is contained in b.
bool IsContainedIn(const Rect &a, const Rect &b);

class DisjointRectCollection
{
public:
	std::vector<Rect> rects;

	bool Add(const Rect &r)
	{
		// Degenerate rectangles are ignored.
		if (r.width == 0 || r.height == 0)
			return true;

		if (!Disjoint(r))
			return false;
		rects.push_back(r);
		return true;
	}

	void Clear()
	{
		rects.clear();
	}

	bool Disjoint(const Rect &r) const
	{
		// Degenerate rectangles are ignored.
		if (r.width == 0 || r.height == 0)
			return true;

		for(size_t i = 0; i < rects.size(); ++i)
			if (!Disjoint(rects[i], r))
				return false;
		return true;
	}

	static bool Disjoint(const Rect &a, const Rect &b)
	{
		if (a.x + a.width <= b.x ||
			b.x + b.width <= a.x ||
			a.y + a.height <= b.y ||
			b.y + b.height <= a.y)
			return true;
		return false;
	}
};

}
