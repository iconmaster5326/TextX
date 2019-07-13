/*
 * test_buffer.cpp
 *
 *  Created on: Sep 30, 2018
 *      Author: iconmaster
 */

#include "buffer.hpp"

#include <iostream>

using namespace std;
using namespace textx;

/*
 * Assert macros
 */
#define ASSERT_TRUE(a) if (!(a)) return "Assertion (" #a ") failed.";
#define ASSERT_EQUALS(a,b) if ((a) != (b)) return "Assertion (" #a " == " #b ") failed.";
#define ASSERT_NOT_EQUALS(a,b) if ((a) == (b)) return "Assertion (" #a " != " #b ") failed.";

/*
 * Test cases
 */
string testStringCtor1Line() {
	string s = "Hello, World!";
	
	Buffer b(s);
	ASSERT_EQUALS(b.lines(), 1);
	ASSERT_EQUALS(b.size(), s.size());
	ASSERT_EQUALS(b.asString(), s);
	
	ASSERT_EQUALS(b.lineLengthAtLine(0), s.size());
	ASSERT_EQUALS(b.lineLengthAtOffset(0), s.size());
	
	ASSERT_EQUALS(b.lineToOffset(0), 0);
	ASSERT_EQUALS(b.lineToOffset(1), s.size());
	ASSERT_EQUALS(b.lineToOffset(2), s.size());
	
	ASSERT_EQUALS(b.lineToOffset(0, 0), 0);
	ASSERT_EQUALS(b.lineToOffset(0, 3), 3);
	ASSERT_EQUALS(b.lineToOffset(0, 500), s.size());
	ASSERT_EQUALS(b.lineToOffset(1, 500), s.size());
	
	for (int i = 0; i < s.size(); i++) {
		ASSERT_EQUALS(b.offsetToLine(i), 0);
		ASSERT_EQUALS(b.offsetToCol(i), i);
	}
	
	ASSERT_EQUALS(b.offsetToLine(s.size()+2), 0);
	ASSERT_EQUALS(b.offsetToCol(s.size()+2), s.size());
	
	return "";
}

string testStringCtor2Lines() {
	string s = "Hello\nWorld";
	
	Buffer b(s);
	ASSERT_EQUALS(b.lines(), 2);
	ASSERT_EQUALS(b.size(), s.size());
	ASSERT_EQUALS(b.asString(), s);
	
	ASSERT_EQUALS(b.lineLengthAtLine(0), 5);
	ASSERT_EQUALS(b.lineLengthAtLine(1), 5);
	ASSERT_EQUALS(b.lineLengthAtOffset(0), 5);
	ASSERT_EQUALS(b.lineLengthAtOffset(6), 5);
	
	ASSERT_EQUALS(b.lineToOffset(0), 0);
	ASSERT_EQUALS(b.lineToOffset(1), 6);
	ASSERT_EQUALS(b.lineToOffset(2), s.size());
	ASSERT_EQUALS(b.lineToOffset(3), s.size());
	
	ASSERT_EQUALS(b.lineToOffset(0, 0), 0);
	ASSERT_EQUALS(b.lineToOffset(0, 3), 3);
	ASSERT_EQUALS(b.lineToOffset(0, 10), 5);
	ASSERT_EQUALS(b.lineToOffset(1, 0), 6);
	ASSERT_EQUALS(b.lineToOffset(1, 3), 9);
	ASSERT_EQUALS(b.lineToOffset(1, 10), s.size());
	
	for (int i = 0; i < s.size(); i++) {
		ASSERT_EQUALS(b.offsetToLine(i), i < 6 ? 0 : 1);
		ASSERT_EQUALS(b.offsetToCol(i), i < 6 ? i : i-6);
	}
	
	ASSERT_EQUALS(b.offsetToLine(s.size()+2), 1);
	ASSERT_EQUALS(b.offsetToCol(s.size()+2), 5);
	
	return "";
}

string testStringCtorManyLines() {
	string s = "A\nB\nC\nD\nE\nF\nG\n";
	
	Buffer b(s);
	ASSERT_EQUALS(b.lines(), 8);
	ASSERT_EQUALS(b.size(), s.size());
	ASSERT_EQUALS(b.asString(), s);
	
	for (int i = 0; i < 7; i++) {
		ASSERT_EQUALS(b.lineLengthAtLine(i), 1);
		
		ASSERT_EQUALS(b.lineToOffset(i), i*2);
		ASSERT_EQUALS(b.lineToOffset(i, 0), i*2);
		ASSERT_EQUALS(b.lineToOffset(i, 1), i*2+1);
		ASSERT_EQUALS(b.lineToOffset(i, 2), i*2+1);
	}
	
	return "";
}

