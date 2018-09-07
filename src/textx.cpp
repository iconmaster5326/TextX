/*
 * textx.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "curses.hpp"
#include "pane.hpp"
#include "app.hpp"

#include <cctype>

using namespace std;
using namespace curses;
using namespace textx;

int main(int argc, char** argv) {
	Window screen = startCurses();
	
	AppPane pane = AppPane(screen);
	pane.addApp(new TextEditorApp((Pane*)&pane));
	pane.refresh();
	
	screen.getKey();
	stopCurses();
	return 0;
}
