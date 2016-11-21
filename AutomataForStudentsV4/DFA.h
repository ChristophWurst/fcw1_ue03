// DFA.h:                                                 HDO, 2006-2014
// -----
// Objects of class DFA represent deterministic finite automata.
//======================================================================

#ifndef DFA_h
#define DFA_h


#include <iosfwd>
#include <map>
#include <string>


#include "ObjectCounter.h"
#include "SymbolStuff.h"
#include "StateStuff.h"
#include "FA.h"


class DFA: public  FA,
           private ObjectCounter<DFA> {

  private:

    void initialize();         // initialize delta only
    virtual void addTransition(State src, TapeSymbol sy, State dest);
    virtual StateSet   deltaAt(State src, TapeSymbol sy) const;

  public:

    // data components inherited from base class: S, V, s1, and F

    State delta[STATES][SYMBOLS]; // state transition table, alternatively:
    // std::map< pair<State, TapeSymbol>, State > deltaMap;

    DFA();                     // construct programmatically (later on)
    DFA(const DFA &dfa);
    DFA(const std::string &fileName); // construct from text file
    DFA(const char *dfaStr);   // construct from C string literal

    virtual ~DFA();

    bool accepts(const TapeSymbol tape[]) const;

    DFA minimalDfaOf();        // minimization: DFA => minimal DFA

}; // DFA

// std::ostream &operator<<(std::ostream &os, const DFA &dfa); not necessary as
// std::ostream &operator<<(std::ostream &os, const  FA & fa); is a smart one ;-)


#endif

// end of DFA.h
//======================================================================



