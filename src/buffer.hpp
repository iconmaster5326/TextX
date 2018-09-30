/*
 * buffer.hpp
 *
 *  Created on: Sep 29, 2018
 *      Author: iconmaster
 */

#ifndef SRC_BUFFER_HPP_
#define SRC_BUFFER_HPP_

#include <algorithm>
#include <string>
#include <deque>

namespace textx {
	using namespace std;
	
	class Buffer {
	public:
		typedef unsigned offset_t;
		typedef unsigned line_t;
		typedef unsigned col_t;
		typedef unsigned size_type;
	private:
		string text;
		deque<offset_t> lineToOffsetCache;
	public:
		// constructors
		inline Buffer() {}
		Buffer(string text);
		template<class Iter> Buffer(Iter begin, Iter end) {
			for (Iter it = begin; it != end; it++) {
				char c = *it;
				text += c;
				if (c == '\n') {
					lineToOffsetCache.push_back(text.size());
				}
			}
		}
		
		// container methods
		void insert(offset_t offset, char c);
		template<class Iter> void insert(offset_t offset, Iter begin, Iter end) {
			deque<offset_t>::iterator lineIt = upper_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
			unsigned charsCopied = 0;
			
			for (Iter it = begin; it != end; it++) {
				char c = *it;
				text.insert(offset, 1, c);
				offset++;
				
				if (c == '\n') {
					lineToOffsetCache.insert(lineIt, offset);
					lineIt++;
				}
				
				charsCopied++;
			}
			
			for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
				*restLines += charsCopied;
			}
		}
		inline void insert(offset_t offset, string s) {
			insert(offset, s.begin(), s.end());
		}
		
		void erase(offset_t offset);
		void erase(offset_t offset, size_type n);
		
		inline size_type size() {
			return text.size();
		}
		inline bool empty() {
			return text.empty();
		}
		inline char operator[](size_type n) {
			return text[n];
		}
		
		// specialized methods
		void offsetToLine(offset_t offset, line_t& line, col_t& col);
		offset_t lineToOffset(line_t line, col_t col);
		size_type lineLength(offset_t offset);
		
		inline size_type lineLength(line_t line, col_t col) {
			return lineLength(lineToOffset(line, col));
		}
		
		inline line_t offsetToLine(offset_t offset) {
			line_t line; col_t col;
			offsetToLine(offset, line, col);
			return line;
		}
		inline col_t offsetToCol(offset_t offset) {
			line_t line; col_t col;
			offsetToLine(offset, line, col);
			return col;
		}
		inline offset_t lineToOffset(line_t line) {
			return lineToOffset(line, 0);
		}
		
		deque<offset_t>::const_iterator beginLines() {
			return lineToOffsetCache.begin();
		}
		deque<offset_t>::const_iterator endLines() {
			return lineToOffsetCache.end();
		}
		
		inline deque<offset_t>::size_type lines() {
			return lineToOffsetCache.size();
		}
		
		inline const string& asString() {
			return text;
		}
	};
}

#endif /* SRC_BUFFER_HPP_ */
