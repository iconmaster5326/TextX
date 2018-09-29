/*
 * key_tester.cpp
 *
 *  Created on: Sep 11, 2018
 *      Author: iconmaster
 */

#include "curses.hpp"

#include <cctype>
#include <cstdlib>

using namespace curses;
using namespace std;

bool shouldShowChar(int c) {
	if (c <= 0 || c > 127) return false;
	return (c != '\n' && c != '\r' && c != '\b');
}

int main(int argc, char** argv) {
	Window screen = startCurses();
	
	screen.printf("$TERM: %s COLORS: %d COLOR_PAIRS: %d\nPress q to exit.\n\n", getenv("TERM"), COLORS, COLOR_PAIRS);
	
	while (true) {
		KeyCode c = screen.getKey();
		if (c.value == 'q') break;
		screen.printf("KEY: %s CODE: %d", c.name().c_str(), c.value);
		if (shouldShowChar(c.value)) {
			screen.printf(" CHAR: %c\n", (char)c.value);
		} else {
			screen.println();
			if (c.isMouseEvent()) {
				MouseEvent mevent = c.asMouseEvent();
				
				int button = -1;
				for (int i = 1; i <= 5; i++) {
					if (mevent.button(i)) {
						button = i;
					}
				}
				
				string type;
				if (mevent.up(button)) {
					type = "UP";
				} else if (mevent.down(button)) {
					type = "DOWN";
				} else if (mevent.click(button)) {
					type = "CLICK";
				} else if (mevent.doubleClick(button)) {
					type = "DOUBLE_CLICK";
				} else if (mevent.tripleClick(button)) {
					type = "TRIPLE_CLICK";
				} else {
					type = "???";
				}
				
				screen.printf("X: %d Y: %d BUTTON: %d TYPE: %s SHIFT? %d CTRL? %d ALT? %d\n", mevent.x(), mevent.y(), button, type.c_str(), mevent.shift(), mevent.control(), mevent.alt());
			}
		}
	}
	
	stopCurses();
	return 0;
}
