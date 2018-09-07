/*
 * app.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "app.hpp"
#include "pane.hpp"

namespace textx {
	using namespace std;
	
	// App
	void App::setPane(Pane* pane) {
		if (this->pane == pane) return;
		
		this->pane->removeApp(this);
		this->pane = pane;
		pane->addApp(this);
	}
	
	// TextEditorApp
	void TextEditorApp::refresh() {
		curses::Window win = getPane()->getContent();
		
		win.setCursor(0, 0);
		win.println("Hello, World!");
		win.refresh();
		
		// TODO
	}
	void TextEditorApp::onKey(curses::KeyCode key) {
		// TODO
	}
	string TextEditorApp::getTitle() {
		return "";
	}
	void TextEditorApp::close() {
		
	}
}
