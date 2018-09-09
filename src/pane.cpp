/*
 * pane.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "pane.hpp"
#include "app.hpp"
#include "colors.hpp"

#include <exception>

namespace textx {
	using namespace std;
	
	// Pane
	Pane::~Pane() {
		// TODO
	}
	
	void Pane::setParent(Pane* pane) {
		if (pane == parent) return;
		
		parent->removeChild(this);
		parent = pane;
		pane->addChild(this);
	}
	
	void Pane::refreshContent() {
		vector<App*> apps = getApps();
		for (vector<App*>::const_iterator it = apps.begin(); it != apps.end(); it++) {
			App* app = *it;
			app->refresh();
		}
	}
	
	curses::Window Pane::getTitleBar() {throw exception();}
	vector<Pane*> Pane::getChildren() {throw exception();}
	void Pane::addChild(Pane* pane) {throw exception();}
	void Pane::removeChild(Pane* pane) {throw exception();}
	vector<App*> Pane::getApps() {throw exception();}
	void Pane::addApp(App* app) {throw exception();}
	void Pane::removeApp(App* app) {throw exception();}
	void Pane::refresh() {throw exception();}
	curses::Window Pane::getContent() {throw exception();}
	curses::Window Pane::getStatusBar() {throw exception();}
	
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
		vector<App*> apps;
		apps.push_back(app);
		return apps;
	}
	void AppPane::addApp(App* app) {
		if (app == this->app) return;
		this->app = app;
		app->setPane(this);
	}
	void AppPane::removeApp(App* app) {
		if (app != this->app) return;
		this->app = NULL;
		app->setPane(NULL);
	}
	void AppPane::refresh() {
		// draw border
		ColorPair color = getColorPair(getColor(255, 255, 255), getColor(0, 0, 128));
		color.use(window);
		window.drawBorder();
		
		//draw title
		if (app != NULL) {
			color.use(titleBar);
			titleBar.setCursor(0, 0);
			titleBar.print(app->getTitle());
		}
		
		// ensure curses does its thing
		window.refresh();
		
		// draw app
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
