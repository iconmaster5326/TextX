/*
 * pane.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "pane.hpp"
#include "app.hpp"

namespace textx {
	using namespace std;
	
	// Pane
	void Pane::setParent(Pane* pane) {
		if (pane == parent) return;
		
		parent->removeChild(this);
		parent = pane;
		pane->addChild(this);
	}
	
	void Pane::refreshContent() {
		for (App* app : getApps()) {
			app->refresh();
		}
	}
	
	// AppPane
	void AppPane::init() {
		int w, h; window.getSize(w, h);
		content = curses::Window(window, 1, 1, w-2, h-2);
		statusBar = curses::Window(window, 2, h-1, w-4, 1);
		titleBar = curses::Window(window, 2, 0, w-4, 1);
	}
	
	AppPane::AppPane(Pane* parent) {
		setParent(parent);
		window = parent->getContent();
		init();
	}
	
	AppPane::AppPane(curses::Window window) {
		this->window = window;
		init();
	}
	
	vector<Pane*> AppPane::getChildren() {
		return vector<Pane*>();
	}
	void AppPane::addChild(Pane* pane) {
		throw "This pane cannot have children";
	}
	void AppPane::removeChild(Pane* pane) {
		throw "This pane cannot have children";
	}
	vector<App*> AppPane::getApps() {
		return vector<App*>({app});
	}
	void AppPane::addApp(App* app) {
		if (app == this->app) return;
		this->app = app;
		app->setPane(this);
	}
	void AppPane::removeApp(App* app) {
		if (app != this->app) return;
		this->app = nullptr;
		app->setPane(nullptr);
	}
	void AppPane::refreshTitle() {
		// TODO
	}
	void AppPane::refresh() {
		// TODO draw border
		refreshTitle();
		app->refresh();
	}
	curses::Window AppPane::getTitleBar() {
		return titleBar;
	}
	curses::Window AppPane::getContent() {
		return content;
	}
	curses::Window AppPane::getStatusBar() {
		return statusBar;
	}
}
