/*
 * window.hpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#ifndef SRC_PANE_HPP_
#define SRC_PANE_HPP_

#include <string>

#include "src/curses.hpp"

namespace textx {
	using namespace std;
	
	class Pane { public:
		curses::Window window;
		
		Pane();
		virtual ~Pane();
	};
}


#endif /* SRC_PANE_HPP_ */
