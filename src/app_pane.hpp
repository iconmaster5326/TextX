/*
 * app_pane.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#ifndef SRC_APP_PANE_HPP_
#define SRC_APP_PANE_HPP_

#include "pane.hpp"

namespace textx {
	using namespace std;
	
	class App;
	
	class AppPane : public Pane {
	private:
		void init();
		
		App* app = NULL;
		ColorPair borderColor;
		curses::Window window;
		curses::Window content;
		curses::Window statusBar;
		curses::Window titleBar;
	protected:
		curses::Window getTitleBar();
	public:
		AppPane(Pane* parent);
		AppPane(curses::Window window);
		~AppPane();
		
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

#endif /* SRC_APP_PANE_HPP_ */
