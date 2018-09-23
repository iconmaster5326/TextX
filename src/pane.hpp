/*
 * window.hpp
 *
 *  Created on: Sep 6, 2018
 *      Author: iconmaster
 */

#ifndef SRC_PANE_HPP_
#define SRC_PANE_HPP_

#include <string>
#include <vector>

#include "curses.hpp"
#include "colors.hpp"

namespace textx {
	using namespace std;
	
	class App;
	
	class Pane {
	private:
		Pane* parent = NULL;
	protected:
		virtual curses::Window getTitleBar(); // abstract
	public:
		virtual ~Pane();
		
		inline Pane* getParent() {
			return parent;
		}
		virtual void setParent(Pane* pane);
		void refreshContent();
		
		// abstract methods
		virtual vector<Pane*> getChildren();
		virtual void addChild(Pane* pane);
		virtual void removeChild(Pane* pane);
		virtual vector<App*> getApps();
		virtual void addApp(App* app);
		virtual void removeApp(App* app);
		virtual void refreshTitleBar();
		virtual void refresh();
		virtual curses::Window getContent();
		virtual curses::Window getStatusBar();
		virtual void clearStatusBar();
		virtual void initWindow(curses::Window window);
	};
}

#endif /* SRC_PANE_HPP_ */
