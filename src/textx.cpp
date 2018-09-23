/*
 * textx.cpp
 *
 *  Created on: Sep 9, 2018
 *      Author: iconmaster
 */

#include <exception>
#include <cstring>

#include "textx.hpp"
#include "app.hpp"
#include "pane.hpp"

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
		
		HGLOBAL hglobal = GlobalAlloc(GMEM_MOVEABLE, s.size());
		LPTSTR lptstr = (LPTSTR) GlobalLock(hglobal); 
		memcpy(lptstr, s.c_str(), s.size());
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
	
	static App* focus;
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
	
	void onKey(curses::KeyCode key) {
		focus->onKey(key);
	}
}
