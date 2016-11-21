// SymbolStuff.cpp:                                       HDO, 2006-2014
// ---------------
// Type TapeSymbol and class SymbolSet represent symbols and
// sets of symbols on the tape for various types of automata.
//======================================================================

#include <iostream>
#include <string>

using namespace std;

#include "SymbolStuff.h"


SymbolSet::SymbolSet() {
  // nothing to do, empty symbol set per default
} // SymbolSet::SymbolSet


SymbolSet::SymbolSet(const SymbolSet &ss)
: CharSet(ss) {
  // nothing to do
} // SymbolSet::SymbolSet


SymbolSet::SymbolSet(TapeSymbol sy)
: CharSet(sy) {
  // nothing to do, base class constructor does the whole work
} // SymbolSet::SymbolSet


SymbolSet::SymbolSet(string str) // format for str: '{' sy { ',' sy } '}'
: CharSet(str) {
  // nothing to do, base class constructor does the whole work
} // SymbolSet::SymbolSet


#ifdef INITIALIZER_LIST_AVAILABLE
SymbolSet::SymbolSet(initializer_list<TapeSymbol> il)
: CharSet(il) {
  // nothing to do, base class constructor does the whole work
} // // SymbolSet::SymbolSet
#endif


SymbolSet::~SymbolSet() {
  // nothing to do
} // SymbolSet::~SymbolSet


// === test ============================================================

#if 0

#include "CharSet.cpp"

int main(int argc, char *argv[]) {
try {

  cout << "START: SymbolStuff" << endl;
  cout << endl;

  SymbolSet emptySet;
  cout << "emptySet = " << emptySet << endl;

  SymbolSet abcSet1("{ a, b, c }");
  cout << "abcSet1  = " << abcSet1  << endl;

#ifdef INITIALIZER_LIST_AVAILABLE
  SymbolSet abcSet2({ 'a', 'b', 'c' });
  cout << "abcSet2  = " << abcSet2  << endl;
#endif

  // SymbolSet error1("a }");
  // SymbolSet error2("{ b");

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

// end of SymbolStuff.cpp
//======================================================================
