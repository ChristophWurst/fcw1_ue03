// NFA.h:                                                 HDO, 2006-2014
// -----
// Objects of class NFA represent nondeterministic finite automata.
//======================================================================

#ifndef NFA_h
#define NFA_h


#include <iosfwd>
#include <map>
#include <string>

#include "ObjectCounter.h"
#include "SymbolStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"


class NFA: public FA,
           private ObjectCounter<NFA> {

  private:

    void initialize();         // initialize delta only
    virtual void addTransition(State src, TapeSymbol sy, State dest);
    virtual StateSet   deltaAt(State src, TapeSymbol sy) const;

  public:

    // data components inherited from base class: S, V, s1, and F

    StateSet  delta[STATES][SYMBOLS]; // state transition table, alternatively
    // std::map< std::pair<State, TapeSymbol>, StateSet > deltaMap;

    NFA();                     // construct programmaticaly (later on)
    NFA(const NFA &nfa);
    NFA(const std::string &fileName); // construct from text file
    NFA(const char *nfaStr);   // construct from C string literal

    virtual ~NFA();

    bool accepts(const TapeSymbol tape[]) const; // uses backtracking via method:
    bool accepts(State s, const TapeSymbol tape[], int i) const;

    StateSet epsilonClosureOf(      State     src) const; // calls:
    StateSet epsilonClosureOf(const StateSet &src) const;

    StateSet allDestinationsFor(const StateSet &src, TapeSymbol sy) const;

    bool accepts2(const TapeSymbol tape[]) const; // uses StateSets

    DFA dfaOf() const;         // transformation: NFA => DFA

}; // NFA

// std::ostream &operator<<(std::ostream &os, const NFA &dfa); is not necessary as
// std::ostream &operator<<(std::ostream &os, const  FA & fa); is a smart one ;-)


#endif

// end of NFA.h
//======================================================================



