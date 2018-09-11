/*
 * key_tester.cpp
 *
 *  Created on: Sep 11, 2018
 *      Author: iconmaster
 */

#include "curses.hpp"

#include <cctype>

using namespace curses;
using namespace std;

bool shouldShowChar(int c) {
	if (c <= 0 || c > 127) return false;
	return (c != '\n' && c != '\r' && c != '\b');
}

int main(int argc, char** argv) {
	Window screen = startCurses();
	
	while (true) {
		KeyCode c = screen.getKey();
		if (c.value == 'q') break;
		screen.printf("KEY: %s CODE: %d", c.name().c_str(), c.value);
		if (shouldShowChar(c.value)) {
			screen.printf(" CHAR: %c\n", (char)c.value);
		} else {
			screen.println();
		}
	}
	
	stopCurses();
	return 0;
}
