/*
 * curses.hpp
 *
 * This is a single-header C++ port of the C curses library,
 * letting you manipulate curses concepts in the OO way.
 *
 */

#ifndef SRC_CURSES_HPP_
#define SRC_CURSES_HPP_

#include <curses.h>

#include <cstdarg>
#include <string>

namespace curses {
	using namespace std;
	
	class Window;
	
	class MouseEvent { public:
		MEVENT event;
		
		inline int x() {
			return event.x;
		}
		
		inline int y() {
			return event.y;
		}
		
		inline bool shift() {
			return (event.bstate & BUTTON_SHIFT) != 0;
		}
		
		inline bool alt() {
			return (event.bstate & BUTTON_ALT) != 0;
		}
		
		inline bool control() {
			return (event.bstate & BUTTON_CTRL) != 0;
		}
		
		inline bool down(int n) {
			return BUTTON_PRESS(event.bstate, n) != 0;
		}
		
		inline bool up(int n) {
			return BUTTON_RELEASE(event.bstate, n) != 0;
		}
		
		inline bool click(int n) {
			return BUTTON_CLICK(event.bstate, n) != 0;
		}
		
		inline bool doubleClick(int n) {
			return BUTTON_DOUBLE_CLICK(event.bstate, n) != 0;
		}
		
		inline bool tripleClick(int n) {
			return BUTTON_TRIPLE_CLICK(event.bstate, n) != 0;
		}
		
		inline bool move() {
			return (event.bstate & ~(BUTTON_SHIFT | BUTTON_ALT | BUTTON_CTRL)) == 0;
		}
	};
	
	class KeyCode { public:
		int value;
		
		inline KeyCode(int value) {
			this->value = value;
		}
		
		inline bool isChar() {
			return value >= 0 && value <= 255;
		}
		
		inline char asChar() {
			return (char)value;
		}
		
		inline bool isMouseEvent() {
			return value == KEY_MOUSE;
		}
		
		inline MouseEvent asMouseEvent() {
			MouseEvent ev = MouseEvent();
			getmouse(&ev.event);
			return ev;
		}
		
		inline string name() {
			return string(keyname(value));
		}
	};
	
	class Window { public:
		WINDOW* raw = NULL;
		
		inline Window() {}
		
		inline Window(WINDOW* raw) {
			this->raw = raw;
		};
		
		inline Window(int x, int y, int w, int h) {
			this->raw = newwin(h, w, y, x);
		}
		
		inline Window(Window win, int x, int y, int w, int h) {
			int px, py; win.getPosition(px, py);
			this->raw = newwin(h, w, py+y, px+x);
		}
		
		inline ~Window() {
			// TODO delwin(raw);
		}
		
		// input
		inline KeyCode getKey() {
			return KeyCode(wgetch(raw));
		}
		
		// output
		inline void print(char c) {
			waddch(raw, c);
		}
		
		inline void print(string s) {
			waddstr(raw, s.c_str());
		}
		
		inline void println() {
			waddch(raw, '\n');
		}
		
		inline void println(string s) {
			waddstr(raw, s.c_str());
			waddch(raw, '\n');
		}
		
		inline void printf(string s, ...) {
			va_list args;
			va_start(args, s);
			vwprintw(raw, s.c_str(), args);
			va_end(args);
		}
		
		inline void refresh() {
			wrefresh(raw);
		}
		
		inline void scrollUp() {
			wscrl(raw, 1);
		}
		
		inline void scrollUp(int n) {
			wscrl(raw, n);
		}
		
		inline void clear() {
			wclear(raw);
		}
		
		// cursor control
		inline void setCursor(int x, int y) {
			wmove(raw, y, x);
		}
		
		inline void moveCursor(int dx, int dy) {
			int x, y;
			getyx(raw, y, x);
			wmove(raw, y+dy, x+dx);
		}
		
		// box drawing
		inline void drawBorder() {
			wborder(raw, 0, 0, 0, 0, 0, 0, 0, 0);
		}
		
		inline void drawBorder(string chars) {
			wborder(raw, chars[0], chars[1], chars[2], chars[3], chars[4], chars[5], chars[6], chars[7]);
		}
		
		inline void drawHLine(char ch) {
			whline(raw, ch, 99999999);
		}
		
