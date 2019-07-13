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
#include <limits>

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
	
	static string eolModeToString(EolMode mode) {
		switch (mode) {
			case EOL_MODE_WINDOWS: return "Windows (\\r\\n)"; break;
			case EOL_MODE_UNIX: return "Unix (\\n)"; break;
			case EOL_MODE_MACOS: return "MacOS (\\r)"; break;
			default: throw exception();
		}
	}
	
	class EolModeMenuItem : public ButtonMenuItem {
	public:
		EolMode mode;
		EolModeMenuItem(EolMode mode) : ButtonMenuItem(eolModeToString(mode), NULL), mode(mode) {}
		void onSelected() {
			exitMenu();
			if (((TextEditorApp*)getFocus())->eolMode != mode) {
				((TextEditorApp*)getFocus())->eolMode = mode;
				((TextEditorApp*)getFocus())->unsaved = true;
				getFocus()->getPane()->refreshTitleBar();
				getFocus()->getPane()->refresh();
			}
		}
	};
	
	class TextEditorAppInfo : public AppInfo {
	public:
		TextEditorAppInfo() : AppInfo("Text Editor") {
			// add hotkeys
			
			// build submenus
			//// file types
			initAllFileTypes();
			vector<MenuItem*> fileTypes;
			for (set<FileType*>::const_iterator it = allFileTypes.begin(); it != allFileTypes.end(); it++) {
				fileTypes.push_back(new FileTypeMenuItem(*it));
			}
			if (fileTypes.empty()) throw "FILE TYPES EMPTY";
			//// eol modes
			vector<MenuItem*> eolModes;
			eolModes.push_back(new EolModeMenuItem(EOL_MODE_WINDOWS));
			eolModes.push_back(new EolModeMenuItem(EOL_MODE_UNIX));
			eolModes.push_back(new EolModeMenuItem(EOL_MODE_MACOS));
			
			// build menu
			//// File
			vector<MenuItem*> fileItems;
			fileItems.push_back(new ButtonMenuItem("Open...", menuFileOpen));
			fileItems.push_back(new ButtonMenuItem("Save", menuFileSave));
			fileItems.push_back(new ButtonMenuItem("Close", menuFileClose));
			fileItems.push_back(new ButtonMenuItem("Exit", menuFileExit));
			menuBar.push_back(Menu("File", fileItems));
			//// Edit
			vector<MenuItem*> editItems;
			editItems.push_back(new SubMenuItem(Menu("EOL Mode", eolModes)));
			menuBar.push_back(Menu("Edit", editItems));
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
	
#ifdef TEXTX_WINDOWS
		static const EolMode defaultEolMode = EOL_MODE_WINDOWS;
#else
		static const EolMode defaultEolMode = EOL_MODE_UNIX;
#endif
	
	TextEditorApp::TextEditorApp(Pane* pane) : App(&appInfo, pane) {
		screenLine = screenSubline = cursorOffset = selBeginOffset = selEndOffset = 0;
		selectingText = false;
		
		hasFilename = false;
		unsaved = true;
		
		fileType = file_type::none;
		eolMode = defaultEolMode;
	};
	
	TextEditorApp::TextEditorApp(Pane* pane, string filename) : App(&appInfo, pane) {
		screenLine = screenSubline = cursorOffset = selBeginOffset = selEndOffset = 0;
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
		
		// attempt to detect EOL mode
		eolMode = defaultEolMode;
		for (string::const_iterator it = buffer.asString().begin(); it != buffer.asString().end(); it++) {
			if (*it == '\r') {
				it++;
				if (it != buffer.asString().end() && *it == '\n') {
					eolMode = EOL_MODE_WINDOWS;
					
				    string::size_type pos = 0;
				    while ((pos = buffer.asString().find("\r\n", pos)) != string::npos) {
				        buffer.erase(pos);
				    }
				    
					break;
				} else {
					eolMode = EOL_MODE_MACOS;
					
					string::size_type pos = 0;
					while ((pos = buffer.asString().find('\r', pos)) != string::npos) {
						buffer.erase(pos);
						buffer.insert(pos, '\n');
					}
					
					break;
				}
			} else if (*it == '\n') {
				eolMode = EOL_MODE_UNIX;
				break;
			}
		}
	};
	
	static unsigned charWidth(char c, unsigned col) {
		switch (c) {
		case '\t': {
			int i = col % 4;
			if (i == 0) return 4; else return i;
		}
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
		
		status.moveCursor(2, 0);
		switch (eolMode) {
		case EOL_MODE_WINDOWS: status.print("Win"); break;
		case EOL_MODE_UNIX: status.print("Unix"); break;
		case EOL_MODE_MACOS: status.print("Mac"); break;
		}
		
		status.setCursor(status.width() - fileType->name.size(), 0);
		status.print(fileType->name);
		
		status.refresh();
	}

	int TextEditorApp::getGutterWidth(curses::Window win) {
		int w, h; win.getSize(w, h);
		int xMin = 1;
		Buffer::line_t maxLineNo = screenLine+h;
		do {
			maxLineNo /= 10;
			xMin++; 
		} while (maxLineNo);
		return xMin;
	}

	int TextEditorApp::getMaxLineWidth(curses::Window win) {
		int w, h; win.getSize(w, h);
		int gutterWidth = getGutterWidth(win);
		return w - gutterWidth;
	}
	
	int TextEditorApp::getLineHeight(curses::Window win, Buffer::line_t line) {
		int rowWidth = getMaxLineWidth(win);
		int lineWidth = buffer.lineLengthAtLine(line);
		return (lineWidth / rowWidth) + 1;
	}

	int TextEditorApp::offsetToSubline(curses::Window win, Buffer::offset_t offset) {
		int rowWidth = getMaxLineWidth(win);
		Buffer::col_t col = buffer.offsetToCol(offset);
		return col / rowWidth;
	}

	Buffer::offset_t TextEditorApp::minScreenOffset(curses::Window win) {
		return buffer.lineToOffset(screenLine) + screenSubline * getMaxLineWidth(win);
	}

	Buffer::offset_t TextEditorApp::maxScreenOffset(curses::Window win) {
		int w, h; win.getSize(w, h);
		int rowWidth = getMaxLineWidth(win);

		int line = screenLine;
		int subline = screenSubline;
		int y = 0;
		Buffer::offset_t offset = minScreenOffset(win);

		while (y < h) {
			int lineHeight = getLineHeight(win, line);
			int lineWidth = buffer.lineLengthAtOffset(buffer.lineToOffset(line) + subline * rowWidth);
			if (lineWidth > rowWidth) lineWidth = rowWidth;

			if (subline < lineHeight) {
				subline++;
			}
			if (subline == lineHeight) {
				subline = 0;
				line++;
				offset++;
			}

			offset += lineWidth;
			y++;
		}

		return offset;
	}

	void TextEditorApp::moveCursor(curses::Window win, Buffer::offset_t offset) {
		cursorOffset = offset;

		// move screen up if needed
		Buffer::offset_t begin = minScreenOffset(win);
		if (offset < begin) {
			screenLine = buffer.offsetToLine(offset);
			screenSubline = buffer.offsetToCol(offset) / getMaxLineWidth(win);
			return;
		}

		// move screen down if needed
		Buffer::offset_t end = maxScreenOffset(win);
		while (offset >= end) {
			int lineHeight = getLineHeight(win, screenLine);

			if (screenSubline < lineHeight) {
				screenSubline++;
			}
			if (screenSubline == lineHeight) {
				screenSubline = 0;
				screenLine++;
			}

			end = maxScreenOffset(win);
		}
	}
	
	void TextEditorApp::updateScreen(curses::Window win, bool cursorOnly) {
		win.setScrollable(false);
		int w, h; win.getSize(w, h);
		int xMin = getGutterWidth(win);
		
		Buffer::line_t line = screenLine;
		Buffer::offset_t offset = minScreenOffset(win);
		
		if (!cursorOnly) {
			win.clear();
			
			// draw gutter
			win.setCursor(0, 0);
			win.setAttributes(A_BOLD);
			getColorPair(color::yellow, color::system).use(win);

			int gutterY = -screenSubline;
			for (int line = screenLine; gutterY < h; line++) {
				int lineHeight = getLineHeight(win, line);
				for (int i = 0; i < lineHeight; i++) {
					if (i == 0 && line < buffer.lines()) {
						win.setCursor(0, gutterY);
						win.printf("%d", line+1);
					}
					win.setCursor(xMin-1, gutterY);
					win.print(ACS_VLINE);
					gutterY++;
				}
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
					win.setCursor(x, y);
				}
				break;
			default:
				if (!cursorOnly) drawChar(win, c);
				x += charWidth(c, x);
				if (x >= w) {
					x = xMin; y++;
					if (!cursorOnly && y < h) win.setCursor(x, y);
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
				
				switch (eolMode) {
				case EOL_MODE_UNIX:
					copy(buffer.asString().begin(), buffer.asString().end(), ostream_iterator<char>(file));
					break;
				case EOL_MODE_WINDOWS:
					for (string::const_iterator it = buffer.asString().begin(); it != buffer.asString().end(); it++) {
						if (*it == '\n') {
							file << "\r\n";
						} else {
							file << *it;
						}
					}
					break;
				case EOL_MODE_MACOS:
					for (string::const_iterator it = buffer.asString().begin(); it != buffer.asString().end(); it++) {
						if (*it == '\n') {
							file << '\r';
						} else {
							file << *it;
						}
					}
					break;
				}
				
				file.flush();
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
			}
			
			if (cursorOffset > 0) {
				moveCursor(win, cursorOffset-1);
			}
			break;
		}
		case KEY_RIGHT: {
			if (selectingText) {
				selectingText = false;
			}
			
			if (cursorOffset < buffer.size()) {
				moveCursor(win, cursorOffset+1);
			};
			break;
		}
		case KEY_UP: {
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			if (line == 0) break;
			moveCursor(win, buffer.lineToOffset(line-1, col));
			
			if (selectingText) {
				selectingText = false;
				refreshCursorOnly = false;
			}
			break;
		}
		case KEY_DOWN: {
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			moveCursor(win, buffer.lineToOffset(line+1, col));
			
			if (selectingText) {
				selectingText = false;
				refreshCursorOnly = false;
			}
			break;
		}
		case KEY_DC: {
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				moveCursor(win, selBeginOffset);
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
				moveCursor(win, selBeginOffset);
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
			moveCursor(win, cursorOffset-1);
			
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
			moveCursor(win, cursorOffset+1);
			
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
			moveCursor(win, buffer.lineToOffset(line-1, col));
			
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
			moveCursor(win, buffer.lineToOffset(line+1, col));
			
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
			moveCursor(win, selBeginOffset);
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
				moveCursor(win, selBeginOffset);
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
				moveCursor(win, selBeginOffset);
			} else {
				// insert literal tab
				buffer.insert(cursorOffset, '\t');
				moveCursor(win, cursorOffset+1);
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
						moveCursor(win, cursorOffset-1);
					}
				}
			}
			
			if (selectingText) {
				if (tabsRemoved != 0) {
					selBeginOffset += (endLine-beginLine)-1;
				}
				moveCursor(win, selBeginOffset);
			}
			break;
		}
		case KEY_HOME: {
			int line = buffer.offsetToLine(cursorOffset);
			moveCursor(win, buffer.lineToOffset(line));
		} break;
		case KEY_END: {
			int line = buffer.offsetToLine(cursorOffset);
			moveCursor(win, buffer.lineToOffset(line, numeric_limits<Buffer::col_t>::max()));
		} break;
		case KEY_PPAGE: { // page up
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			line -= win.height();
			if (line < 0) {
				line = 0;
				col = 0;
			}
			moveCursor(win, buffer.lineToOffset(line, col));
		} break;
		case KEY_NPAGE: { // page down
			int line, col; buffer.offsetToLine(cursorOffset, line, col);
			line += win.height();
			moveCursor(win, buffer.lineToOffset(line, col));
		} break;
		case '\n':
		case KEY_ENTER: {
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				moveCursor(win, selBeginOffset);
				selectingText = false;
			}
			
			buffer.insert(cursorOffset, '\n');
			moveCursor(win, cursorOffset+1);
			
			// scroll screen if needed
			int line = buffer.offsetToLine(cursorOffset);
			if (line-screenLine >= win.height()-1) {
				screenLine++;
			}
			
			// auto-indent
			int lastLine = buffer.lineToOffset(line-1);
			while (lastLine > 0 && lastLine < buffer.size() && buffer[lastLine] != '\n' && isspace(buffer[lastLine])) {
				buffer.insert(cursorOffset, buffer[lastLine]);
				moveCursor(win, cursorOffset+1);
				lastLine++;
			}
			
			markAsUnsaved();
		} break;
		default: {
			if (selectingText) {
				buffer.erase(selBeginOffset, selEndOffset-selBeginOffset);
				moveCursor(win, selBeginOffset);
				selectingText = false;
			}
			
			buffer.insert(cursorOffset, key.value);
			moveCursor(win, cursorOffset+1);
			
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
			int xMin = getGutterWidth(win);
			
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
