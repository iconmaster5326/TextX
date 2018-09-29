/*
 * tab_pane.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include <algorithm>

#include "tab_pane.hpp"
#include "app.hpp"
#include "colors.hpp"
#include "textx.hpp"

namespace textx {
	using namespace std;
	
	TabPane::TabPane(Pane* parent) : borderColor(getColorPair(color::white, color::blue)) {
		setParent(parent);
		window = parent->getContent();
		initWindow(window);
	}
	
	TabPane::TabPane(curses::Window window) : borderColor(getColorPair(color::white, color::blue)) {
		this->window = window;
		initWindow(window);
	}
	
	TabPane::~TabPane() {
		borderColor.dispose();
		
		for (list<App*>::const_iterator it = apps.begin(); it != apps.end(); it++) {
			delete *it;
		}
	}
	
	vector<Pane*> TabPane::getChildren() {
		return vector<Pane*>();
	}
	void TabPane::addChild(Pane* pane) {
		throw "This pane cannot have children";
	}
	void TabPane::removeChild(Pane* pane) {
		throw "This pane cannot have children";
	}
	vector<App*> TabPane::getApps() {
		return vector<App*>(apps.begin(), apps.end());
	}
	void TabPane::addApp(App* app) {
		list<App*>::iterator it = find(apps.begin(), apps.end(), app);
		if (it == apps.end()) {
			apps.push_back(app);
			app->setPane(this);
		}
		
		currentTab = app;
		refresh();
	}
	void TabPane::removeApp(App* app) {
		if (currentTab == app) currentTab = NULL;
		list<App*>::iterator it = find(apps.begin(), apps.end(), app);
		if (it != apps.end()) {
			apps.erase(it);
		}
	}
	void TabPane::refreshTitleBar() {
		borderColor.use(titleBar);
		window.copyInto(titleBar, false);
		titleBar.setCursor(0, 0);
		for (list<App*>::const_iterator it = apps.begin(); it != apps.end(); it++) {
			App* app = *it;
			if (getFocus() == app) titleBar.enableAttributes(A_REVERSE);
			if (currentTab == app) titleBar.enableAttributes(A_UNDERLINE);
			titleBar.print(app->getTitle());
			titleBar.disableAttributes(A_REVERSE | A_UNDERLINE);
			titleBar.moveCursor(2, 0);
		}
		titleBar.refresh();
	}
	void TabPane::refresh() {
		// draw border
		borderColor.use(window);
		window.drawBorder();
		
		// draw title TODO no code duplication
		borderColor.use(titleBar);
		window.copyInto(titleBar, false);
		titleBar.setCursor(0, 0);
		for (list<App*>::const_iterator it = apps.begin(); it != apps.end(); it++) {
			App* app = *it;
			if (getFocus() == app) titleBar.enableAttributes(A_REVERSE);
			if (currentTab == app) titleBar.enableAttributes(A_UNDERLINE);
			titleBar.print(app->getTitle());
			titleBar.disableAttributes(A_REVERSE | A_UNDERLINE);
			titleBar.moveCursor(2, 0);
		}
		
		// setup drawing for status bar
		borderColor.use(statusBar);
		
		// ensure curses does its thing
		window.refresh();
		titleBar.refresh();
		
		// draw app
		if (currentTab != NULL) currentTab->refresh();
	}
	curses::Window TabPane::getTitleBar() {
		return titleBar;
	}
	curses::Window TabPane::getContent() {
		return content;
	}
	curses::Window TabPane::getStatusBar() {
		return statusBar;
	}
	void TabPane::clearStatusBar() {
		window.copyInto(statusBar, false);
	}
	void TabPane::initWindow(curses::Window window) {
		this->window = window;
		
		int w, h; window.getSize(w, h);
		content = curses::Window(window, 1, 1, w-2, h-2);
		statusBar = curses::Window(window, 2, h-1, w-4, 1); statusBar.setScrollable(false);
		titleBar = curses::Window(window, 2, 0, w-4, 1); titleBar.setScrollable(false);
		
		refresh();
	}
	
	void TabPane::onMouse(curses::MouseEvent mevent) {
		if (content.inWindow(mevent)) {
			currentTab->onMouse(content, mevent);
			if (currentTab != getFocus()) {
				setFocus(currentTab);
				refreshTextX();
			}
		}
		
		if (statusBar.inWindow(mevent)) {
			currentTab->onMouse(statusBar, mevent);
			if (currentTab != getFocus()) {
				setFocus(currentTab);
				refreshTextX();
			}
		}
	}
}
