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
		unsigned char r, g, b;
		curses::getColor(index, r, g, b);
		int code = colorToInt(r, g, b);
		if (colorsInUse.find(code) != colorsInUse.end()) {
			colorsInUse[code].usages--;
			if (colorsInUse[code].usages == 0) {
				// free index for future use
				colorsInUse.erase(code);
				colorIndicesInUse.insert(index);
			}
		}
	}
	
	ColorPair::ColorPair(short index) {
		this->index = index;
	}
	
	void ColorPair::dispose() {
		short fg, bg;
		curses::getColorPair(index, fg, bg);
		Color(fg).dispose(); Color(bg).dispose();
		int code = pairToInt(fg, bg);
		if (pairsInUse.find(code) != pairsInUse.end()) {
			pairsInUse[code].usages--;
			if (pairsInUse[code].usages == 0) {
				// free index for future use
				pairsInUse.erase(code);
				pairIndicesInUse.insert(index);
			}
		}
	}
	
	ColorPair getColorPair(Color fg, Color bg) {
		unsigned code = pairToInt(fg.index, bg.index);
		if (pairsInUse.find(code) == pairsInUse.end()) {
			short index = 1;
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
		// truncate the color values, as curses.hpp does, so they convert to and from curses color values properly
		// (if you don't do this, dispose() will attempt to dispose the wrong color!)
		r = (((r/255.0)*1000.0)/1000.0)*255.0;
		g = (((g/255.0)*1000.0)/1000.0)*255.0;
		b = (((b/255.0)*1000.0)/1000.0)*255.0;
		// check the cache
		unsigned code = colorToInt(r, g, b);
		if (colorsInUse.find(code) == colorsInUse.end()) {
			short index = 8;
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
	
	namespace color {
		Color system = Color(-1);
		Color black = Color(COLOR_BLACK);
		Color red = Color(COLOR_RED);
		Color green = Color(COLOR_GREEN);
		Color yellow = Color(COLOR_YELLOW);
		Color blue = Color(COLOR_BLUE);
		Color magenta = Color(COLOR_MAGENTA);
		Color cyan = Color(COLOR_CYAN);
		Color white = Color(COLOR_WHITE);
		
		namespace pair {
			ColorPair system = ColorPair(0);
		}
	}
}
