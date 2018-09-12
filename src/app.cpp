/*
 * app.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "app.hpp"
#include "pane.hpp"
#include "textx.hpp"

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
	void App::onKey(curses::KeyCode key) {throw exception();}
	MenuBar* App::getMenuBar() {throw exception();}
}
