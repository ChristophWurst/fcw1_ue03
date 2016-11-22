// SymbolStuff.h:                                         HDO, 2006-2014
// -------------
// Type TapeSymbol and class SymbolSet represent symbols and
// sets of symbols on the tape for various types of automata.
//======================================================================

#ifndef SymbolStuff_h
#define SymbolStuff_h


#include "ObjectCounter.h"
#include "CharSet.h"


#define SYMBOLS     127   // max. number of diff. tape symbols (only chars)

#define EOT_SYMBOL  '\0'  // end of tape symbol (used as sentinel on tape)
#define EPS         '~'   // ~ as neither greek e nor lambda in font
#define EPSILON     '~'   // ~ as neither greek e nor lambda in font


typedef char TapeSymbol;  // Tape to prevent name clash with class Symbol for Grammars


class SymbolSet: public CharSet,  // equivalent to std::set<TapeSymbol>
                 private ObjectCounter<SymbolSet> {

  public:

    SymbolSet();
    SymbolSet(const SymbolSet &ss);
    SymbolSet(TapeSymbol sy);

    SymbolSet(std::string str); // format for str: '{' sy { ',' sy } '}'
      // ... where sy is a tape symbol, typically a letter

#ifdef INITIALIZER_LIST_AVAILABLE
    SymbolSet(initializer_list<TapeSymbol> il);
#endif

    virtual ~SymbolSet();

}; // SymbolSet

// std::ostream &operator<<(std::ostream &os, const SymbolSet &ss); not necessary as
// std::ostream &operator<<(std::ostream &os, const CharSet   &cs); works fine


#include <iosfwd>
#include <unordered_map>
#include <memory>
#include <string>
#include <typeinfo>

#include "ObjectCounter.h"


class   SymbolPool;  // singleton providing a factory for T- and NTSymbols

class   Symbol;      // abstract base class for
class  TSymbol;      // 1)  concrete class and
class NTSymbol;      // 2)  concrete class


// === class SymbolPool ================================================

class SymbolPool final: // no public base class
                        private ObjectCounter<SymbolPool> {

  friend std::ostream &operator<<(std::ostream &os, const SymbolPool &sp);

  private:

    static SymbolPool *instance; // the one and only SymbolPool (singleton)

    std::unordered_map<std::string,  TSymbol *>  tSyMap;
    std::unordered_map<std::string, NTSymbol *> ntSyMap;

    SymbolPool(); // with implementation for singleton pattern only
    SymbolPool(const SymbolPool *sp); // prohibit copy construction and ...
    SymbolPool &operator=(const SymbolPool *sp); // ... assignment

  public:

    static SymbolPool *getInstance(); // create or get the singleton

    ~SymbolPool();

     TSymbol * tSymbol(const std::string name);
    NTSymbol *ntSymbol(const std::string name);

}; // SymbolPool

std::ostream &operator<<(std::ostream &os, const SymbolPool &sp);


// === class Symbol ====================================================

class Symbol { // abstract base class, so no object counting necessary

private:

    Symbol(const Symbol *sy);  // prohibit copy construction and ...
    Symbol &operator=(const Symbol *sy); // ... assignment

  protected:

    Symbol(const std::string &name);
    virtual ~Symbol() = 0;     // abstract method for abstract class
                               //   but implementation is necessary (!)

  public:

    const std::string name;    // const as symbols may be elements of set

    virtual bool isT () const; // is symbol a T  in a grammar
    virtual bool isNT() const; // is symbol a NT in a grammar

}; // Symbol

bool isT (const Symbol *sy);   // sy->isT () <==> isT (sy)
bool isNT(const Symbol *sy);   // sy->isNT() <==> isNT(sy)

bool operator< (const Symbol &sy1, const Symbol &sy2);
bool operator> (const Symbol &sy1, const Symbol &sy2);
bool operator==(const Symbol &sy1, const Symbol &sy2);
bool operator!=(const Symbol &sy1, const Symbol &sy2);

struct LessForSymbolPtrs {
  bool operator()(const Symbol *sy1, const Symbol *sy2) const;
}; // LessForSymbolPtrs

struct EqualForSymbolPtrs {
  bool operator()(const Symbol *sy1, const Symbol *sy2) const;
}; // EqualForSymbolPtrs

std::ostream &operator<<(std::ostream &os, const Symbol &sy);


// === class TSymbol ===================================================

class TSymbol final: public  Symbol,
                     private ObjectCounter<TSymbol> {

  friend class SymbolPool; // for construction and destruction only

  private:

    TSymbol(const TSymbol *tSy); // prohibit copy construction and ...
    TSymbol &operator=(const TSymbol *tSy); // ... assignment

  protected:

    TSymbol(const std::string &name);
    virtual ~TSymbol();

}; // TSymbol


// === class NTSymbol ==================================================

class NTSymbol final: public  Symbol,
                      private ObjectCounter<NTSymbol> {

  friend class SymbolPool; // for construction and destruction only

  private:

    NTSymbol(const NTSymbol *tSy); // prohibit copy construction and
    NTSymbol &operator=(const NTSymbol *tSy); // assignment

  protected:

    NTSymbol(const std::string &name);
    virtual ~NTSymbol();

}; // NTSymbol

#endif

// end of SymbolStuff.h
//======================================================================

