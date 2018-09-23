/*
 * textx.cpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */


#include "textx.hpp"
#include "app.hpp"
#include "pane.hpp"

namespace textx {
	using namespace std;
	
	static string clipboard;
	
	string getClipboard() {
		return clipboard;
	}
	
	void setClipboard(string s) {
		clipboard = s;
	}
	
	static App* focus;
	static vector<App*> apps;
	static vector<Pane*> rootPanes;
	
	App* getFocus() {
		return focus;
	}
	
	void setFocus(App* app) {
		focus = app;
	}
	
	vector<App*>* getAllApps() {
		return &apps;
	}
	
	vector<Pane*>* getRootPanes() {
		return &rootPanes;
	}
	
	void onKey(curses::KeyCode key) {
		focus->onKey(key);
	}
}
