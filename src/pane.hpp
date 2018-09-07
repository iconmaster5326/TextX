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

namespace textx {
	using namespace std;
	
	class App;
	
	class Pane {
	private:
		Pane* parent = nullptr;
	protected:
		virtual curses::Window getTitleBar() = 0;
	public:
		virtual ~Pane() = default;
		
		inline Pane* getParent() {
			return parent;
		}
		virtual void setParent(Pane* pane);
		
		virtual vector<Pane*> getChildren() = 0;
		virtual void addChild(Pane* pane) = 0;
		virtual void removeChild(Pane* pane) = 0;
		virtual vector<App*> getApps() = 0;
		virtual void addApp(App* app) = 0;
		virtual void removeApp(App* app) = 0;
		virtual void refreshTitle() = 0;
		virtual void refresh() = 0;
		virtual curses::Window getContent() = 0;
		virtual curses::Window getStatusBar() = 0;
		
		void refreshContent();
	};
	
	class AppPane : public Pane {
	private:
		void init();
		
		App* app = nullptr;
		curses::Window window;
		curses::Window content;
		curses::Window statusBar;
		curses::Window titleBar;
	protected:
		curses::Window getTitleBar() override;
	public:
		AppPane(Pane* parent);
		AppPane(curses::Window window);
		
		vector<Pane*> getChildren() override;
		void addChild(Pane* pane) override;
		void removeChild(Pane* pane) override;
		vector<App*> getApps() override;
		void addApp(App* app) override;
		void removeApp(App* app) override;
		void refreshTitle() override;
		void refresh() override;
		curses::Window getContent() override;
		curses::Window getStatusBar() override;
	};
}

#endif /* SRC_PANE_HPP_ */
