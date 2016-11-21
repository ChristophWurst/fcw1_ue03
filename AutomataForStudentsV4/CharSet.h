// CharSet.h:                                            HDO, 2006-08-24
// ---------
// CharSet is the base class for the classes StateSet and SymbolSet
// as for both the base types (State and TapeSymbol) are simply chars.
//======================================================================

#ifndef CharSet_h
#define CharSet_h


#if ( (__GNUG__ >= 4) && (__GNUC_MINOR__ >= 8) ) || \
    ( _MSC_VER >= 1800 )
  #define INITIALIZER_LIST_AVAILABLE
#endif

#ifdef INITIALIZER_LIST_AVAILABLE
  #include <initializer_list>
#endif
#include <iosfwd>
#include <set>
#include <string>


class CharSet: public std::set<char> {

  public:

    CharSet();
    CharSet(const CharSet &cs);
    CharSet(char ch);

    CharSet(std::string str); // format for str: '{' ch { ',' ch } '}'
      // ... where ch is a char literal, typically a letter

#ifdef INITIALIZER_LIST_AVAILABLE
    CharSet(initializer_list<char> il);
#endif

    virtual ~CharSet();

}; // CharSet

std::ostream &operator<<(std::ostream &os, const CharSet &ss);


#endif

// end of CharSet.h
//======================================================================

