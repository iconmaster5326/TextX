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
	Window screen = startCurses();
	
//	win.println("Key tester. Press F1 to exit.");
//	while (true) {
//		KeyCode key = win.getKey();
//		if (key.value == KEY_F(1)) break;
//		win.printf("NAME: %s CODE: %d CHAR: %c\n", key.name().c_str(), (int)key.value, (char)key.value);
//	}
#define PAIR_DEFAULT 1
#define PAIR_BORDER 2
#define PAIR_SEL 3
#define PAIR_LINE 4
	
	setColorPair(PAIR_DEFAULT, COLOR_WHITE, COLOR_BLACK);
	setColorPair(PAIR_BORDER, COLOR_WHITE, COLOR_BLUE);
	setColorPair(PAIR_SEL, COLOR_BLACK, COLOR_WHITE);
	setColorPair(PAIR_LINE, COLOR_YELLOW, COLOR_BLACK);
	
	Window win = Window(0, 1, COLS, LINES-1);
	win.setColor(PAIR_BORDER);
	win.drawBorder();
	
	screen.moveCursor(0, 0);
	screen.setAttributesAtLine(A_BOLD, PAIR_DEFAULT); screen.setColor(PAIR_DEFAULT); screen.enableAttributes(A_BOLD);
	screen.print("File  Edit  View  Window  Help");
	screen.setCursor(COLS-5, 0);
	screen.print("TextX");
	screen.disableAttributes(A_BOLD);
	
	win.setColor(PAIR_SEL);
	win.setCursor(2, 0);
	win.print("file1.txt");
	win.moveCursor(2, 0);
	win.setColor(PAIR_BORDER);
	win.print("file2.sh");
	win.moveCursor(2, 0);
	win.print("file3.makefile");
	
	win.setColor(PAIR_LINE);
	for (int i = 0; i < win.height()-2; i++) {
		win.setCursor(1, i+1);
		win.printf("%2d:", i+1);
	}
	
	win.setCursor(5, 1);
	win.setColor(PAIR_DEFAULT);
	win.print("Hello, world!");
	
	win.setCursor(1, win.height()-1);
	win.setColor(PAIR_BORDER);
	win.print("INS");
	win.moveCursor(2, 0);
	win.print("Line: 1");
	win.moveCursor(2, 0);
	win.print("Col: 1");
	win.moveCursor(2, 0);
	win.print("Plain Text");
	
	screen.refresh();
	win.refresh();
	screen.getKey();
	stopCurses();
	return 0;
}
