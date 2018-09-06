/*
 * textx.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include <curses.h>

int main(int argc, char** argv) {
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	
	wprintw(stdscr, "Hello, World!");
	getch();
	
	endwin();
	return 0;
}
