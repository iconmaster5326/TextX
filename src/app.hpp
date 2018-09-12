/*
 * app.hpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#ifndef SRC_APP_HPP_
#define SRC_APP_HPP_

#include <string>

#include "curses.hpp"
#include "menu.hpp"

namespace textx {
	using namespace std;
	
	class Pane;
	
	class App {
	private:
		Pane* pane;
	public:
		App(Pane* pane);
		virtual ~App();
		
		inline Pane* getPane() {
			return pane;
		}
		void setPane(Pane* pane);
		
		// abstract methods
		virtual void refresh();
		virtual void onKey(curses::KeyCode key);
		virtual string getTitle();
		virtual MenuBar* getMenuBar();
	};
}

#endif /* SRC_APP_HPP_ */
