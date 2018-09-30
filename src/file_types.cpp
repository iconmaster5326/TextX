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
	
	Token FileType::getToken(const Buffer& buffer, Buffer::offset_t offset) {
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
		
		// Base class for C, C++, Java, etc.
		class FileTypeCLike : public FileType {
		public:
			unsigned N_KEYWORDS;
			const char* const* KEYWORDS;
			
			FileTypeCLike(const string& name) : FileType(name) {}
			
			Token getToken(const Buffer& buffer, Buffer::offset_t offset) {
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
				}
				
				return Token(n, color, attrs);
			}
		};
		
		// C
		class FileTypeC : public FileTypeCLike {
		public:
			FileTypeC() : FileTypeCLike("C") {
				static const int N_KEYWORDS_ = 32;
				static const char* KEYWORDS_[N_KEYWORDS_] = {
						"auto","break","case","char",
						"const","continue","default","do",
						"double","else","enum","extern",
						"float","for","goto","if",
						"int","long","register","return",
						"short","signed","sizeof","static",
						"struct","switch","typedef","union",
						"unsigned","void","volatile","while"
				};
				
				N_KEYWORDS = N_KEYWORDS_;
				KEYWORDS = KEYWORDS_;
			}
			FileTypeC(const string& name) : FileTypeCLike(name) {}
			
			bool isAssociatedFile(const string& filename) {
				return endsWith(filename, ".c") || endsWith(filename, ".h");
			}
			
			Token getToken(const Buffer& buffer, Buffer::offset_t offset) {
				char c0 = buffer[offset];
				
				if (c0 == '#') {
					// preprocessor directive
					int n = 1;
					
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
					
					return Token(n, getColorPair(color::yellow, color::system), 0);
				}
				
				return FileTypeCLike::getToken(buffer, offset);
			}
		};
		
		static FileTypeC _c;
		FileType* c = &_c;
		
		// C++
		class FileTypeCXX : public FileTypeC {
		public:
			FileTypeCXX() : FileTypeC("C++") {
				static const int N_KEYWORDS_ = 96;
				static const char* KEYWORDS_[N_KEYWORDS_] = {
						"alignas","alignof","and","and_eq","asm","atomic_cancel","atomic_commit",
						"atomic_noexcept","auto","bitand","bitor","bool","break","case","catch",
						"char","char16_t","char32_t","class","compl","concept","const","constexpr",
						"const_cast","continue","co_await","co_return","co_yield","decltype",
						"default","delete","do","double","dynamic_cast","else","enum","explicit",
						"export","extern","false","float","for","friend","goto","if","import",
						"inline","int","long","module","mutable","namespace","new","noexcept",
						"not","not_eq","nullptr","operator","or","or_eq","private","protected",
						"public","reflexpr","register","reinterpret_cast","requires","return",
						"short","signed","sizeof","static","static_assert","static_cast","struct",
						"switch","synchronized","template","this","thread_local","throw","true",
						"try","typedef","typeid","typename","union","unsigned","using","virtual",
						"void","volatile","wchar_t","while","xor","xor_eq"
				};
				
				N_KEYWORDS = N_KEYWORDS_;
				KEYWORDS = KEYWORDS_;
			}
			
			bool isAssociatedFile(const string& filename) {
				return endsWith(filename, ".cpp") || endsWith(filename, ".hpp") || endsWith(filename, ".cxx") || endsWith(filename, ".hxx");
			}
		};
		
		static FileTypeCXX _cxx;
		FileType* cxx = &_cxx;
		
		// Java
		class FileTypeJava : public FileTypeCLike {
		public:
			FileTypeJava() : FileTypeCLike("Java") {
				static const int N_KEYWORDS_ = 53;
				static const char* KEYWORDS_[N_KEYWORDS_] = {
						"abstract","continue","for","new","switch",
						"assert","default","goto","package","synchronized",
						"boolean","do","if","private","this",
						"break","double","implements","protected","throw",
						"byte","else","import","public","throws",
						"case","enum","instanceof","return","transient",
						"catch","extends","int","short","try",
						"char","final","interface","static","void",
						"class","finally","long","strictfp","volatile",
						"const","float","native","super","while",
						"true","false","null"
				};
				
				N_KEYWORDS = N_KEYWORDS_;
				KEYWORDS = KEYWORDS_;
			}
			
			bool isAssociatedFile(const string& filename) {
				return endsWith(filename, ".java");
			}
			
			Token getToken(const Buffer& buffer, Buffer::offset_t offset) {
				char c0 = buffer[offset];
				
				if (c0 == '@') {
					// annotation
					int n = 1;
					
					int i = offset+1;
					while (i < buffer.size() && (isalnum(buffer[i]) || i == '_')) {
						i++; n++;
					}
					
					return Token(n, getColorPair(color::yellow, color::system), 0);
				}
				
				return FileTypeCLike::getToken(buffer, offset);
			}
		};
		
		static FileTypeJava _java;
		FileType* java = &_java;
	}
	
	void initAllFileTypes() {
		if (!allFileTypes.empty()) return;
		
		allFileTypes.insert(file_type::none);
		allFileTypes.insert(file_type::c);
		allFileTypes.insert(file_type::cxx);
		allFileTypes.insert(file_type::java);
	}
}
