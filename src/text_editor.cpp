/*
 * text_editor.cpp
 *
 *  Created on: Sep 10, 2018
 *      Author: iconmaster
 */

#include "text_editor.hpp"

#include "textx.hpp"
#include "pane.hpp"
#include "app.hpp"
#include "colors.hpp"

#include <cstdlib>
#include <cctype>
#include <iterator>
#include <iostream>
#include <fstream>

namespace textx {
	using namespace std;
	
	static void menuFileExit() {
		curses::stopCurses();
		exit(0);
	}
	
	static void menuFileOpen() {
		
	}
	
	static void menuFileSave() {
		TextEditorApp* editor = dynamic_cast<TextEditorApp*>(getFocus());
		if (!editor) throw "Menu not for this app";
		editor->saveBuffer();
	}
	
	static void menuFileClose() {
		
	}
	
	void TextEditorApp::init() {
		offset = 0; cursorOffset = 0;
		selectingText = false;
		
		// build file menu
		vector<MenuItem*> fileItems;
		fileItems.push_back(new ButtonMenuItem("Open...", menuFileOpen));
		fileItems.push_back(new ButtonMenuItem("Save", menuFileSave));
		fileItems.push_back(new ButtonMenuItem("Close", menuFileClose));
		fileItems.push_back(new ButtonMenuItem("Exit", menuFileExit));
		menuBar.push_back(Menu("File", fileItems));
	}
	
	TextEditorApp::TextEditorApp(Pane* pane) : App(pane) {
		init();
		
		hasFilename = false;
		unsaved = true;
	};
	
	TextEditorApp::TextEditorApp(Pane* pane, string filename) : App(pane) {
		init();
		
		this->filename = filename;
		hasFilename = true;
		unsaved = false;
		
		// load file into buffer
		ifstream file(filename.c_str());
		if (file.good()) {
			noskipws(file);
			copy(istream_iterator<char>(file), istream_iterator<char>(), back_inserter(buffer));
		} else {
			unsaved = true;
		}
	};
	
	static unsigned charWidth(char c, unsigned col) {
		switch (c) {
		case '\t': {
			int i = col % 4;
			if (i == 0) return 4; else return i;
		}
		case '\r': return 0;
		default: return 1;
		}
	}
	
	static void drawChar(curses::Window win, char c) {
		switch (c) {
		case '\t': {
			int tabSpaces = charWidth('\t', win.cursorX());
			for (int i = 0; i < tabSpaces; i++) win.print(' ');
			break;
		}
		case '\r': return;
		default: {
			if (isprint(c) || isspace(c)) {
				win.print(c);
			} else {
				win.print('?');
			}
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
			if (selectingText && offset >= selBeginOffset) win.enableAttributes(A_REVERSE);
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
				x += charWidth(c, x);
				if (x >= w) {
					x = 0; y++;
				}
			}
			
			if (currentOffset < cursorOffset) {
				cx = x;
				cy = y;
			}
			
			if (!cursorOnly && selectingText && currentOffset == selBeginOffset-1) win.enableAttributes(A_REVERSE);
			if (!cursorOnly && selectingText && currentOffset == selEndOffset-1) win.disableAttributes(A_REVERSE);
			
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
				col+= charWidth(*it, col);
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
				curCol+= charWidth(*it, curCol);
			}
			
			offset++;
		}
		
