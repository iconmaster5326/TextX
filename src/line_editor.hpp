/*
 * line_editor.hpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#ifndef SRC_LINE_EDITOR_HPP_
#define SRC_LINE_EDITOR_HPP_

#include "curses.hpp"

namespace textx {
	using namespace std;
	
	class LineEditor {
	public:
		string buffer;
		unsigned offset, cursorOffset, selBeginOffset, selEndOffset;
		bool selectingText, done;
		curses::Window win;
		int x, y, w;
		
		inline LineEditor() {
			offset = cursorOffset = selBeginOffset = selEndOffset = 0;
			selectingText = done = false;
		}
		inline LineEditor(curses::Window win, int x, int y, int w) {
			this->win = win; this->x = x; this->y = y; this->w = w;
			offset = cursorOffset = selBeginOffset = selEndOffset = 0;
			selectingText = done = false;
		}
		inline LineEditor(curses::Window win, int x, int y, int w, string buffer) {
			this->win = win; this->x = x; this->y = y; this->w = w;
			this->buffer = buffer;
			offset = cursorOffset = selBeginOffset = selEndOffset = 0;
			selectingText = done = false;
		}
		
		void onKey(curses::KeyCode key);
		void refresh();
	};
}

#endif /* SRC_LINE_EDITOR_HPP_ */
