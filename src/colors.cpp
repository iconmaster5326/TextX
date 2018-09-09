/*
 * colors.cpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#include "colors.hpp"
#include "curses.hpp"

#include <map>
#include <set>

namespace textx {
	using namespace std;
	
	class RC {
	public:
		unsigned usages;
		unsigned value;
		RC() {
			usages = -1;
			value = -1;
		}
		RC(unsigned value) {
			usages = 1;
			this->value = value;
		}
	};
	
	static map<unsigned, RC> pairsInUse;
	static map<unsigned, RC> colorsInUse;
	
	static set<short> pairIndicesInUse;
	static set<short> colorIndicesInUse;
	
	static unsigned colorToInt(unsigned char r, unsigned char g, unsigned char b) {
		return r | (g << 8) | (b << 16);
	}
	
//	static void intToColor(unsigned i, unsigned char& r, unsigned char& g, unsigned char& b) {
//		r = i & 255;
//		g = (i >> 8) & 255;
//		b = (i >> 16) & 255;
//	}
	
	static unsigned pairToInt(short fg, short bg) {
		return fg | (bg << 16);
	}
	
//	static void intToPair(unsigned i, short& fg, short& bg) {
//		fg = i & 65535;
//		fg = (i >> 16) & 65535;
//	}
	
	Color::Color(short index) {
		this->index = index;
	}
	
	void Color::dispose() {
		// TODO
	}
	
	ColorPair::ColorPair(short index) {
		this->index = index;
	}
	
	void ColorPair::dispose() {
		// TODO
	}
	
	ColorPair getColorPair(Color fg, Color bg) {
		unsigned code = pairToInt(fg.index, bg.index);
		if (pairsInUse.find(code) == pairsInUse.end()) {
			short index = 2;
			while (true) {
				if (index >= COLOR_PAIRS) throw exception();
				if (pairIndicesInUse.find(index) == pairIndicesInUse.end()) break;
				index++;
			}
			
			pairIndicesInUse.insert(index);
			pairsInUse[code] = RC(index);
			bool success = curses::setColorPair(index, fg.index, bg.index);
			if (!success) throw exception();
			
			return ColorPair(index);
		} else {
			pairsInUse[code].usages++;
			return ColorPair(pairsInUse[code].value);
		}
	}
	
	Color getColor(unsigned char r, unsigned char g, unsigned char b) {
		unsigned code = colorToInt(r, g, b);
		if (colorsInUse.find(code) == colorsInUse.end()) {
			short index = 1;
			while (true) {
				if (index >= COLORS) throw exception();
				if (colorIndicesInUse.find(index) == colorIndicesInUse.end()) break;
				index++;
			}
			
			colorIndicesInUse.insert(index);
			colorsInUse[code] = RC(index);
			bool success = curses::setColor(index, r, g, b);
			if (!success) throw exception();
			
			return Color(index);
		} else {
			colorsInUse[code].usages++;
			return Color(colorsInUse[code].value);
		}
	}
}
