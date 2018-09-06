/*
 * textx.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "curses.hpp"

#include <cctype>

using namespace std;
using namespace curses;
// using namespace textx;

int main(int argc, char** argv) {
	Window win = curses::startCurses();
	
	win.println("Key tester. Press F1 to exit.");
	while (true) {
		KeyCode key = win.getKey();
		if (key.value == KEY_F(1)) break;
		win.printf("NAME: %s CODE: %d CHAR: %c\n", key.name().c_str(), (int)key.value, (char)key.value);
	}
	
	stopCurses();
	return 0;
}
