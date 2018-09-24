/*
 * app.hpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#ifndef SRC_APP_HPP_
#define SRC_APP_HPP_

#include <string>
#include <set>

#include "curses.hpp"
#include "menu.hpp"
#include "hotkeys.hpp"

namespace textx {
	using namespace std;
	
	class Pane; class HotkeyStore;
	
	class AppInfo {
	public:
		string name;
		HotkeyStore hotkeys;
		
		inline AppInfo() {}
		inline AppInfo(string name) {
			this->name = name;
		}
		virtual ~AppInfo();
		
		virtual App* open(Pane* pane);
	};
	
	class App {
	private:
		Pane* pane;
	public:
		AppInfo* info;
		
		App(AppInfo* info, Pane* pane);
		virtual ~App();
		
		inline Pane* getPane() {
			return pane;
		}
		void setPane(Pane* pane);
		void close();
		
		// abstract methods
		virtual void refresh();
		virtual void onKey(curses::KeyCode key);
		virtual string getTitle();
		virtual MenuBar* getMenuBar();
	};
	
	extern set<AppInfo*> allApps;
}

#endif /* SRC_APP_HPP_ */
