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
	
	if (argc <= 1) {
		App* app = new TextEditorApp((Pane*)&pane);
		pane.addApp(app);
		setFocus(app);
	} else {
		for (int i = 1; i < argc; i++) {
			App* app = new TextEditorApp((Pane*)&pane, string(argv[i]));
			pane.addApp(app);
			setFocus(app);
		}
	}
	
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
				set_escdelay(escDelay);
				break;
			} default: {
				// an alt-modified key; tack on our ALT flag and continue processing it below
				key.value = MAKE_ALTKEY(key.value);
			}
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
