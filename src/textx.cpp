/*
 * textx.cpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#include <exception>
#include <cstring>
#include <typeinfo>
#include <algorithm>

#include "textx.hpp"
#include "app.hpp"
#include "pane.hpp"
#include "tab_pane.hpp"
#include "menu.hpp"

#if TEXTX_WINDOWS
#include "windows.h"
#endif

namespace textx {
	using namespace std;
	
#if TEXTX_WINDOWS
	static HWND ourHwnd = NULL;
	
	static BOOL findOurHwnd(HWND hwnd, LPARAM ourId) {
		DWORD id = GetWindowThreadProcessId(hwnd, NULL);
		if (id == (DWORD)ourId) {
			ourHwnd = hwnd;
		}
		return TRUE;
	}
	
	static void getOurHwnd() {
		// get console window
		ourHwnd = GetConsoleWindow();
		if (ourHwnd) return;
		// no console window; do we have an active window?
		ourHwnd = GetActiveWindow();
		if (ourHwnd) return;
		// if all else fails, just rummage through all possible windows and see if we own one of them
		BOOL success = EnumWindows(findOurHwnd, GetCurrentProcessId());
		if (!success) throw exception();
		// this thread doesn't have a window; panic
		if (!ourHwnd) throw exception();
	}
	
	string getClipboard() {
		BOOL success = OpenClipboard(NULL);
		if (!success) throw exception();
		
		HANDLE handle = GetClipboardData(CF_TEXT);
		if (!handle) {
			CloseClipboard();
			throw exception();
		}
		
		LPTSTR lptstr = (LPTSTR) GlobalLock(handle);
		if (!lptstr) {
			CloseClipboard();
			throw exception();
		}
		string s((const char*) lptstr);
		
		GlobalUnlock(handle);
		CloseClipboard();
		
		return s;
	}
	
	void setClipboard(string s) {
		if (!ourHwnd) getOurHwnd();
		
		BOOL success = OpenClipboard(ourHwnd);
		if (!success) throw exception();
		
		success = EmptyClipboard();
		if (!success) {
			CloseClipboard();
			throw exception();
		}
		
		HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, s.size()+1);
		LPTSTR lptstr = (LPTSTR) GlobalLock(hglobal); 
		memcpy(lptstr, s.c_str(), s.size()+1);
		GlobalUnlock(hglobal);
		HANDLE handle = SetClipboardData(CF_TEXT, hglobal);
		if (!handle) {
			CloseClipboard();
			throw exception();
		}
		
		CloseClipboard();
	}
#else
	static string clipboard;
	
	string getClipboard() {
		return clipboard;
	}
	
	void setClipboard(string s) {
		clipboard = s;
	}
#endif
	
	static App* focus = NULL;
	static vector<App*> apps;
	static vector<Pane*> rootPanes;
	
	App* getFocus() {
		return focus;
	}
	
	void setFocus(App* app) {
		focus = app;
	}
	
	vector<App*>* getAllApps() {
		return &apps;
	}
	
	vector<Pane*>* getRootPanes() {
		return &rootPanes;
	}
	
	void selectLeftPane() {
		Pane* rawPane = getFocus()->getPane();
		if (typeid(*rawPane) == typeid(TabPane)) {
			TabPane* pane = dynamic_cast<TabPane*>(rawPane);
			list<App*>::const_reverse_iterator it = find(pane->apps.rbegin(), pane->apps.rend(), pane->currentTab);
			it++;
			if (it == pane->apps.rend()) return;
			App* nextApp = *it;
			setFocus(nextApp);
			pane->addApp(nextApp);
		}
	}
	
	void selectRightPane() {
		Pane* rawPane = getFocus()->getPane();
		if (typeid(*rawPane) == typeid(TabPane)) {
			TabPane* pane = dynamic_cast<TabPane*>(rawPane);
			list<App*>::const_iterator it = find(pane->apps.begin(), pane->apps.end(), pane->currentTab);
			it++;
			if (it == pane->apps.end()) return;
			App* nextApp = *it;
			setFocus(nextApp);
			pane->addApp(nextApp);
		}
	}
	
	void onKey(curses::KeyCode key) {
		switch (key.value) {
		case 543: { // alt-left
			selectLeftPane();
			break;
		}
		case 558: { // alt-right
			selectRightPane();
			break;
		}
		case KEY_F(1): selectMenu(0); break;
		case KEY_F(2): selectMenu(1); break;
		case KEY_F(3): selectMenu(2); break;
		case KEY_F(4): selectMenu(3); break;
		case KEY_F(5): selectMenu(4); break;
		case KEY_F(6): selectMenu(5); break;
		case KEY_F(7): selectMenu(6); break;
		case KEY_F(8): selectMenu(7); break;
		case KEY_F(9): selectMenu(8); break;
		case KEY_F(10): selectMenu(9); break;
		case KEY_F(11): selectMenu(10); break;
		case KEY_F(12): selectMenu(11); break;
		default: focus->onKey(key);
		}
	}
	
	void refreshTextX() {
		refreshMenuBar();
		
		vector<Pane*>* panes = getRootPanes();
		for (vector<Pane*>::const_iterator it = panes->begin(); it != panes->end(); it++) {
			(*it)->refresh();
		}
	}
}
