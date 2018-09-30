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
		size_type pos = 0;
		while ((pos = t.find('\n', pos)) != string::npos) {
			lineToOffsetCache.push_back(pos);
		}
	}

	void Buffer::insert(offset_t offset, char c) {
		deque<offset_t>::iterator lineIt = upper_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
		
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
		}
		
		text.erase(text.begin()+offset);
		
		for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
			(*restLines)--;
		}
	}

	void Buffer::erase(offset_t offset, Buffer::size_type n) {
		deque<offset_t>::iterator lineIt = lower_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);

		for (int i = 0; i < n; i++) {
			if (text[offset] == '\n') {
				lineIt = lineToOffsetCache.erase(lineIt);
			}
			
			erase(offset);
		}
		
		for (deque<offset_t>::iterator restLines = lineIt; restLines != lineToOffsetCache.end(); restLines++) {
			*restLines -= n;
		}
	}
	
	void Buffer::offsetToLine(offset_t offset, line_t& line, col_t& col) {
		deque<offset_t>::iterator it = upper_bound(lineToOffsetCache.begin(), lineToOffsetCache.end(), offset);
		unsigned lineOff = 0;
		if (it == lineToOffsetCache.begin()) {
			line = 0;
		} else if (it == lineToOffsetCache.end()) {
			lineOff = lineToOffsetCache.back();
			line = lines()-1;
			col = size()-lineOff;
			return;
		} else {
			it--;
			lineOff = (*it)+1;
			line = it-lineToOffsetCache.begin();
		}
		
		col = offset - lineOff;
		size_type lineLen = lineLength(lineOff);
		if (col >= lineLen) col = lineLen-1;
	}

	Buffer::offset_t Buffer::lineToOffset(line_t line, col_t col) {
		if (line <= 0) {
			return 0;
		}
		
		if (line >= lines()) {
			return size()-1;
		}
		
		offset_t offset = lineToOffsetCache[line];
		if (col <= 0) return offset;
		size_type n = lineLength(offset);
		if (col >= n) return offset+n-1;
		return offset+col;
	}
	
	Buffer::size_type Buffer::lineLength(offset_t offset) {
		size_type pos = text.find('\n', offset);
		if (pos == string::npos) return text.size() - offset;
		return pos - offset;
	}
}
