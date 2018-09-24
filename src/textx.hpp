

/*
 * textx.hpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#ifndef SRC_TEXTX_HPP_
#define SRC_TEXTX_HPP_

#if defined(__CYGWIN__) || defined(__CYGWIN32__) || \
	defined(__MINGW32__) || defined(__MINGW64__) || \
	defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__) || \
	defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
#define TEXTX_WINDOWS 1
#endif

#include <vector>

#include "curses.hpp"

namespace textx {
	using namespace std;
	
	class App; class Pane;
	
	// clipboard control
	string getClipboard();
	void setClipboard(string s);
	
	// app control
	App* getFocus();
	void setFocus(App* app);
	vector<App*>* getAllApps();
	
	// pane control
	vector<Pane*>* getRootPanes();
	void selectLeftPane();
	void selectRightPane();
	
	// event handlers
	const int ALTKEY = 512;
	inline int MAKE_ALTKEY(int keycode) {
		return keycode | ALTKEY;
	}
	inline bool IS_ALTKEY(int keycode) {
		return keycode & ALTKEY;
	}
	inline int GET_ALTKEY(int keycode) {
		return keycode & ~ALTKEY;
	}
	void onKey(curses::KeyCode key);
	
	// screen control
	void refreshTextX();
}


#endif /* SRC_TEXTX_HPP_ */
