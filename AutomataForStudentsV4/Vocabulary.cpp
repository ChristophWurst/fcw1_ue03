// Vocabulary.cpp:                                        HDO, 2004-2015
// --------------
// Generic class Vocabulary shall be instantiated with [T|NT]Symbol only.
// An object of an instance of the generic class Vocabulary is a set of
// pointers to (possibly different types of) symbols: V = VT + VNT:
// Vocabulary.cpp would not be necessary, but the existing one
// contains a simple test program.
// =====================================================================

#include "Vocabulary.h"

// nothing to do for template class Vocabulary: the header is sufficient


// === test ============================================================

#if 0

#include <cstdio>

#include <iostream>
#include <typeinfo>

using namespace std;

#include "SymbolStuff.cpp"

int main(int argc, char *argv[]) {
try {

  SymbolPool *sp = SymbolPool::getInstance();

  Vocabulary< TSymbol> vT;
  Vocabulary<NTSymbol> vNt;

    Symbol *  sy = nullptr;
   TSymbol * tSy = sp-> tSymbol("t");
  NTSymbol *ntSy = sp->ntSymbol("NT");

  vT .insert(tSy);
  vNt.insert(ntSy);

  cout << "vT:  " << vT  << endl;
  cout << "vNt: " << vNt << endl;

  cout << "vT .contains(t) : " << vT. contains( tSy) << endl;
  cout << "vNt.contains(NT): " << vNt.contains(ntSy) << endl;

  string name;

  name = "t";
  cout << "vT .symbolFor(\"" << name << "\") : ";
  sy = vT.symbolFor(name);
  if (sy != nullptr)
    cout << *sy;
  else
    cout << "nullptr";
  cout << endl;

  name = "NT";
  cout << "vNt.symbolFor(\"" << name << "\"): ";
  sy = vNt.symbolFor(name);
  if (sy != nullptr)
    cout << *sy;
  else
    cout << "nullptr";
  cout << endl;

  delete sp;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of Vocabulary.cpp
// =====================================================================