string testStringCtorEmptyLines() {
	string s = "\n\n\n";
	
	Buffer b(s);
	ASSERT_EQUALS(b.lines(), 4);
	ASSERT_EQUALS(b.size(), s.size());
	ASSERT_EQUALS(b.asString(), s);
	
	for (int i = 0; i < b.lines(); i++) {
		ASSERT_EQUALS(b.lineLengthAtLine(i), 0);
		ASSERT_EQUALS(b.lineToOffset(i), i);
		ASSERT_EQUALS(b.lineToOffset(i, 0), i);
		ASSERT_EQUALS(b.lineToOffset(i, 1), i == 0 ? 0 : i);
		ASSERT_EQUALS(b.lineToOffset(i, 500), i == 0 ? 0 : i);
		ASSERT_EQUALS(b.offsetToLine(i), i);
		ASSERT_EQUALS(b.offsetToCol(i), 0);
	}
	
	return "";
}

string testIterCtor() {
	string s = "Hello\nWorld";
	
	Buffer b(s.begin(), s.end());
	ASSERT_EQUALS(b.lines(), 2);
	ASSERT_EQUALS(b.size(), s.size());
	ASSERT_EQUALS(b.asString(), s);
	
	ASSERT_EQUALS(b.lineLengthAtLine(0), 5);
	ASSERT_EQUALS(b.lineLengthAtLine(1), 5);
	ASSERT_EQUALS(b.lineLengthAtOffset(0), 5);
	ASSERT_EQUALS(b.lineLengthAtOffset(6), 5);
	
	ASSERT_EQUALS(b.lineToOffset(0), 0);
	ASSERT_EQUALS(b.lineToOffset(1), 6);
	ASSERT_EQUALS(b.lineToOffset(2), s.size());
	ASSERT_EQUALS(b.lineToOffset(3), s.size());
	
	ASSERT_EQUALS(b.lineToOffset(0, 0), 0);
	ASSERT_EQUALS(b.lineToOffset(0, 3), 3);
	ASSERT_EQUALS(b.lineToOffset(0, 10), 5);
	ASSERT_EQUALS(b.lineToOffset(1, 0), 6);
	ASSERT_EQUALS(b.lineToOffset(1, 3), 9);
	ASSERT_EQUALS(b.lineToOffset(1, 10), s.size());
	
	for (int i = 0; i < s.size(); i++) {
		ASSERT_EQUALS(b.offsetToLine(i), i < 6 ? 0 : 1);
		ASSERT_EQUALS(b.offsetToCol(i), i < 6 ? i : i-6);
	}
	
	ASSERT_EQUALS(b.offsetToLine(s.size()+2), 1);
	ASSERT_EQUALS(b.offsetToCol(s.size()+2), 5);
	
	return "";
}

string testInsertChar() {
	Buffer b;
	ASSERT_EQUALS(b.lines(), 1);
	
	b.insert(0, 'a'); ASSERT_EQUALS(b.lines(), 1);
	b.insert(0, '\n'); ASSERT_EQUALS(b.lines(), 2);
	b.insert(0, 'b'); ASSERT_EQUALS(b.lines(), 2);
	b.insert(0, 'c'); ASSERT_EQUALS(b.lines(), 2);
	b.insert(0, '\n'); ASSERT_EQUALS(b.lines(), 3);
	b.insert(0, 'd'); ASSERT_EQUALS(b.lines(), 3);
	
	ASSERT_EQUALS(b.asString(), "d\ncb\na");
	ASSERT_EQUALS(b.lines(), 3);
	
	Buffer::line_iterator it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 1);
	it++;
	ASSERT_EQUALS(*it, 4);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	return "";
}

string testInsertString() {
	Buffer b("Hellorld\n!");
	ASSERT_EQUALS(b.lines(), 2);
	
	b.insert(4, "o\nooo\nW");
	
	ASSERT_EQUALS(b.asString(), "Hello\nooo\nWorld\n!");
	ASSERT_EQUALS(b.lines(), 4);
	
	Buffer::line_iterator it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 5);
	it++;
	ASSERT_EQUALS(*it, 9);
	it++;
	ASSERT_EQUALS(*it, 15);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	return "";
}

