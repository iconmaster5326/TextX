/*
 * colors.hpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#ifndef SRC_COLORS_HPP_
#define SRC_COLORS_HPP_

#include "curses.hpp"

namespace textx {
	using namespace std;
	
	class Color;
	class ColorPair;
	
	ColorPair getColorPair(Color fg, Color bg);
	Color getColor(unsigned char r, unsigned char g, unsigned char b);
	
	class Color {
	friend Color getColor(unsigned char r, unsigned char g, unsigned char b);
	friend ColorPair getColorPair(Color fg, Color bg);
	private:
		short index;
	public:
		Color(short index);
		inline short getIndex() {
			return index;
		}
		void dispose();
	};
	
	class ColorPair {
	friend Color getColor(unsigned char r, unsigned char g, unsigned char b);
	friend ColorPair getColorPair(Color fg, Color bg);
	private:
		short index;
	public:
		ColorPair(short index);
		inline short getIndex() {
			return index;
		}
		void dispose();
		inline void use(curses::Window win) {
			win.setColor(index);
		}
	};
	
	namespace color {
		// color ID -1: the default foreground or background color for the terminal.
		extern Color system;
		// color constants built into curses.
		extern Color black;
		extern Color red;
		extern Color green;
		extern Color yellow;
		extern Color blue;
		extern Color magenta;
		extern Color cyan;
		extern Color white;
		// the system pair: that is, (-1,-1)
		namespace pair {
			extern ColorPair system;
		}
	}
}

#endif /* SRC_COLORS_HPP_ */
