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
	void Pane::refreshTitleBar() {throw exception();}
	void Pane::refresh() {throw exception();}
	curses::Window Pane::getContent() {throw exception();}
	curses::Window Pane::getStatusBar() {throw exception();}
	void Pane::clearStatusBar() {throw exception();}
	void Pane::initWindow(curses::Window window) {throw exception();}
}
