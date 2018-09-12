/*
 * menu.hpp
 *
 *  Created on: Sep 11, 2018
 *      Author: iconmaster
 */

#ifndef SRC_MENU_HPP_
#define SRC_MENU_HPP_

#include <vector>

#include "curses.hpp"

namespace textx {
	using namespace std;
	
	class MenuItem {
	public:
		inline MenuItem() {}
		virtual ~MenuItem();
		
		// overrideable
		virtual bool leaf();
		
		// abstract
		virtual int getMinWidth();
		virtual void refresh(curses::Window window);
		virtual void onSelected();
	};
	
	class Menu {
	public:
		string name;
		vector<MenuItem*> items;
		
		inline Menu(string name) : name(name), items(items) {}
		inline Menu(string name, vector<MenuItem*> items) : name(name), items(items) {}
	};
	
	typedef vector<Menu> MenuBar;
	
	// Menu control
	extern bool inMenu;
	void refreshMenu();
	void refreshMenuBar();
	void onKeyInMenu(curses::KeyCode key);
	
	void selectMenu(int index);
	void selectMenu(Menu* menu);
	void selectPrevMenu();
	void selectNextMenu();
	void exitMenu();
	
	// MenuItem implementations
	class ButtonMenuItem : public MenuItem {
	public:
		typedef void (*Handler)();
		
		string text;
		Handler handler;
		
		inline ButtonMenuItem(string text, Handler handler) : text(text), handler(handler) {}
		
		int getMinWidth();
		void refresh(curses::Window window);
		void onSelected();
	};
}

#endif /* SRC_MENU_HPP_ */
