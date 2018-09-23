/*
 * tab_pane.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#ifndef SRC_TAB_PANE_HPP_
#define SRC_TAB_PANE_HPP_

#include <list>

#include "pane.hpp"

namespace textx {
	using namespace std;
	
	class TabPane : public Pane {
	private:
		void init();
		
		ColorPair borderColor;
		curses::Window window;
		curses::Window content;
		curses::Window statusBar;
		curses::Window titleBar;
	protected:
		curses::Window getTitleBar();
	public:
		App* currentTab = NULL;
		list<App*> apps;
		
		TabPane(Pane* parent);
		TabPane(curses::Window window);
		~TabPane();
		
		vector<Pane*> getChildren();
		void addChild(Pane* pane);
		void removeChild(Pane* pane);
		vector<App*> getApps();
		void addApp(App* app);
		void removeApp(App* app);
		void refreshTitleBar();
		void refresh();
		curses::Window getContent();
		curses::Window getStatusBar();
		void clearStatusBar();
		void initWindow(curses::Window window);
	};
}

#endif /* SRC_TAB_PANE_HPP_ */
