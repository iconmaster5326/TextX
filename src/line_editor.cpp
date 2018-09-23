/*
 * line_editor.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include "line_editor.hpp"

namespace textx {
	using namespace std;
	
	void LineEditor::onKey(curses::KeyCode key) {
		if (done) throw "Already done";
		
		switch (key.value) {
		case '\n':
			done = true;
			break;
		default:
			buffer.insert(buffer.begin()+cursorOffset, key.value);
			cursorOffset++;
			refresh(); win.refresh();
		}
	}
	
	void LineEditor::refresh() {
		win.setCursor(x, y);
		win.print(buffer.substr(offset, w));
		if (selectingText && selBeginOffset < offset+w && selEndOffset < offset) {
			win.setCursor(selBeginOffset <= offset ? x : x+(selBeginOffset-offset), y);
			win.setAttributesAt(A_REVERSE, (selEndOffset-selBeginOffset >= w) ? w : selEndOffset-selBeginOffset);
		}
		win.setCursor(x+(cursorOffset-offset), y);
	}
}
