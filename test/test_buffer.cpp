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
	
	// print results
	cout << endl;
	cout << "Tests run: " << totalTests << endl;
	cout << "Passed: " << passedTests << endl;
	cout << "Failed: " << totalTests-passedTests << endl;
	
	// exit with 1 if tests failed, 0 otherwise
	return passedTests != totalTests;
}
