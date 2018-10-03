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
#include "app_pane.hpp"
#include "file_dialog.hpp"

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
	
	static App* menuFocus;
	
	static void menuFileOpenHandler(bool ok, string path, void* filePane) {
		delete ((Pane*)filePane);
		
		if (ok) {
			TextEditorApp* app = new TextEditorApp(menuFocus->getPane(), path);
			menuFocus->getPane()->addApp(app);
			
			TextEditorApp* teApp = dynamic_cast<TextEditorApp*>(menuFocus);
			if (teApp && !teApp->hasFilename && teApp->buffer.empty()) {
				teApp->close();
				delete teApp;
			}
			
			setFocus(app);
		} else {
			setFocus(menuFocus);
		}
		
		refreshTextX();
	}
	
	static void menuFileOpen() {
		menuFocus = getFocus();
		
		curses::Window screen = curses::Window(0, 1, COLS, LINES-1);
		AppPane* filePane = new AppPane(screen);
		
		FileDialogApp* fileApp = new FileDialogApp(filePane, "", menuFileOpenHandler, filePane);
		filePane->addApp(fileApp);
		setFocus(fileApp);
		
		refreshMenuBar();
		filePane->refresh();
	}
	
	static void menuFileSave() {
		TextEditorApp* editor = dynamic_cast<TextEditorApp*>(getFocus());
		if (!editor) throw "Menu not for this app";
		editor->saveBuffer();
	}
	
	static void menuFileClose() {
		App* focus = getFocus();
		focus->close();
		delete focus;
	}
	
	class FileTypeMenuItem : public ButtonMenuItem {
	public:
		FileType* ft;
		FileTypeMenuItem(FileType* ft) : ButtonMenuItem(ft->name, NULL), ft(ft) {}
		void onSelected() {
			exitMenu();
			((TextEditorApp*)getFocus())->fileType = ft;
			getFocus()->refresh();
		}
	};
	
	class TextEditorAppInfo : public AppInfo {
	public:
		TextEditorAppInfo() : AppInfo("Text Editor") {
			// add hotkeys
			
			// build menu
			initAllFileTypes();
			vector<MenuItem*> fileTypes;
			for (set<FileType*>::const_iterator it = allFileTypes.begin(); it != allFileTypes.end(); it++) {
				fileTypes.push_back(new FileTypeMenuItem(*it));
			}
			if (fileTypes.empty()) throw "FILE TYPES EMPTY";
			
			//// File
			vector<MenuItem*> fileItems;
			fileItems.push_back(new ButtonMenuItem("Open...", menuFileOpen));
			fileItems.push_back(new ButtonMenuItem("Save", menuFileSave));
			fileItems.push_back(new ButtonMenuItem("Close", menuFileClose));
			fileItems.push_back(new ButtonMenuItem("Exit", menuFileExit));
			menuBar.push_back(Menu("File", fileItems));
			//// View
			vector<MenuItem*> viewItems;
			viewItems.push_back(new SubMenuItem(Menu("File Type", fileTypes)));
			menuBar.push_back(Menu("View", viewItems));
			//// Window
			vector<MenuItem*> windowItems;
			windowItems.push_back(new ButtonMenuItem("Next Tab", selectRightPane));
			windowItems.push_back(new ButtonMenuItem("Previous Tab", selectLeftPane));
			menuBar.push_back(Menu("Window", windowItems));
		}
		App* open(Pane* pane) {
			TextEditorApp* app = new TextEditorApp(pane);
			return app;
		}
	};
	static TextEditorAppInfo appInfo;
	
	TextEditorApp::TextEditorApp(Pane* pane) : App(&appInfo, pane) {
		screenLine = cursorOffset = selBeginOffset = selEndOffset = 0;
		selectingText = false;
		
		hasFilename = false;
		unsaved = true;
		
		fileType = file_type::none;
	};
	
	TextEditorApp::TextEditorApp(Pane* pane, string filename) : App(&appInfo, pane) {
		screenLine = cursorOffset = selBeginOffset = selEndOffset = 0;
		selectingText = false;
		
		this->filename = filename;
		hasFilename = true;
		unsaved = false;
		
		// find file type (if possible)
		fileType = file_type::none;
		for (set<FileType*>::const_iterator it = allFileTypes.begin(); it != allFileTypes.end(); it++) {
			FileType* ft = *it;
			if (ft->isAssociatedFile(filename)) {
				fileType = ft;
				break;
			}
		}
		
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
		int line, col; buffer.offsetToLine(cursorOffset, line, col);
		
		curses::Window status = pane->getStatusBar();
		pane->clearStatusBar();
		
		status.setCursor(0, 0);
		status.printf("Line %d", line+1);
		status.moveCursor(2, 0);
		status.printf("Col %d", col+1);
		
		status.setCursor(status.width() - fileType->name.size(), 0);
		status.print(fileType->name);
		
		status.refresh();
	}
	
	void TextEditorApp::updateScreen(curses::Window win, bool cursorOnly) {
		int w, h; win.getSize(w, h);
		
		// find size of the gutter
		int xMin = 1;
		
		Buffer::line_t line = screenLine;
		Buffer::line_t maxLineNo = screenLine+h;
		do {
			maxLineNo /= 10;
			xMin++; 
		} while (maxLineNo);
		
		Buffer::offset_t offset = buffer.lineToOffset(screenLine);
		
		if (!cursorOnly) {
			win.clear();
			
			// draw gutter
			win.setCursor(0, 0);
			win.setAttributes(A_BOLD);
			getColorPair(color::yellow, color::system).use(win);
			
			for (int y = 0; y < h; y++) {
				if (line < buffer.lines()) {
					win.setCursor(0, y);
					win.printf("%d", line+1);
				}
				win.setCursor(xMin-1, y);
				win.print(ACS_VLINE);
				
				int len = buffer.lineLengthAtLine(line);
				while (len > w-xMin) {
					len -= w-xMin;
					y++;
					win.setCursor(xMin-1, y);
					win.print(ACS_VLINE);
				}
				
				line++;
			}
			
			win.setAttributes(0);
			color::pair::system.use(win);
			
			// initialize state for drawing pane
			win.setCursor(xMin, 0);
			if (selectingText && offset >= selBeginOffset) win.enableAttributes(A_REVERSE);
		}
		
		int x = xMin, y = 0;
		unsigned currentOffset = offset;
		string::const_iterator it = buffer.asString().begin()+offset;
		int cx = xMin, cy = 0;
		
		Token token = Token(0, color::pair::system, 0);
		
		while (y < h && it != buffer.asString().end()) {
			if (!cursorOnly && token.length <= 0) {
				token = fileType->getToken(buffer, currentOffset);
				win.setAttributes(token.attribues);
				token.color.use(win);
				if (token.length <= 0) token.length = LINES * COLS;
			}
			token.length--;
			
			if (!cursorOnly && selectingText && currentOffset >= selBeginOffset) win.enableAttributes(A_REVERSE);
			if (!cursorOnly && selectingText && currentOffset >= selEndOffset) win.disableAttributes(A_REVERSE);
			
			char c = *it;
			switch (c) {
			case '\n':
				x = xMin; y++;
				if (!cursorOnly && y < h) {
					win.print('\n');
					win.moveCursor(xMin, 0);
				}
				break;
			default:
				if (!cursorOnly) drawChar(win, c);
				x += charWidth(c, x);
				if (x >= w) {
					x = xMin; y++;
					if (!cursorOnly) win.moveCursor(xMin, 0);
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
	
	void TextEditorApp::saveBuffer() {
		if (hasFilename) {
			if (unsaved) {
				ofstream file(filename.c_str());
				copy(buffer.asString().begin(), buffer.asString().end(), ostream_iterator<char>(file));
				
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
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			cursorOffset = buffer.lineToOffset(line-1, col);
			
			if (line-screenLine <= 0) {
				screenLine--;
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
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			cursorOffset = buffer.lineToOffset(line+1, col);
			
			if (line-screenLine >= win.height()-1) {
				screenLine++;
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
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			} else {
				if (cursorOffset >= buffer.size()) break;
				buffer.erase(cursorOffset);
			}
			
			markAsUnsaved();
			break;
		}
		case 127:
		case KEY_BACKSPACE: {
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			} else {
				if (cursorOffset == 0) break;
				buffer.erase(cursorOffset-1);
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
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			cursorOffset = buffer.lineToOffset(line-1, col);
			
			if (line-screenLine <= 0) {
				screenLine--;
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
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			cursorOffset = buffer.lineToOffset(line+1, col);
			
			if (line-screenLine >= win.height()-1) {
				screenLine++;
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
			setClipboard(buffer.asString().substr(selBeginOffset, selEndOffset-selBeginOffset));
			
			buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
			cursorOffset = selBeginOffset;
			selectingText = false;
			
			break;
		}
		case 3: { // ^C: copy
			if (!selectingText) return;
			setClipboard(buffer.asString().substr(selBeginOffset, selEndOffset-selBeginOffset));
			
			break;
		}
		case 22: { // ^V: paste
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			}
			
			string clip = getClipboard();
			buffer.insert(cursorOffset, clip.begin(), clip.end());
			cursorOffset += clip.size();
			
			break;
		}
		case 9: { // ^I: tab
			if (selectingText) {
				// indent all lines in selection
				Buffer::line_t beginLine = buffer.offsetToLine(selBeginOffset);
				Buffer::line_t endLine = buffer.offsetToLine(selEndOffset);
				
				for (int i = beginLine; i <= endLine; i++) {
					Buffer::offset_t offset = buffer.lineToOffset(i);
					buffer.insert(offset, '\t');
				}
				
				selBeginOffset++; selEndOffset++;
				selEndOffset += endLine-beginLine;
				cursorOffset = selBeginOffset;
			} else {
				// insert literal tab
				buffer.insert(cursorOffset, '\t');
				cursorOffset++;
			}
			
			markAsUnsaved();
			break;
		}
		case KEY_BTAB: {
			// indent all lines in selection
			Buffer::line_t beginLine;
			Buffer::line_t endLine;
			
			if (selectingText) {
				beginLine = buffer.offsetToLine(selBeginOffset);
				endLine = buffer.offsetToLine(selEndOffset);
			} else {
				beginLine = endLine = buffer.offsetToLine(cursorOffset);
			}
			
			int tabsRemoved = 0;
			for (int i = beginLine; i <= endLine; i++) {
				Buffer::offset_t offset = buffer.lineToOffset(i);
				if (buffer[offset] == '\t') {
					buffer.erase(offset);
					tabsRemoved++;
					
					if (selectingText) {
						if (offset >= selBeginOffset) {
							selBeginOffset--;
							selEndOffset--;
						} else if (offset <= selEndOffset) {
							selEndOffset--;
						}
					} else {
						cursorOffset--;
					}
				}
			}
			
			if (selectingText) {
				if (tabsRemoved != 0) {
					selBeginOffset += (endLine-beginLine)-1;
				}
				cursorOffset = selBeginOffset;
			}
			break;
		}
		default: {
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				cursorOffset = selBeginOffset;
				selectingText = false;
			}
			
			buffer.insert(cursorOffset, key.value);
			cursorOffset++;
			
			if (key.value == '\n') {
				int line, dummy; buffer.offsetToLine(cursorOffset, line, dummy);
				
				if (line-screenLine >= win.height()-1) {
					screenLine++;
				}
			}
			
			markAsUnsaved();
		}
		}
		
		drawStatusBar(pane, win);
		updateScreen(win, refreshCursorOnly);
	}
	
	string TextEditorApp::getTitle() {
		if (hasFilename) {
			string basename = filename;
			string::size_type lastSlash = basename.find_last_of("\\/");
			if (lastSlash != string::npos) {
				basename.erase(0, lastSlash+1);
			}
			
			return (unsaved ? "*" : "") + basename;
		} else {
			return "*(untitled)";
		}
	}
	
	void TextEditorApp::onMouse(curses::Window win, curses::MouseEvent mevent) {
		int x, y; win.getPosition(x, y);
		x = mevent.x() - x;
		y = mevent.y() - y;
		
		if (win.raw == getPane()->getStatusBar().raw) {
			// status bar click
			
		} else {
			// content click
			int w, h; win.getSize(w, h);
			
			// find size of the gutter
			int xMin = 1;
			
			int maxLineNo = screenLine+h;
			do {
				maxLineNo /= 10;
				xMin++; 
			} while (maxLineNo);
			
			// do mouse event
			if (mevent.click(1)) {
				// left click; move cursor
				cursorOffset = buffer.lineToOffset(screenLine+y, x-xMin);
				
				if (selectingText) {
					selectingText = false;
					updateScreen(win, false);
				} else {
					updateScreen(win, true);
				}
			} else if (mevent.down(1)) {
				// left button down; start drag
				cursorOffset = buffer.lineToOffset(screenLine+y, x-xMin);
				selBeginOffset = selEndOffset = cursorOffset;
				
				if (selectingText) {
					updateScreen(win, false);
				} else {
					selectingText = true;
					updateScreen(win, true);
				}
			} else if (mevent.up(1)) {
				// left button up; finish drag
				if (!selectingText) return;
				
				cursorOffset = buffer.lineToOffset(screenLine+y, x-xMin);
				selEndOffset = cursorOffset;
				
				if (selEndOffset < selBeginOffset) {
					swap(selEndOffset, selBeginOffset);
				}
				
				updateScreen(win, false);
			}
		}
	}
}
