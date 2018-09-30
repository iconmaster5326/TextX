/*
 * buffer.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: iconmaster
 */

#include "buffer.hpp"

namespace textx {
	using namespace std;
	
	Buffer::Buffer(string t) : text(t) {
		lineToOffsetCache.push_back(-1);
		
		string::size_type pos = 0;
		while ((pos = text.find('\n', pos)) != string::npos) {
			lineToOffsetCache.push_back(pos);
			pos++;
		}
	}

	void Buffer::insert(offset_t offset, char c) {
		deque<offset_t>::iterator lineIt = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
		
		text.insert(offset, 1, c);
		if (c == '\n') {
			lineToOffsetCache.insert(lineIt, offset);
			lineIt++;
		}
		
		for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
			(*restLines)++;
		}
	}

	void Buffer::erase(offset_t offset) {
		deque<offset_t>::iterator lineIt = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
		if (text[offset] == '\n') {
			lineIt = lineToOffsetCache.erase(lineIt);
			lineIt++;
		}
		
		text.erase(text.begin()+offset);
		
		for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
			(*restLines)--;
		}
	}

	void Buffer::erase(offset_t offset, string::size_type n) {
		deque<offset_t>::iterator lineIt = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);

		for (int i = 0; i < n; i++) {
			if (text[offset] == '\n') {
				lineIt = lineToOffsetCache.erase(lineIt);
			}
			
			text.erase(offset, 1);
		}
		
		for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
			*restLines -= n;
		}
	}
	
	void Buffer::offsetToLine(offset_t offset, line_t& line, col_t& col) {
		if (offset <= 0) {
			line = 0;
			col = 0;
			return;
		}
		
		deque<offset_t>::iterator it = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
		
		it--;
		unsigned lineOff = (*it)+1;
		line = it - lineToOffsetCache.begin();
		
		col = offset - lineOff;
		string::size_type lineLen = lineLengthAtOffset(lineOff);
		if (col >= lineLen) col = lineLen;
	}

	Buffer::offset_t Buffer::lineToOffset(line_t line, col_t col) {
		if (line >= lines()) {
			return size();
		}
		
		offset_t offset = lineToOffsetCache[line]+1;
		if (col <= 0) return offset;
		string::size_type n = lineLengthAtOffset(offset);
		if (col >= n) return offset+n;
		return offset+col;
	}
	
	string::size_type Buffer::lineLengthAtOffset(offset_t offset) {
		string::size_type pos = text.find('\n', offset);
		if (pos == string::npos) return text.size() - offset;
		return pos - offset;
	}
	
	string::size_type Buffer::lineLengthAtLine(line_t line) {
		if (line == 0) return lineLengthAtOffset(0);
		return lineLengthAtOffset(lineToOffsetCache[line]+1);
	}
}
