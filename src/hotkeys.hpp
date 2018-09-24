/*
 * hotkeys.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#ifndef SRC_HOTKEYS_HPP_
#define SRC_HOTKEYS_HPP_

#include <map>
#include <set>
#include <string>

namespace textx {
	using namespace std;
	
	class App;
	
	class Hotkey {
	public:
		string name;
		
		Hotkey(string name);
		virtual ~Hotkey();
		virtual void fire();
	};
	
	class HotkeyStore : public map<int,Hotkey*> {
	public:
		set<Hotkey*> unbound;
		
		void addHotkey(Hotkey* key);
		void bindHotkey(Hotkey* key, int keycode);
		
		// TODO implement when needed
		// set<Hotkey> allHotkeys();
		// bool isBound(Hotkey* hotkey);
		// int getBoundTo(Hotkey* hotkey);
	};
	
	string keycodeName(int keycode);
	Hotkey* getHotkey(int keycode);
}

#endif /* SRC_HOTKEYS_HPP_ */
