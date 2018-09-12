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

#include "text_editor.hpp"

using namespace std;
using namespace curses;
using namespace textx;

int main(int argc, char** argv) {
	startCurses();
	Window notMenuBar = Window(0, 1, COLS, LINES-1);
	
	AppPane pane = AppPane(notMenuBar);
	getRootPanes()->push_back(&pane);
	
	App* app;
	if (argc <= 1) {
		app = new TextEditorApp((Pane*)&pane);
	} else {
		app = new TextEditorApp((Pane*)&pane, argv[1]);
	}
	
	pane.addApp(app);
	setFocus(app);
	
	refreshMenuBar();
	pane.refresh();
	
	bool alt = false;
	while (true) {
		KeyCode key = notMenuBar.getKey();
		
		if (alt) {
			alt = false;
			
			switch (key.value) {
			case 27: // the user triple-escaped; eat the next ESC and send ESC to TextX
				if (inMenu) {
					onKeyInMenu(key);
				} else {
					onKey(key);
				}
				notMenuBar.getKey();
			}
			
			continue;
		}
		
		switch (key.value) {
		case KEY_F(1): selectMenu(0); break;
		case KEY_F(2): selectMenu(1); break;
		case KEY_F(3): selectMenu(2); break;
		case KEY_F(4): selectMenu(3); break;
		case KEY_F(5): selectMenu(4); break;
		case KEY_F(6): selectMenu(5); break;
		case KEY_F(7): selectMenu(6); break;
		case KEY_F(8): selectMenu(7); break;
		case KEY_F(9): selectMenu(8); break;
		case KEY_F(10): selectMenu(9); break;
		case KEY_F(11): selectMenu(10); break;
		case KEY_F(12): selectMenu(11); break;
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
