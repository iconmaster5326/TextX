/*
 * text_editor.hpp
 *
 *  Created on: Sep 10, 2018
 *      Author: iconmaster
 */

#ifndef SRC_TEXT_EDITOR_HPP_
#define SRC_TEXT_EDITOR_HPP_

#include <string>
#include <deque>

#include "app.hpp"

namespace textx {
	using namespace std;
	
	typedef deque<char> TEBuffer;
	
	class TextEditorApp : public App {
	private:
		void updateScreen(curses::Window win, bool cursorOnly);
	public:
		TEBuffer buffer;
		unsigned long offset, cursorOffset;
		string filename;
		bool hasFilename;
		bool unsaved;
		
		TextEditorApp(Pane* pane);
		TextEditorApp(Pane* pane, string filename);
		
		void refresh();
		void onKey(curses::KeyCode key);
		string getTitle();
		void close();
	};
}

#endif /* SRC_TEXT_EDITOR_HPP_ */
