// Vocabulary.cpp:                                        HDO, 2004-2015
// --------------
// Generic class Vocabulary shall be instantiated with [T|NT]Symbol only.
// An object of an instance of the generic class Vocabulary is a set of
// pointers to (possibly different types of) symbols: V = VT + VNT:
// Vocabulary.cpp would not be necessary, but the existing one
// contains a simple test program.
// =====================================================================

#ifndef Vocabulary_h
#define Vocabulary_h


#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include "ObjectCounter.h"
#include "SymbolStuff.h"


template <typename SyT> // where SyT is either TSymbol or NTSymbol
class Vocabulary: public  std::set<SyT *, LessForSymbolPtrs>,
                  private ObjectCounter<Vocabulary<SyT>> {

  private:

    typedef std::set<SyT *, LessForSymbolPtrs> Base;

    // helper class to support binary search for T- and NTSymbols
    class XSymbol final: public  Symbol,
                         private ObjectCounter<XSymbol> {
      public:
        XSymbol(const std::string &name): Symbol(name) {}
        ~XSymbol() {}
    }; // XSymbol

  public:

    Vocabulary() {
      // nothing to do
    } // Vocabulary

    Vocabulary(const Vocabulary &v)
    : Base(v) {
      // nothing to do
    } // Vocabulary

    bool contains(SyT *sy) const {
      return Base::find(sy) != Base::end();
    } // contains

    SyT *symbolFor(const std::string &name) const {
      XSymbol xSy(name);
      auto it =  Base::find((SyT*)(&xSy));
      if ( it != Base::end() )
        return *it;
      else
        return nullptr;
    } // symbolFor

    bool hasSymbolWith(const std::string &name) const {
      XSymbol xSy(name);
      return Base::find((SyT*)(&xSy)) != Base::end();
    } // hasSymbolWith

}; // Vocabulary<SyT>

template <typename SyT>
static std::ostream &operator<<(std::ostream &os,
                                const Vocabulary<SyT> &v) {
  os << "{ ";
  bool first = true;
  for (auto e: v) {
    if (!first)
      os << ", ";
    os << *e;
    first = false;
  } // fore
  os << " }";
  return os;
} // operator<<


#endif


// end of Vocabulary.h
// =====================================================================
