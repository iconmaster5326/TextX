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
		virtual ~App() = default;
		
		inline Pane* getPane() {
			return pane;
		}
		void setPane(Pane* pane);
		
		virtual void refresh() = 0;
		virtual void onKey(curses::KeyCode key) = 0;
		virtual string getTitle() = 0;
		virtual void close() = 0;
	};
	
	class TextEditorApp : public App {
	public:
		inline TextEditorApp(Pane* pane) : App(pane) {};
		
		void refresh() override;
		void onKey(curses::KeyCode key) override;
		string getTitle() override;
		void close() override;
	};
}

#endif /* SRC_APP_HPP_ */