string testEraseChar() {
	Buffer::line_iterator it;
	Buffer b("01234\n6789\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 5);
	it++;
	ASSERT_EQUALS(*it, 10);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(3);
	ASSERT_EQUALS(b.asString(), "0124\n6789\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 4);
	it++;
	ASSERT_EQUALS(*it, 9);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(8-1);
	ASSERT_EQUALS(b.asString(), "0124\n679\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 4);
	it++;
	ASSERT_EQUALS(*it, 8);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(5-1);
	ASSERT_EQUALS(b.asString(), "0124679\nb");
	ASSERT_EQUALS(b.lines(), 2);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 8);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	return "";
}

string testEraseString() {
	Buffer::line_iterator it;
	Buffer b("01234\n6789\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 5);
	it++;
	ASSERT_EQUALS(*it, 10);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(1, 3);
	ASSERT_EQUALS(b.asString(), "04\n6789\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 2);
	it++;
	ASSERT_EQUALS(*it, 7);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(3, 2);
	ASSERT_EQUALS(b.asString(), "04\n89\nb");
	ASSERT_EQUALS(b.lines(), 3);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 2);
	it++;
	ASSERT_EQUALS(*it, 5);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	b.erase(1, 3);
	ASSERT_EQUALS(b.asString(), "09\nb");
	ASSERT_EQUALS(b.lines(), 2);
	
	it = b.beginLines();
	ASSERT_EQUALS(*it, -1);
	it++;
	ASSERT_EQUALS(*it, 2);
	it++;
	ASSERT_EQUALS(it, b.endLines());
	
	return "";
}

string testEraseNewlines() {
	Buffer::line_iterator it;
	Buffer b("\n\n\n\n");
	ASSERT_EQUALS(b.lines(), 5);
	
	b.erase(3);
	ASSERT_EQUALS(b.asString(), "\n\n\n");
	ASSERT_EQUALS(b.lines(), 4);
	
	b.erase(0);
	ASSERT_EQUALS(b.asString(), "\n\n");
	ASSERT_EQUALS(b.lines(), 3);
	
	b.erase(1);
	ASSERT_EQUALS(b.asString(), "\n");
	ASSERT_EQUALS(b.lines(), 2);
	
	b.erase(0);
	ASSERT_EQUALS(b.asString(), "");
	ASSERT_EQUALS(b.lines(), 1);
	
	return "";
}

string testInsetOnEmptyLine() {
	Buffer b("\n\n\n");
	ASSERT_EQUALS(b.lines(), 4);
	
	b.insert(2, "abc");
	ASSERT_EQUALS(b.asString(), "\n\nabc\n");
	ASSERT_EQUALS(b.lines(), 4);
	
	return "";
}

#include <fstream>
#include <iterator>

string testLoadReadme() {
	ifstream readme("README.md");
	ASSERT_TRUE(readme.good());
	noskipws(readme);
	Buffer b;
	copy(istream_iterator<char>(readme), istream_iterator<char>(), back_inserter(b));
	b.insert(0, '\n');
	for (int i = 0; i < b.size()-1; i++) {
		char c = b[i];
		if (c == '\n') {
			ASSERT_EQUALS(b.offsetToCol(i+1), 0);
		} else {
			ASSERT_NOT_EQUALS(b.offsetToCol(i+1), 0);
		}
	}
	return "";
}

/*
 * Test harness
 */
typedef string (*Test)();

int totalTests = 0;
int passedTests = 0;

#define TEST(fn) testImpl(#fn,fn)

void testImpl(const string& name, Test t) {
	totalTests++;
	
	cout << name << "... ";
	cout.flush();
	
	string s = t();
	if (s.empty()) {
		cout << "ok" << endl;
		passedTests++;
	} else {
		cout << "FAILED" << endl;
		cout << '\t' << s << endl;
	}
}

int main(int argc, char** argv) {
	// run tests
	TEST(testStringCtor1Line);
	TEST(testStringCtor2Lines);
	TEST(testStringCtorManyLines);
	TEST(testStringCtorEmptyLines);
	TEST(testIterCtor);
	TEST(testInsertChar);
	TEST(testInsertString);
	TEST(testEraseChar);
	TEST(testEraseString);
	TEST(testEraseNewlines);
	TEST(testInsetOnEmptyLine);
	TEST(testLoadReadme);
	
	// print results
	cout << endl;
	cout << "Tests run: " << totalTests << endl;
	cout << "Passed: " << passedTests << endl;
	cout << "Failed: " << totalTests-passedTests << endl;
	
	// exit with 1 if tests failed, 0 otherwise
	return passedTests != totalTests;
}
