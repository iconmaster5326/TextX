/*
 * text_editor.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: iconmaster
 */

#include "text_editor.hpp"

#include "pane.hpp"
#include "app.hpp"
#include "colors.hpp"

#include <cstdlib>
#include <cctype>

namespace textx {
	using namespace std;
	
	TextEditorApp::TextEditorApp(Pane* pane) : App(pane) {
		offset = 0; cursorOffset = 0;
		hasFilename = false;
		unsaved = true;
	};
	
	TextEditorApp::TextEditorApp(Pane* pane, string filename) : App(pane) {
		offset = 0; cursorOffset = 0;
		this->filename = filename;
		hasFilename = true;
		unsaved = false;
		// TODO load file into editor
	};
	
	static void drawChar(curses::Window win, char c) {
		switch (c) {
		case '\n':
			win.print(c);
			break;
		default:
			if (isprint(c)) {
				win.print(c);
			} else {
				win.print('?');
			}
		}
	}
	
	void TextEditorApp::drawStatusBar(Pane* pane, curses::Window win) {
		unsigned line, col; offsetToLine(cursorOffset, line, col);
		
		curses::Window status = pane->getStatusBar();
		pane->clearStatusBar();
		
		status.setCursor(0, 0);
		status.printf("Line %d", line+1);
		status.moveCursor(2, 0);
		status.printf("Col %d", col+1);
		
		status.refresh();
	}
	
	void TextEditorApp::updateScreen(curses::Window win, bool cursorOnly) {
		if (!cursorOnly) {
			win.clear();
			win.setCursor(0, 0);
		}
		
		int x = 0, y = 0;
		int w, h; win.getSize(w, h);
		unsigned currentOffset = offset;
		TEBuffer::const_iterator it = buffer.begin()+offset;
		int cx = 0, cy = 0;
		
		while (y < h && it != buffer.end()) {
			char c = *it;
			switch (c) {
			case '\n':
				x = 0; y++;
				if (!cursorOnly && y < h) win.print('\n');
				break;
			default:
				if (!cursorOnly) drawChar(win, c);
				x++;
				if (x >= w) {
					x = 0; y++;
				}
			}
			
			if (currentOffset < cursorOffset) {
				cx = x;
				cy = y;
			}
			
			it++; currentOffset++;
		}
		
		win.setCursor(cx, cy);
		win.refresh();
	}
	
	void TextEditorApp::offsetToLine(unsigned offset, unsigned& line, unsigned& col) {
		col = 0; line = 0;
		if (offset <= 0) return;
		
		unsigned currentOffset = 0;
		for (TEBuffer::const_iterator it = buffer.begin(); it != buffer.end(); it++) {
			if (*it == '\n') {
				col = 0;
				line++;
			} else {
				col++;
			}
			
			currentOffset++;
			if (currentOffset == offset) return;
		}
		
		// if we fall through, line and col are set to the end of the buffer
	}
	
	int TextEditorApp::lineToOffset(unsigned line, unsigned col) {
		unsigned offset = 0, curLine = 0, curCol = 0;
		
		for (TEBuffer::const_iterator it = buffer.begin(); it != buffer.end(); it++) {
			if (line == curLine && col == curCol) return offset;
			
			if (*it == '\n') {
				if (line == curLine) return offset;
				curLine++;
				curCol = 0;
			} else {
				curCol++;
			}
			
			offset++;
		}
		
		// if we fall through, return the last offset possible
		return offset;
	}
	
	void TextEditorApp::refresh() {
		curses::Window win = getPane()->getContent();
		color::pair::system.use(win);
		
		drawStatusBar(getPane(), win);
		
		updateScreen(win, false);
	}
	
	void TextEditorApp::onKey(curses::KeyCode key) {
		Pane* pane = getPane();
		curses::Window win = pane->getContent();
		bool refreshCursorOnly = false;
		switch (key.value) {
		case KEY_LEFT: {
			refreshCursorOnly = true;
			if (cursorOffset > 0) cursorOffset--;
			break;
		}
		case KEY_RIGHT: {
			refreshCursorOnly = true;
			if (cursorOffset < buffer.size()) cursorOffset++;
			break;
		}
		case KEY_UP: {
			refreshCursorOnly = true;
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			cursorOffset = lineToOffset(line-1, col);
			break;
		}
		case KEY_DOWN: {
			refreshCursorOnly = true;
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			cursorOffset = lineToOffset(line+1, col);
			break;
		}
		default: {
			buffer.insert(buffer.begin()+cursorOffset, key.value);
			cursorOffset++;
		}
		}
		
		drawStatusBar(pane, win);
		updateScreen(win, refreshCursorOnly);
	}
	
	string TextEditorApp::getTitle() {
		return (unsaved ? "*" : "") + (hasFilename ? filename : "(untitled)");
	}
	
	void TextEditorApp::close() {
		
	}
}
