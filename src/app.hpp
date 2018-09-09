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

namespace textx {
	using namespace std;
	
	class Pane;
	
	class App {
	private:
		Pane* pane;
	public:
		inline App(Pane* pane) {
			this->pane = pane;
		}
		virtual ~App();
		
		inline Pane* getPane() {
			return pane;
		}
		void setPane(Pane* pane);
		
		// abstract methods
		virtual void refresh();
		virtual void onKey(curses::KeyCode key);
		virtual string getTitle();
		virtual void close();
	};
	
	class TextEditorApp : public App {
	public:
		inline TextEditorApp(Pane* pane) : App(pane) {};
		
		void refresh();
		void onKey(curses::KeyCode key);
		string getTitle();
		void close();
	};
}

#endif /* SRC_APP_HPP_ */
