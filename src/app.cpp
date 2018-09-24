/*
 * app.cpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#include "app.hpp"
#include "pane.hpp"
#include "textx.hpp"

#include <algorithm>
#include <exception>

namespace textx {
	using namespace std;
	
	set<AppInfo*> allApps;
	
	AppInfo::~AppInfo() {}

	App* AppInfo::open(Pane* pane) {
		throw "app cannot be opened";
	}
	
	App::App(AppInfo* info, Pane* pane) {
		this->info = info;
		this->pane = pane;
		getAllApps()->push_back(this);
	}
	
	App::~App() {
		vector<App*>* apps = getAllApps();
		vector<App*>::iterator it = find(apps->begin(),apps->end(), this);
		if (it != apps->end()) apps->erase(it);
		
		// TODO
	}
	
	void App::close() {
		// exit TextX if we're the only app TODO do something else
		if (getAllApps()->size() == 1) {
			curses::stopCurses();
			exit(0);
		}
		
		// close this editor
		getPane()->removeApp(this);
		
		// find a new app if needed
		if (this == getFocus()) {
			vector<App*> apps = getPane()->getApps();
			if (apps.empty()) {
				setFocus(NULL);
			} else {
				setFocus(apps.front());
				getPane()->addApp(apps.front());
			}
		}
		
		// refresh
		getPane()->refresh();
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
