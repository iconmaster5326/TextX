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
#include "buffer.hpp"

namespace textx {
	using namespace std;
	
	enum EolMode {
		EOL_MODE_WINDOWS,
		EOL_MODE_UNIX,
		EOL_MODE_MACOS,
	};
	
	class TextEditorApp : public App {
	private:
		void init();
		int getGutterWidth(curses::Window win);
		int getMaxLineWidth(curses::Window win);
		int getLineHeight(curses::Window win, Buffer::line_t line);
		int offsetToSubline(curses::Window win, Buffer::offset_t offset);
		Buffer::offset_t minScreenOffset(curses::Window win);
		Buffer::offset_t maxScreenOffset(curses::Window win);
		void moveCursor(curses::Window win, Buffer::offset_t offset);
	public:
		Buffer buffer;
		string filename;
		Buffer::line_t screenLine;
		int screenSubline;
		Buffer::offset_t cursorOffset, selBeginOffset, selEndOffset;
		bool hasFilename, unsaved, selectingText;
		FileType* fileType;
		EolMode eolMode;
		
		TextEditorApp(Pane* pane);
		TextEditorApp(Pane* pane, string filename);
		
		// overrides
		void refresh();
		void onKey(curses::KeyCode key);
		string getTitle();
		void onMouse(curses::Window window, curses::MouseEvent mevent);
		
		// new methods
		void drawStatusBar(Pane* pane, curses::Window win);
		void updateScreen(curses::Window win, bool cursorOnly);
		void saveBuffer();
		void markAsUnsaved();
	};
}

#endif /* SRC_TEXT_EDITOR_HPP_ */
