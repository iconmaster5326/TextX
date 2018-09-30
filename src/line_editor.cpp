/*
 * line_editor.cpp
 *
 *  Created on: Sep 23, 2018
 *      Author: iconmaster
 */

#include "line_editor.hpp"
#include "textx.hpp"

namespace textx {
	using namespace std;
	
	void LineEditor::onKey(curses::KeyCode key) {
		if (done) throw "Already done";
		
		switch (key.value) {
		case '\n':
			done = true;
			break;
		case 127:
		case KEY_BACKSPACE: {
			if (selectingText) {
				buffer.erase(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			} else {
				if (cursorOffset == 0) break;
				buffer.erase(buffer.begin()+cursorOffset-1);
				cursorOffset--;
			}
			
			refresh(); win.refresh();
			break;
		}
		case KEY_DC: {
			if (selectingText) {
				buffer.erase(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			} else {
				if (cursorOffset >= buffer.size()) break;
				buffer.erase(buffer.begin()+cursorOffset);
			}
			
			refresh(); win.refresh();
			break;
		}
		case KEY_LEFT: {
			if (selectingText) {
				selectingText = false;
			}
			
			if (cursorOffset == 0) break;
			
			cursorOffset--;
			refresh(); win.refresh();
			break;
		}
		case KEY_RIGHT: {
			if (selectingText) {
				selectingText = false;
			}
			
			if (cursorOffset >= buffer.size()) break;
			
			cursorOffset++;
			refresh(); win.refresh();
			break;
		}
		case KEY_SLEFT: {
			if (cursorOffset == 0) break;
			cursorOffset--;
			
			if (selectingText) {
				if (cursorOffset < selBeginOffset) {
					selBeginOffset--;
				} else {
					selEndOffset--;
				}
			} else {
				selectingText = true;
				selBeginOffset = cursorOffset;
				selEndOffset = cursorOffset+1;
			}
			break;
		}
		case KEY_SRIGHT: {
			if (cursorOffset >= buffer.size()) break;
			cursorOffset++;
			
			if (selectingText) {
				if (cursorOffset <= selEndOffset) {
					selBeginOffset++;
				} else {
					selEndOffset++;
				}
			} else {
				selectingText = true;
				selBeginOffset = cursorOffset-1;
				selEndOffset = cursorOffset;
			}
			break;
		}
		case 24: { // ^X: cut
			if (!selectingText) return;
			setClipboard(string(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset));
			
			buffer.erase(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset);
			cursorOffset = selBeginOffset;
			selectingText = false;
			
			break;
		}
		case 3: { // ^C: copy
			if (!selectingText) return;
			setClipboard(string(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset));
			
			break;
		}
		case 22: { // ^V: paste
			if (selectingText) {
				buffer.erase(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			}
			
			string clip = getClipboard();
			buffer.insert(buffer.begin()+cursorOffset, clip.begin(), clip.end());
			cursorOffset += clip.size();
			
			break;
		}
		default:
			buffer.insert(buffer.begin()+cursorOffset, key.value);
			cursorOffset++;
			refresh(); win.refresh();
		}
	}
	
	void LineEditor::refresh() {
		win.setCursor(x, y);
		win.drawHLine(' ', w);
		win.print(buffer.substr(offset, w));
		if (selectingText && selBeginOffset < offset+w && selEndOffset > offset) {
			win.setCursor(selBeginOffset <= offset ? x : x+(selBeginOffset-offset), y);
			win.setAttributesAt(A_REVERSE, (selEndOffset-selBeginOffset >= w) ? w : selEndOffset-selBeginOffset);
		}
		win.setCursor(x+(cursorOffset-offset), y);
	}
}
