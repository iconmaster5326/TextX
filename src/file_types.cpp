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
	
	Token FileType::getToken(Buffer& buffer, Buffer::offset_t offset) {
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
			
			Token getToken(Buffer& buffer, Buffer::offset_t offset) {
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
						if (buffer.asString().compare(offset, n, KEYWORDS[i]) == 0) {
							color = getColorPair(color::blue, color::system);
							attrs = A_BOLD;
							break;
						}
					}
				} else if (isdigit(c0) || c0 == '.' || c0 == '-') {
					// number
					
					
					int i = offset+1;
					bool gotPeriod = c0 == '.';
					while (i < buffer.size() && (isdigit(buffer[i]) || (!gotPeriod && buffer[i] == '.'))) {
						if (!gotPeriod && buffer[i] == '.') gotPeriod = true;
						i++; n++;
					}
					
					if (!(n == 1 && (gotPeriod || c0 == '-'))) {
						color = getColorPair(color::magenta, color::system);
						
						if (buffer[i] == 'e' || buffer[i] == 'E') {
							i++; n++;
							if (buffer[i] == '-' || buffer[i] == '+' || isdigit(buffer[i])) {
								i++; n++;
								while (i < buffer.size() && isdigit(buffer[i])) {
									i++; n++;
								}
							}
						}
						
						if (buffer[i] == 'f' || buffer[i] == 'l' || buffer[i] == 'F' || buffer[i] == 'L') {
							i++; n++;
						}
					}
					
					
				} else if (c0 == '/' && buffer.size() > offset+1 && buffer[offset+1] == '/') {
					// comment
					color = getColorPair(color::green, color::system);
					
					int i = offset+1;
					while (i < buffer.size() && (buffer[i] != '\n')) {
						i++; n++;
					}
				} else if (c0 == '/' && buffer.size() > offset+1 && buffer[offset+1] == '*') {
					// multiline comment (TODO properly render ones that come from before offset)
					color = getColorPair(color::green, color::system);
					
					int i = offset+1;
					while (i < buffer.size()-1 && !(buffer[i] == '*' && buffer[i+1] == '/')) {
						i++; n++;
					}
					n += 2;
				} else if (c0 == '"') {
					// string constant
					color = getColorPair(color::cyan, color::system);
					
					int i = offset+1;
					bool escape = false;
					while (i < buffer.size() && (escape || buffer[i] != '"')) {
						if (escape) {
							escape = false;
						} else if (buffer[i] == '\\') {
							escape = true;
						}
						
						i++; n++;
					}
					n++;
				} else if (c0 == '\'') {
					// char constant
					color = getColorPair(color::cyan, color::system);
					
					int i = offset+1;
					bool escape = false;
					while (i < buffer.size() && (escape || buffer[i] != '\'')) {
						if (escape) {
							escape = false;
						} else if (buffer[i] == '\\') {
							escape = true;
						}
						
						i++; n++;
					}
					n++;
				} else if (c0 == '#') {
					// preprocessor directive
					color = getColorPair(color::yellow, color::system);
					
					int i = offset+1;
					bool escape = false;
					while (i < buffer.size() && (escape || buffer[i] != '\n')) {
						if (escape) {
							escape = false;
						} else if (buffer[i] == '\\') {
							escape = true;
						}
						i++; n++;
					}
				}
				
				return Token(n, color, attrs);
			}
		};
		
		static FileTypeC _c;
		FileType* c = &_c;
	}
	
	void initAllFileTypes() {
		if (!allFileTypes.empty()) return;
		
		allFileTypes.insert(file_type::none);
		allFileTypes.insert(file_type::c);
	}
}
