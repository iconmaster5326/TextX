/*
 * file_types.hpp
 *
 *  Created on: Sep 29, 2018
 *      Author: iconmaster
 */

#ifndef SRC_FILE_TYPES_HPP_
#define SRC_FILE_TYPES_HPP_

#include "curses.hpp"
#include "colors.hpp"
#include "buffer.hpp"

#include <string>
#include <set>

namespace textx {
	using namespace std;
	
	class Token {
	public:
		int length;
		ColorPair color;
		attr_t attribues;
		
		inline Token(int length, ColorPair color, attr_t attribues) : color(color) {
			this->length = length;
			this->attribues = attribues;
		}
	};
	
	class FileType {
	public:
		string name;
		
		inline FileType(const string& name) {
			this->name = name;
		}
		virtual ~FileType();
		
		virtual bool isAssociatedFile(const string& filename);
		virtual Token getToken(Buffer& buffer, Buffer::offset_t offset);
	};
	
	extern set<FileType*> allFileTypes;
	void initAllFileTypes();
	
	namespace file_type {
		extern FileType* none;
		extern FileType* c;
	}
}

#endif /* SRC_FILE_TYPES_HPP_ */
