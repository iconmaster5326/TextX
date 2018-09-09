/*
 * app.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "app.hpp"
#include "pane.hpp"

#include <cstdlib>
#include <exception>

namespace textx {
	using namespace std;
	
	// App
	App::~App() {
		// TODO
	}
	
	void App::setPane(Pane* pane) {
		if (this->pane == pane) return;
		
		this->pane->removeApp(this);
		this->pane = pane;
		pane->addApp(this);
	}
	
	void App::refresh() {throw exception();}
	string App::getTitle() {throw exception();}
	void App::close() {throw exception();}
	void App::onKey(curses::KeyCode key) {throw exception();}
	
	// TextEditorApp
	void TextEditorApp::refresh() {
		curses::Window win = getPane()->getContent();
		
		win.setCursor(0, 0);
		char* term = getenv("TERM");
		if (term != NULL) win.println(string(term)); else win.println("($TERM not set)");
		win.refresh();
		
		// TODO
	}
	void TextEditorApp::onKey(curses::KeyCode key) {
		// TODO
	}
	string TextEditorApp::getTitle() {
		return "Test App";
	}
	void TextEditorApp::close() {
		
	}
}