		// if we fall through, return the last offset possible
		return offset;
	}
	
	void TextEditorApp::saveBuffer() {
		if (hasFilename) {
			if (unsaved) {
				ofstream file(filename.c_str());
				copy(buffer.begin(), buffer.end(), ostream_iterator<char>(file));
				
				unsaved = false;
				getPane()->refreshTitleBar();
			}
		} else {
			// TODO save dialog
		}
	}
	
	void TextEditorApp::markAsUnsaved() {
		unsaved = true;
		getPane()->refreshTitleBar();
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
		case 19: // Control S: save
			saveBuffer();
			refreshCursorOnly = true;
			break;
		case KEY_LEFT: {
			if (selectingText) {
				selectingText = false;
			} else {
				refreshCursorOnly = true;
			}
			
			if (cursorOffset > 0) {
				cursorOffset--;
				while (cursorOffset > 0 && buffer[cursorOffset] == '\r') cursorOffset--;
			}
			break;
		}
		case KEY_RIGHT: {
			if (selectingText) {
				selectingText = false;
			} else {
				refreshCursorOnly = true;
			}
			
			if (cursorOffset < buffer.size()) {
				cursorOffset++;
				while (cursorOffset < buffer.size() && buffer[cursorOffset] == '\r') cursorOffset++;
			};
			break;
		}
		case KEY_UP: {
			unsigned baseLine, dummy; offsetToLine(offset, baseLine, dummy);
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			cursorOffset = lineToOffset(line-1, col);
			
			if (line-baseLine <= 0) {
				offset = lineToOffset(baseLine-1, 0);
			} else {
				refreshCursorOnly = true;
			}
			
			if (selectingText) {
				selectingText = false;
				refreshCursorOnly = false;
			}
			break;
		}
		case KEY_DOWN: {
			unsigned baseLine, dummy; offsetToLine(offset, baseLine, dummy);
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			cursorOffset = lineToOffset(line+1, col);
			
			if (line-baseLine >= win.height()-1) {
				offset = lineToOffset(baseLine+1, 0);
			} else {
				refreshCursorOnly = true;
			}
			
			if (selectingText) {
				selectingText = false;
				refreshCursorOnly = false;
			}
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
			
			markAsUnsaved();
			break;
		}
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
			
			markAsUnsaved();
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
		case KEY_SR: { // shift-up
			unsigned oldCursor = cursorOffset;
			unsigned baseLine, dummy; offsetToLine(offset, baseLine, dummy);
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			cursorOffset = lineToOffset(line-1, col);
			
			if (line-baseLine <= 0) {
				offset = lineToOffset(baseLine-1, 0);
			}
			
			if (selectingText) {
				if (cursorOffset < selBeginOffset) {
					selBeginOffset = cursorOffset;
				} else {
					selEndOffset = cursorOffset;
				}
			} else {
				selectingText = true;
				selBeginOffset = cursorOffset;
				selEndOffset = oldCursor;
			}
			
			break;
		}
		case KEY_SF: { // shift-down
			unsigned oldCursor = cursorOffset;
			unsigned baseLine, dummy; offsetToLine(offset, baseLine, dummy);
			unsigned line, col; offsetToLine(cursorOffset, line, col);
			cursorOffset = lineToOffset(line+1, col);
			
			if (line-baseLine >= win.height()-1) {
				offset = lineToOffset(baseLine+1, 0);
			}
			
			if (selectingText) {
				if (cursorOffset <= selEndOffset) {
					selBeginOffset = cursorOffset;
				} else {
					selEndOffset = cursorOffset;
				}
			} else {
				selectingText = true;
				selBeginOffset = oldCursor;
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
		default: {
			if (selectingText) {
				buffer.erase(buffer.begin()+selBeginOffset, buffer.begin()+selEndOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			}
			
			buffer.insert(buffer.begin()+cursorOffset, key.value);
			cursorOffset++;
			
			if (key.value == '\n') {
				unsigned baseLine, dummy; offsetToLine(offset, baseLine, dummy);
				unsigned line; offsetToLine(cursorOffset, line, dummy);
				
				if (line-baseLine >= win.height()-1) {
					offset = lineToOffset(baseLine+1, 0);
				}
			}
			
			markAsUnsaved();
		}
		}
		
		drawStatusBar(pane, win);
		updateScreen(win, refreshCursorOnly);
	}
	
	string TextEditorApp::getTitle() {
		return (unsaved ? "*" : "") + (hasFilename ? filename : "(untitled)");
	}
	
	MenuBar* TextEditorApp::getMenuBar() {
		return &menuBar;
	}
}
