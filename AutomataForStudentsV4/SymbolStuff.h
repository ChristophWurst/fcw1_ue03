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


#endif

// end of SymbolStuff.h
//======================================================================

