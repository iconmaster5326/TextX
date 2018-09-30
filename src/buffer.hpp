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
		typedef int offset_t;
		typedef int line_t;
		typedef int col_t;
	private:
		string text;
		deque<offset_t> lineToOffsetCache;
	public:
		// constructors
		inline Buffer() {
			lineToOffsetCache.push_back(-1);
		}
		
		Buffer(string text);
		template<class Iter> Buffer(Iter begin, Iter end) {
			lineToOffsetCache.push_back(-1);
			
			for (Iter it = begin; it != end; it++) {
				char c = *it;
				text += c;
				if (c == '\n') {
					lineToOffsetCache.push_back(text.size()-1);
				}
			}
		}
		
		// container methods TODO add others for STL compatibility
		typedef const char& const_reference;
		inline void push_back(char c) {
			if (c == '\n') {
				lineToOffsetCache.push_back(text.size());
			}
			text += c;
		}
		
		void insert(offset_t offset, char c);
		template<class Iter> void insert(offset_t offset, Iter begin, Iter end) {
			deque<offset_t>::iterator lineIt = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
			unsigned charsCopied = 0;
			
			for (Iter it = begin; it != end; it++) {
				char c = *it;
				text.insert(offset-1, 1, c);
				offset++;
				
				if (c == '\n') {
					lineToOffsetCache.insert(lineIt, offset-2);
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
		void erase(offset_t offset, string::size_type n);
		
		inline string::size_type size() const {
			return text.size();
		}
		inline bool empty() const {
			return text.empty();
		}
		inline char operator[](string::size_type n) const {
			return text[n];
		}
		
		// specialized methods
		void offsetToLine(offset_t offset, line_t& line, col_t& col) const;
		offset_t lineToOffset(line_t line, col_t col) const;
		string::size_type lineLengthAtOffset(offset_t offset) const;
		string::size_type lineLengthAtLine(line_t line) const;
		
		inline line_t offsetToLine(offset_t offset) const {
			line_t line; col_t col;
			offsetToLine(offset, line, col);
			return line;
		}
		inline col_t offsetToCol(offset_t offset) const {
			line_t line; col_t col;
			offsetToLine(offset, line, col);
			return col;
		}
		inline offset_t lineToOffset(line_t line) const {
			return lineToOffset(line, 0);
		}
		
		typedef deque<offset_t>::const_iterator line_iterator;
		line_iterator beginLines() const {
			return lineToOffsetCache.begin();
		}
		line_iterator endLines() const {
			return lineToOffsetCache.end();
		}
		
		inline deque<offset_t>::size_type lines() const {
			return lineToOffsetCache.size();
		}
		
		inline const string& asString() const {
			return text;
		}
	};
}

#endif /* SRC_BUFFER_HPP_ */
