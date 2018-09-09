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
		Color(short index);
	public:
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
		ColorPair(short index);
	public:
		inline short getIndex() {
			return index;
		}
		void dispose();
		inline void use(curses::Window win) {
			win.setColor(index);
		}
	};
}

#endif /* SRC_COLORS_HPP_ */
