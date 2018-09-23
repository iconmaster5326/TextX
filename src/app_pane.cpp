/*
 * app_pane.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include "app_pane.hpp"
#include "app.hpp"
#include "colors.hpp"
#include "textx.hpp"

namespace textx {
	using namespace std;
	
	AppPane::AppPane(Pane* parent) : borderColor(getColorPair(color::white, color::blue)) {
		setParent(parent);
		window = parent->getContent();
		initWindow(window);
	}
	
	AppPane::AppPane(curses::Window window) : borderColor(getColorPair(color::white, color::blue)) {
		this->window = window;
		initWindow(window);
	}
	
	AppPane::~AppPane() {
		borderColor.dispose();
		if (app != NULL) delete app;
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
		vector<App*> apps;
		apps.push_back(app);
		return apps;
	}
	void AppPane::addApp(App* app) {
		if (app == this->app) return;
		this->app = app;
		app->setPane(this);
		refresh();
	}
	void AppPane::removeApp(App* app) {
		if (app != this->app) return;
		this->app = NULL;
		app->setPane(NULL);
	}
	void AppPane::refreshTitleBar() {
		borderColor.use(titleBar);
		window.copyInto(titleBar, false);
		if (app != NULL) {
			titleBar.setCursor(0, 0);
			if (getFocus() == app) titleBar.enableAttributes(A_REVERSE);
			titleBar.print(app->getTitle());
		}
		titleBar.refresh();
	}
	void AppPane::refresh() {
		// draw border
		borderColor.use(window);
		window.drawBorder();
		
		// draw title TODO no code duplication
		borderColor.use(titleBar);
		window.copyInto(titleBar, false);
		if (app != NULL) {
			titleBar.setCursor(0, 0);
			if (getFocus() == app) titleBar.enableAttributes(A_REVERSE);
			titleBar.print(app->getTitle());
		}
		
		// setup drawing for status bar
		borderColor.use(statusBar);
		
		// ensure curses does its thing
		window.refresh();
		titleBar.refresh();
		
		// draw app
		if (app != NULL) app->refresh();
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
	void AppPane::clearStatusBar() {
		window.copyInto(statusBar, false);
	}
	void AppPane::initWindow(curses::Window window) {
		this->window = window;
		
		int w, h; window.getSize(w, h);
		content = curses::Window(window, 1, 1, w-2, h-2);
		statusBar = curses::Window(window, 2, h-1, w-4, 1); statusBar.setScrollable(false);
		titleBar = curses::Window(window, 2, 0, w-4, 1); titleBar.setScrollable(false);
		
		refresh();
	}
}
