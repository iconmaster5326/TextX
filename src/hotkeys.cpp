/*
 * hotkeys.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include "hotkeys.hpp"
#include "textx.hpp"
#include "app.hpp"
#include "pane.hpp"

namespace textx {
	using namespace std;
	
	Hotkey::Hotkey(string name) {
		this->name = name;
	}
	Hotkey::~Hotkey() {}

	void Hotkey::fire() {
		throw "hotkey didn't implement fire()!";
	}

	void HotkeyStore::addHotkey(Hotkey* key) {
		unbound.insert(key);
	}

	void HotkeyStore::bindHotkey(Hotkey* key, int keycode) {
		set<Hotkey*>::const_iterator it = unbound.find(key);
		if (it != unbound.end()) {
			unbound.erase(it);
		}
		
		this->operator [](keycode) = key;
	}

	string keycodeName(int keycode) {
		return "TODO"; // TODO
	}

	Hotkey* getHotkey(int keycode) {
		if (getFocus()) {
			// check if the app has overriden a hotkey
			HotkeyStore::const_iterator it = getFocus()->info->hotkeys.find(keycode);
			if (it != getFocus()->info->hotkeys.end()) {
				return it->second;
			}
			
			// check in the global hotkey table
			it = globalHotkeys.find(keycode);
			if (it != globalHotkeys.end()) {
				return it->second;
			}
			
			// none found
			return NULL;
		}
	}
}
