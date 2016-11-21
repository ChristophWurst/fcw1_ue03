// FA.h:                                                  HDO, 2011-2014
// ----
// Class FA is the abstract base class for finite automata DFA and NFA.
// Defined mainly to provide common methods for input and output.
//======================================================================

#ifndef FA_h
#define FA_h


#include <iosfwd>
#include <string>

#include "SymbolStuff.h"
#include "StateStuff.h"


class FA {                            // abstract base class for DFA and NFA

  friend
    std::ostream &operator<<(std::ostream &os, const FA &fa); // for deltaAt

  protected:

    void initFromStream(std::istream &is);  // read FA from stream, synax:
      // -> S -> a B | ...  leading -> flags the start sate
      //    B -> c E | ...  meaning: delta[B][c] = E
      // () E -> ...        leading () flags final state(s)
      // also allowed: -> () S -> ... for start state being final

    virtual void addTransition(State src, TapeSymbol sy, State dest) = 0;
    //  called from initFromStream only

    virtual StateSet   deltaAt(State src, TapeSymbol sy) const = 0;
    //  called from operator<< only

  public:

    StateSet  S;                      // set of states ("nonterminals")
    SymbolSet V;                      // set of tape symbols ("terminals")
    // delta to be defined in classes DFA and NFA derived from FA
    State     s1;                     // start state, an element of S
    StateSet  F;                      // set of final states, a subset of S

    FA();

    virtual ~FA();

    virtual bool accepts(const TapeSymbol tape[]) const = 0;

    void generateGraphVizFile(const std::string &file); // use file type .gv
      // see www.graphviz.org for software and details

}; // FA

std::ostream &operator<<(std::ostream &os, const FA &fa);


#endif

// end of FA.h
//======================================================================
