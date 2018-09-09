/*
 * app.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "app.hpp"
#include "pane.hpp"
#include "textx.hpp"
#include "colors.hpp"

#include <cstdlib>
#include <exception>

namespace textx {
	using namespace std;
	
	// App
	App::App(Pane* pane) {
		this->pane = pane;
		getAllApps()->push_back(this);
	}
	
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
	static void drawTextEditorStatusBar(Pane* pane, curses::Window win) {
		int x, y; win.getCursor(x, y);
		curses::Window status = pane->getStatusBar();
		pane->clearStatusBar();
		status.setCursor(0, 0);
		status.printf("Line %d", y);
		status.moveCursor(2, 0);
		status.printf("Col %d", x);
		
		status.refresh();
	}
	void TextEditorApp::refresh() {
		curses::Window win = getPane()->getContent();
		color::pair::system.use(win);
		
		win.setCursor(0, 0);
		win.print("Your terminal is: ");
		char* term = getenv("TERM");
		if (term != NULL) win.println(string(term)); else win.println("($TERM not set)");
		win.println("Press F1 to exit.");
		drawTextEditorStatusBar(getPane(), win);
		win.refresh();
		
		// TODO
	}
	void TextEditorApp::onKey(curses::KeyCode key) {
		Pane* pane = getPane();
		curses::Window win = pane->getContent();
		switch (key.value) {
		case KEY_LEFT: win.moveCursor(-1, 0); break;
		case KEY_RIGHT: win.moveCursor(1, 0); break;
		case KEY_UP: win.moveCursor(0, -1); break;
		case KEY_DOWN: win.moveCursor(0, 1); break;
		default: win.print(key.value);
		}
		
		drawTextEditorStatusBar(pane, win);
		win.refresh();
		
		// TODO
	}
	string TextEditorApp::getTitle() {
		return "Test App";
	}
	void TextEditorApp::close() {
		
	}
}