		inline void drawVLine(char ch) {
			wvline(raw, ch, 99999999);
		}
		
		inline void drawHLine(char ch, int n) {
			whline(raw, ch, n);
		}
		
		inline void drawVLine(char ch, int n) {
			wvline(raw, ch, n);
		}
		
		// attributes
		inline void setAttributes(attr_t attrs) {
			wattrset(raw, attrs);
		}
		
		inline void setAttributesAt(attr_t attrs, int n) {
			wchgat(raw, n, attrs, 0, NULL);
		}
		
		inline void setAttributesAtChar(attr_t attrs) {
			wchgat(raw, 1, attrs, 0, NULL);
		}
		
		inline void setAttributesAtLine(attr_t attrs) {
			wchgat(raw, -1, attrs, 0, NULL);
		}
		
		inline void enableAttributes(attr_t attrs) {
			wattron(raw, attrs);
		}
		
		inline void disableAttributes(attr_t attrs) {
			wattroff(raw, attrs);
		}
		
		// TODO: this doesn't compile in some versions of ncurses
//		inline attr_t getAttributes() {
//			attr_t attrs;
//			wattr_get(raw, &attrs, NULL, NULL);
//			return attrs;
//		}
		
		// color
		inline void setColor(short pair) {
			wcolor_set(raw, pair, NULL);
		}
		
		inline void setAttributesAt(attr_t attrs, short pair, int n) {
			wchgat(raw, n, attrs, pair, NULL);
		}
		
		inline void setAttributesAtChar(attr_t attrs, short pair) {
			wchgat(raw, 1, attrs, pair, NULL);
		}
		
		inline void setAttributesAtLine(attr_t attrs, short pair) {
			wchgat(raw, -1, attrs, pair, NULL);
		}
		
		// window info
		inline void getCursor(int& x, int& y) {
			getyx(raw, y, x);
		}
		
		inline int cursorX() {
			int x, y;
			getCursor(x, y);
			return x;
		}
		
		inline int cursorY() {
			int x, y;
			getCursor(x, y);
			return y;
		}
		
		inline void getSize(int& w, int& h) {
			getmaxyx(raw, h, w);
		}
		
		inline int width() {
			int w, h;
			getSize(w, h);
			return w;
		}
		
		inline int height() {
			int w, h;
			getSize(w, h);
			return h;
		}
		
		inline void getPosition(int& x, int& y) {
			getbegyx(raw, y, x);
		}
		
		inline int x() {
			int x, y;
			getPosition(x, y);
			return x;
		}
		
		inline int y() {
			int x, y;
			getPosition(x, y);
			return y;
		}
	};
	
	// startup/shutdown
	inline Window startCurses() {
		initscr();
		raw();
		keypad(stdscr, TRUE);
		noecho();
		scrollok(stdscr, TRUE);
		
		if(has_colors() == TRUE) {
			start_color();
			use_default_colors();
		}
		mousemask(ALL_MOUSE_EVENTS, NULL);
		
		clear();
		return Window(stdscr);
	}
	
	inline void stopCurses() {
		endwin();
	}
	
	// colors
	inline bool setColorPair(short pair, short fg, short bg) {
		return init_pair(pair, fg, bg) != ERR;
	}
	
	inline bool setColor(short color, unsigned char r, unsigned char g, unsigned char b) {
		if (can_change_color() == FALSE) return false;
		return init_color(color, r/255.0*1000.0, g/255.0*1000.0, b/255.0*1000.0) != ERR;
	}
	
	inline bool getColorPair(short pair, short& fg, short& bg) {
		return pair_content(pair, &fg, &bg) != ERR;
	}
	
	inline bool getColor(short color, unsigned char& r, unsigned char& g, unsigned char& b) {
		short rawR, rawG, rawB;
		int ret = color_content(color, &rawR, &rawG, &rawB);
		r = rawR/1000.0*255.0;
		g = rawG/1000.0*255.0;
		b = rawB/1000.0*255.0;
		return ret != ERR;
	}
	
	// misc
	enum CursorState {
		CURSOR_HIDDEN,
		CURSOR_SHOWN,
		CURSOR_HIGHLIGHTED,
	};
	
	inline void setCursor(CursorState state) {
		curs_set(state);
	}
}

#endif /* SRC_CURSES_HPP_ */
