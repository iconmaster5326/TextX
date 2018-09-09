

/*
 * textx.hpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#ifndef SRC_TEXTX_HPP_
#define SRC_TEXTX_HPP_

#include <vector>

#include "curses.hpp"

namespace textx {
	using namespace std;
	
	class App;
	
	// app control
	App* getFocus();
	void setFocus(App* app);
	vector<App*>* getAllApps();
	
	// event handlers
	void onKey(curses::KeyCode key);
}


#endif /* SRC_TEXTX_HPP_ */
