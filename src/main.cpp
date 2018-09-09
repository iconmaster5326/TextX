/*
 * textx.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "curses.hpp"
#include "textx.hpp"
#include "pane.hpp"
#include "app.hpp"

using namespace std;
using namespace curses;
using namespace textx;

int main(int argc, char** argv) {
	Window screen = startCurses();
	
	AppPane pane = AppPane(screen);
	App* app = new TextEditorApp((Pane*)&pane);
	pane.addApp(app);
	setFocus(app);
	pane.refresh();
	
	while (true) {
		KeyCode key = screen.getKey();
		if (key.value == KEY_F(1)) break;
		onKey(key);
	}
	
	stopCurses();
	return 0;
}
