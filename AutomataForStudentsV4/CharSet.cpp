// SymbolStuff.cpp:                                       HDO, 2006-2014
// ---------------
// Type TapeSymbol and class SymbolSet represent symbols and
// sets of symbols on the tape for various types of automata.
//======================================================================

#include <algorithm>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

using namespace std;

#include "CharSet.h"


CharSet::CharSet() {
  // nothing to do, empty char set per default
} // CharSet::CharSet


CharSet::CharSet(const CharSet &cs)
: set<char>(cs) {
  // nothing to do
} // CharSet::CharSet


CharSet::CharSet(char ch) {
  insert(ch);
} // CharSet::CharSet


CharSet::CharSet(string str) { // format for str: '{' c { ',' c } '}'
  // 1. strip blanks in str
  string::iterator newEnd;
  newEnd = remove(str.begin(), str.end(), ' ');
  str.erase(newEnd, str.end());
  // 2. convert string str to set<char>
  if (str[0] != '{')
    throw invalid_argument("leading '{' missing in set \"" + str + "\"");
  for (unsigned int i = 1; i < str.length() - 1; i++) {
    if (str[i] != ',')
      insert(str[i]);
  } // for
  if (str[str.length() - 1] != '}')
    throw invalid_argument("trailing '}' missing in set \"" + str + "\"");
} // CharSet::CharSet


#ifdef INITIALIZER_LIST_AVAILABLE
CharSet::CharSet(initializer_list<char> il) {
  for (char ch: il) {
    insert(ch);
  } // for
} // CharSet::CharSet
#endif


CharSet::~CharSet() {
  // nothing to do
} // CharSet::~CharSet


ostream &operator<<(ostream &os, const CharSet &cs) {
  cout << "{ ";
  bool first = true;
  for (char e: cs) {
    if (!first)
      cout << ", ";
    cout << e;
    first = false;
  } // for
  cout << " }";
  return os;
} // operator<<


// === test ============================================================

#if 0

int main(int argc, char *argv[]) {
try {

  cout << "START: CharSet" << endl;
  cout << endl;

  CharSet emptySet;
  cout << "emptySet  = " << emptySet  << endl;

  CharSet singleSet('1');
  cout << "singleSet = " << singleSet << endl;

  CharSet abcSet1("{ a, b, c }");
  cout << "abcSet1   = " << abcSet1   << endl;

#ifdef INITIALIZER_LIST_AVAILABLE
  CharSet abcSet2({'a', 'b', 'c'}); // or: abcSet2{'a', 'b', 'c'}
  cout << "abcSet2   = " << abcSet2   << endl;
#endif

  CharSet copySet(abcSet1);
  cout << "copySet   = " << copySet    << endl;

  // CharSet error1("a }");
  // CharSet error2("{ b");

  cout << endl;
  cout << "END" << endl;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of CharSet.h
//======================================================================
