/*
 * menu.cpp
 *
 *  Created on: Sep 11, 2018
 *      Author: iconmaster
 */

#include <exception>
#include <vector>
#include <algorithm>

#include "textx.hpp"
#include "menu.hpp"
#include "colors.hpp"
#include "app.hpp"
#include "pane.hpp"

namespace textx {
	using namespace std;
	
	// MenuManager
	bool inMenu;
	
	static Menu* currentMenu;
	static vector<Menu*> menuHistory;
	static curses::Window menuWin;
	static ColorPair menuColor = color::pair::system;
	static int menuItemAt;
	
	void refreshMenu() {
		if (!inMenu) return;
		
		menuColor.use(menuWin);
		menuWin.drawBorder();
		
		int i = 0;
		for (vector<MenuItem*>::const_iterator it = currentMenu->items.begin(); it != currentMenu->items.end(); it++) {
			int w = menuWin.width()-2;
			curses::Window menuItemWin(menuWin, 1, i+1, w, 1); menuItemWin.setScrollable(false);
			menuColor.use(menuItemWin);
			menuItemWin.setBackgroundColor(menuColor.getIndex());
			(*it)->refresh(menuItemWin);
			if (menuItemAt == i) {
				menuItemWin.setCursor(0, 0);
				menuItemWin.setAttributesAt(A_REVERSE, menuColor.getIndex(), w);
			}
			menuItemWin.copyInto(menuWin, false);
			menuItemWin.dispose();
			i++;
		}
		
		menuWin.setCursor(1, menuItemAt+1);
		menuWin.refresh();
	}
	
	static curses::Window menuBar;
	static bool menuBarInit = false;
	void refreshMenuBar() {
		if (!menuBarInit) {
			menuBar = curses::Window(0, 0, COLS, 1);
			menuBar.setScrollable(false);
			menuBarInit = true;
		} else if (menuBar.width() != COLS) {
			menuBar.dispose();
			menuBar = curses::Window(0, 0, COLS, 1);
			menuBar.setScrollable(false);
		}
		
		color::pair::system.use(menuBar);
		
		if (getFocus() != NULL) {
			menuBar.setCursor(0, 0);
			MenuBar* bar = getFocus()->getMenuBar();
			for (MenuBar::const_iterator it = bar->begin(); it != bar->end(); it++) {
				vector<Menu*>::const_iterator inHistory = std::find(menuHistory.begin(), menuHistory.end(), &(*it));
				if (inHistory != menuHistory.end()) menuBar.enableAttributes(A_REVERSE);
				menuBar.print(it->name);
				if (inHistory != menuHistory.end()) menuBar.disableAttributes(A_REVERSE);
				menuBar.moveCursor(2, 0);
			}
		}
		
		menuBar.setCursor(COLS-5, 0);
		menuBar.print("TextX");
		
		menuBar.refresh();
	}
	
	void onKeyInMenu(curses::KeyCode key) {
		switch (key.value) {
		case KEY_UP: {
			menuItemAt--;
			if (menuItemAt < 0) menuItemAt = currentMenu->items.size()-1;
			refreshMenu();
			break;
		}
		case KEY_DOWN: {
			menuItemAt++;
			if (menuItemAt >= currentMenu->items.size()) menuItemAt = 0;
			refreshMenu();
			break;
		}
		case '\n':
		case KEY_ENTER: {
			MenuItem* item = currentMenu->items[menuItemAt];
			item->onSelected();
			break;
		}
		case KEY_LEFT: {
			selectPrevMenu();
			break;
		}
		case KEY_RIGHT: {
			MenuItem* item = currentMenu->items[menuItemAt];
			if (item->leaf()) {
				selectNextMenu();
			} else {
				item->onSelected();
			}
			break;
		}
		case 27: { // escape
			exitMenu();
			break;
		}
		}
	}
	
	void selectMenu(int index) {
		if (index < 0 || getFocus() == NULL) return;
		MenuBar* bar = getFocus()->getMenuBar();
		if (index >= bar->size()) return;
		selectMenu(&bar->at(index));
	}
	
	static void refreshBackground() {
		vector<Pane*>* panes = getRootPanes();
		for (vector<Pane*>::const_iterator it = panes->begin(); it != panes->end(); it++) {
			(*it)->refresh();
		}
	}
	
	void selectMenu(Menu* menu) {
		if (inMenu) {
			menuWin.dispose();
			refreshBackground();
		} else {
			inMenu = true;
			menuColor = getColorPair(color::white, color::blue);
		}
		
		menuHistory.push_back(menu);
		currentMenu = menu;
		menuItemAt = 0;
		
		int nItems = menu->items.size();
		int maxItemLen = 0;
		for (vector<MenuItem*>::const_iterator it = menu->items.begin(); it != menu->items.end(); it++) {
			int itemLen = (*it)->getMinWidth();
			if (itemLen > maxItemLen) maxItemLen = itemLen;
		}
		
		menuWin = curses::Window(0, 1, maxItemLen+2, nItems+2);
		refreshMenuBar();
		refreshMenu();
	}
	
	void selectPrevMenu() {
		menuHistory.pop_back();
		
		if (menuHistory.empty()) {
			if (getFocus() == NULL) {
				exitMenu();
			} else {
				// try to move left
				MenuBar* bar = getFocus()->getMenuBar();
				int i = 0;
				for (MenuBar::const_iterator it = bar->begin(); it != bar->end(); it++) {
					if (currentMenu == &(*it)) {
						selectMenu(&bar->at(i == 0 ? bar->size()-1 : i-1));
						return;
					}
					i++;
				}
				
				// if we fall through, we're not in a menu bar; we should exit
				exitMenu();
			}
		} else {
			Menu* lastMenu = menuHistory[menuHistory.size()-1];
			menuHistory.pop_back();
			selectMenu(lastMenu);
		}
	}
	
	void selectNextMenu() {
		menuHistory.pop_back();
		
		if (getFocus() == NULL) {
			exitMenu();
		} else {
			// try to move right
			MenuBar* bar = getFocus()->getMenuBar();
			int i = 0;
			for (MenuBar::const_iterator it = bar->begin(); it != bar->end(); it++) {
				if (currentMenu == &(*it)) {
					selectMenu(&bar->at(i == bar->size()-1 ? 0 : i+1));
					return;
				}
				i++;
			}
			
			// if we fall through, do nothing; we're in a submenu
		}
	}
	
	void exitMenu() {
		if (inMenu) {
			menuWin.dispose();
			menuColor.dispose();
		}
		currentMenu = NULL;
		menuHistory.clear();
		inMenu = false;
		
		refreshMenuBar();
		refreshBackground();
	}
	
	// MenuItem
	MenuItem::~MenuItem() {}

	bool MenuItem::leaf() {
		return true;
	}

	int MenuItem::getMinWidth() {throw exception();}
	void MenuItem::refresh(curses::Window window) {throw exception();}
	void MenuItem::onSelected() {throw exception();}
	
	// ButtonMenuItem
	void ButtonMenuItem::refresh(curses::Window window) {
		window.print(text);
	}
	
	void ButtonMenuItem::onSelected() {
		handler();
		exitMenu();
	}
	
	int ButtonMenuItem::getMinWidth() {
		return text.size();
	}
	
	// SubMenuItem
	bool SubMenuItem::leaf() {
		return false;
	}
	
	int SubMenuItem::getMinWidth() {
		return menu.name.size()+2;
	}

	void SubMenuItem::refresh(curses::Window window) {
		window.print(menu.name);
		window.print(" >");
	}

	void SubMenuItem::onSelected() {
		selectMenu(&menu);
	}
}
