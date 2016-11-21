// StateStuff.cpp:                                        HDO, 2006-2014
// --------------
// Type State, classes StateSet, and SetOfStateStets,
// representing states, sets of symbols, and sets of sets of
// symbols for various types of automata.
//======================================================================

#include <iostream>
#include <string>

#include <algorithm>
#include <iterator>
#include <set>

using namespace std;

#include "StateStuff.h"


bool defined(State s) {
  return s != UNDEF_STATE;
} // defined


// --- implementation of class StateSet ---

StateSet::StateSet() {
  // nothing to do, empty state set per default
} // StateSet::StateSet


StateSet::StateSet(const StateSet &ss)
: CharSet(ss) {
  // nothing to do
} // StateSet::StateSet


StateSet::StateSet(State s) {
  insert(s);
} // StateSet::StateSet


StateSet::StateSet(string str) // format for str: '{' s { ',' s } '}'
: CharSet(str) {
  // nothing to do, base class constructor does the whole work
} // StateSet::StateSet


#ifdef INITIALIZER_LIST_AVAILABLE
StateSet::StateSet(initializer_list<State> il)
: CharSet(il) {
  // nothing to do, base class constructor does the whole work
} // StateSet::StateSet
#endif


StateSet::~StateSet() {
  // nothing to do
} // StateSet::~StateSet


bool StateSet::contains(State s) const {
  return find(s) != end();
} // StateSet::contains


void StateSet::remove(State s) {
  StateSet::iterator it = find(s);
  if (it != end())
    erase(it);
} // StateSet::remove

State StateSet::getAndRemove() {
  StateSet::iterator it = begin();
  State element = *it;
  erase(it);
  return element;
} // StateSet::getAndRemove


State StateSet::anElement() const {
  return *begin();
} // StateSet::anElement


StateSet intersection(const StateSet &ss1, const StateSet &ss2) {
  StateSet ss;
  set_intersection(ss1.begin(), ss1.end(),
                   ss2.begin(), ss2.end(),
                   inserter(ss, ss.begin()));
  return ss;
} // intersection


bool defined(const StateSet &ss) {
  return ss.size() > 0;
} // defined


// --- implementation of class SetOfStateSets ---

SetOfStateSets::SetOfStateSets() {
  // nothing to do, empty set of StateSets per default
} // SetOfStateSets::SetOfStateSets


SetOfStateSets::~SetOfStateSets() {
  // nothing to do
} // SetOfStateSets::~SetOfStateSets


bool SetOfStateSets::contains(const StateSet &ss) const {
  return find(ss) != end();
} // SetOfStateSets::contains


StateSet SetOfStateSets::getAndRemove() {
  SetOfStateSets::iterator it = begin();
  StateSet element = *it;
  erase(it);
  return element;
} // SetOfStateSets::getAndRemove


ostream &operator<<(ostream &os, const SetOfStateSets &soss) {
  os << "{ ";
  bool first = true;
  for (const StateSet &ss: soss) {
    if (!first)
      os << ", ";
    os << ss;
    first = false;
  } // for
  os << " }";
  return os;
} // operator<<


// === test ============================================================

#if 0

#include "CharSet.cpp"

int main(int argc, char *argv[]) {
try {

  cout << "START: StateStuff" << endl;
  cout << endl;

  StateSet emptySet;
  cout << "emptySet = " << emptySet << endl;

  StateSet abcSet("{ a, b, c }");
  cout << "abcSet   = " << abcSet   << endl;

  cout << "abcSet.contains('a') = " <<
           abcSet.contains('a') << endl;
  cout << "abcSet.contains('x') = " <<
           abcSet.contains('x') << endl;

  cout << "abcSet.getAndRemove() = " <<
           abcSet.getAndRemove() << endl;
  cout << "abcSet   = " << abcSet   << endl;

  cout << "abcSet.anElement() = " <<
           abcSet.anElement() << endl;
  cout << "abcSet   = " << abcSet   << endl;

  cout << "intersection(StateSet(\"{ a, b, c }\"), StateSet(\"{ b, c, d }\")) = " <<
           intersection(StateSet("{ a, b, c }"), StateSet("{ b, c, d }")) << endl;

  // StateSet error1("a }");
  // StateSet error2("{ b");

  SetOfStateSets emptySetOfSets;
  cout << "emptySetOfSets = " << emptySetOfSets << endl;

  SetOfStateSets abcSetPartition;
  abcSetPartition.insert(StateSet("{ a }"));
  abcSetPartition.insert(StateSet("{ b, c }"));
  cout << "abcSetPartition = " << abcSetPartition << endl;

  cout << "abcSetPartition.contains(StateSet(\"{ a }\")) = " <<
           abcSetPartition.contains(StateSet("{ a }")) << endl;
  cout << "abcSetPartition.contains(StateSet(\"{ c }\")) = " <<
           abcSetPartition.contains(StateSet("{ c }")) << endl;

  cout << "abcSetPartition.getAndRemove() = " <<
           abcSetPartition.getAndRemove() << endl;
  cout << "abcSetPartition = " << abcSetPartition   << endl;

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

// end of StateStuff.cpp
//======================================================================
