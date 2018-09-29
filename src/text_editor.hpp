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
#include <map>

#include "app.hpp"
#include "menu.hpp"
#include "file_types.hpp"

namespace textx {
	using namespace std;
	
	class TextEditorApp : public App {
	private:
		void init();
	public:
		string buffer;
		string filename;
		unsigned offset, cursorOffset, selBeginOffset, selEndOffset;
		bool hasFilename, unsaved, selectingText;
		FileType* fileType;
		
		TextEditorApp(Pane* pane);
		TextEditorApp(Pane* pane, string filename);
		
		// overrides
		void refresh();
		void onKey(curses::KeyCode key);
		string getTitle();
		
		// new methods
		void drawStatusBar(Pane* pane, curses::Window win);
		void updateScreen(curses::Window win, bool cursorOnly);
		void offsetToLine(unsigned offset, unsigned& line, unsigned& col);
		int lineToOffset(unsigned line, unsigned col);
		void saveBuffer();
		void markAsUnsaved();
	};
}

#endif /* SRC_TEXT_EDITOR_HPP_ */
