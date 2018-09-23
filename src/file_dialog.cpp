/*
 * file_dialog.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include "file_dialog.hpp"

namespace textx {
	using namespace std;
	
	textx::FileDialogApp::FileDialogApp(Pane* thePane, string suggested, FileDialogHandler handler, void* cookie) : App(thePane) {
		curses::Window win = thePane->getContent();
		int w, h; win.getSize(w, h);
		lineEditor = LineEditor(win, 2, h/2, w-4, suggested);
		
		this->handler = handler;
		this->cookie = cookie;
	}

	void textx::FileDialogApp::refresh() {
		curses::Window win = getPane()->getContent();
		int w, h; win.getSize(w, h);
		if (lineEditor.win.raw != win.raw) {
			lineEditor.win = win;
			lineEditor.y = h/2;
			lineEditor.w = w-4;
		}
		
		string header = "Enter the filename you wish to choose:";
		win.setCursor(w/2-header.size()/2, 1);
		win.print(header);
		
		lineEditor.refresh();
		win.refresh();
	}

	void textx::FileDialogApp::onKey(curses::KeyCode key) {		
		lineEditor.onKey(key);
		
		if (lineEditor.done) {
			handler(true, lineEditor.buffer, cookie);
		}
	}

	string textx::FileDialogApp::getTitle() {
		return "Choose a File";
	}

	MenuBar* textx::FileDialogApp::getMenuBar() {
		return &menuBar;
	}
}
