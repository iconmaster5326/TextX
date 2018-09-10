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
#include "colors.hpp"

using namespace std;
using namespace curses;
using namespace textx;

static void drawMenuBar(Window menuBar) {
	menuBar.setCursor(0, 0);
	menuBar.print("File  Edit  View  Help");
	menuBar.setCursor(COLS-5, 0);
	menuBar.print("TextX");
	menuBar.refresh();
}

int main(int argc, char** argv) {
	startCurses();
	Window menuBar = Window(0, 0, COLS, 1); menuBar.setScrollable(false);
	Window notMenuBar = Window(0, 1, COLS, LINES-1);
	
	color::pair::system.use(menuBar);
	menuBar.enableAttributes(A_BOLD);
	drawMenuBar(menuBar);
	
	AppPane pane = AppPane(notMenuBar);
	App* app = new TextEditorApp((Pane*)&pane);
	pane.addApp(app);
	setFocus(app);
	pane.refresh();
	
	while (true) {
		KeyCode key = notMenuBar.getKey();
		switch (key.value) {
		case KEY_F(1): goto end;
		// TODO case KEY_RESIZE: drawMenuBar(menuBar); pane.refresh(); break;
		default: onKey(key);
		}
	}
	
	end:
	stopCurses();
	return 0;
}
