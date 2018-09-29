/*
 * file_types.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: iconmaster
 */

#include "file_types.hpp"

#include <cctype>

namespace textx {
	using namespace std;
	
	// FileType
	FileType::~FileType() {}
	
	bool FileType::isAssociatedFile(const string& filename) {
		return false;
	}
	
	Token FileType::getToken(const string& buffer, int offset) {
		return Token(0, color::pair::system, 0);
	}
	
	set<FileType*> allFileTypes;
	
	// utils for below
	static bool endsWith(const string &fullString, const string &ending) {
	    if (fullString.length() >= ending.length()) {
	        return fullString.compare(fullString.length()-ending.length(), ending.length(), ending) == 0;
	    } else {
	        return false;
	    }
	}
	
	namespace file_type {
		static FileType _none("Plain Text");
		FileType* none = &_none;
		
		// C
		class FileTypeC : public FileType {
		public:
			FileTypeC() : FileType("C") {}
			
			bool isAssociatedFile(const string& filename) {
				return endsWith(filename, ".c") || endsWith(filename, ".h");
			}
			
			Token getToken(const string& buffer, int offset) {
				static const size_t N_KEYWORDS = 32;
				static const string KEYWORDS[N_KEYWORDS] = {
						"auto","break","case","char",
						"const","continue","default","do",
						"double","else","enum","extern",
						"float","for","goto","if",
						"int","long","register","return",
						"short","signed","sizeof","static",
						"struct","switch","typedef","union",
						"unsigned","void","volatile","while"
				};
				
				int n = 1;
				ColorPair color = color::pair::system;
				attr_t attrs = 0;
				
				char c0 = buffer[offset];
				if (isalpha(c0) || c0 == '_') {
					// word
					int i = offset+1;
					while (i < buffer.size() && (isalnum(buffer[i]) || buffer[i] == '_')) {
						i++; n++;
					}
					
					// is keyword?
					for (int i = 0; i < N_KEYWORDS; i++) {
						if (buffer.compare(offset, n, KEYWORDS[i]) == 0) {
							color = getColorPair(color::blue, color::black);
							attrs = A_BOLD;
							break;
						}
					}
				}
				
				return Token(n, color, attrs);
			}
		};
		
		static FileTypeC _c;
		FileType* c = &_c;
	}
	
	void initAllFileTypes() {
		allFileTypes.insert(file_type::none);
		allFileTypes.insert(file_type::c);
	}
}
