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
#include "menu.hpp"

#include "tab_pane.hpp"
#include "text_editor.hpp"

using namespace std;
using namespace curses;
using namespace textx;

int main(int argc, char** argv) {
	startCurses();
	Window notMenuBar = Window(0, 1, COLS, LINES-1);
	
	TabPane pane = TabPane(notMenuBar);
	getRootPanes()->push_back(&pane);
	
	App* app;
	if (argc <= 1) {
		app = new TextEditorApp((Pane*)&pane);
	} else {
		app = new TextEditorApp((Pane*)&pane, argv[1]);
	}
	
	App* app2;
	if (argc <= 2) {
		app2 = new TextEditorApp((Pane*)&pane);
	} else {
		app2 = new TextEditorApp((Pane*)&pane, argv[2]);
	}
	
	pane.addApp(app);
	pane.addApp(app2);
	setFocus(app2);
	
	refreshMenuBar();
	pane.refresh();
	
	bool alt = false;
	mainLoop: while (true) {
		KeyCode key = notMenuBar.getKey();
		
		if (alt) {
			alt = false;
			
			switch (key.value) {
			case 27: { // ESC
				// the user double-escaped; send ESC to TextX and immediately grab the next char
				if (inMenu) {
					onKeyInMenu(key);
				} else {
					onKey(key);
				}
				
				int escDelay = ESCDELAY;
				set_escdelay(1);
				key = notMenuBar.getKey();
				set_escdelay(ESCDELAY);
				break;
			} default:
				goto mainLoop;
			}
		}
		
		switch (key.value) {
		case 27: // escape
			alt = true;
			break;
		case KEY_RESIZE:
			refreshMenu();
			refreshMenuBar();
			
			notMenuBar.dispose();
			notMenuBar = Window(0, 1, COLS, LINES-1);
			pane.initWindow(notMenuBar);
			break;
		default:
			if (inMenu) {
				onKeyInMenu(key);
			} else {
				onKey(key);
			}
		}
	}
	
	// unreachable
	return 1;
}
