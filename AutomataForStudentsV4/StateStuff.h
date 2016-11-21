// StateStuff.h:                                          HDO, 2006-2014
// ------------
// Type State, classes StateSet, and SetOfStateStets,
// representing states, sets of symbols, and sets of sets of
// symbols for various types of automata.
//======================================================================

#ifndef StateStuff_h
#define StateStuff_h


#include <iosfwd>
#include <set>
#include <string>

#include "ObjectCounter.h"
#include "CharSet.h"


#define STATES       127  // max. number of states (only chars)
#define UNDEF_STATE  0    // undefined state


typedef char State;

bool defined(State s);


class StateSet: public  CharSet,  // equivalent to std::set<State>
                private ObjectCounter<StateSet> {

  public:

    StateSet();
    StateSet(const StateSet &ss);
    StateSet(State s);

    StateSet(std::string str); // format for str: '{' s { ',' s } '}'
      // where s is a State, typically a letter

#ifdef INITIALIZER_LIST_AVAILABLE
    StateSet(initializer_list<State> il);
#endif

    virtual ~StateSet();

    bool contains(State s) const;

    void remove(State s);

    State getAndRemove();

    State anElement() const;

}; // StateSet

StateSet intersection(const StateSet &ss1, const StateSet &ss2);

// std::ostream &operator<<(std::ostream &os, const StateSet &ss): not necessary as
// std::ostream &operator<<(std::ostream &os, const CharSet  &ss) works fine

bool defined(const StateSet &ss);


class SetOfStateSets: public  std::set<StateSet>,
                      private ObjectCounter<SetOfStateSets> {

  public:

    SetOfStateSets();

    virtual ~SetOfStateSets();

    bool contains(const StateSet &ss) const;

    StateSet getAndRemove();

}; // SetOfStateSets

std::ostream &operator<<(std::ostream &os, const SetOfStateSets &soss);


#endif

// end of StateStuff.h
//======================================================================

