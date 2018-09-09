/*
 * textx.cpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */


#include "textx.hpp"
#include "app.hpp"

namespace textx {
	using namespace std;
	
	static App* focus;
	static vector<App*> apps;
	
	App* getFocus() {
		return focus;
	}
	
	void setFocus(App* app) {
		focus = app;
	}
	
	vector<App*> getAllApps() {
		return apps;
	}
	
	void onKey(curses::KeyCode key) {
		focus->onKey(key);
	}
}
