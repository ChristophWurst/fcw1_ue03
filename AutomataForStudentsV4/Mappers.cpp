// Mappers.cpp:                                           HDO, 2006-2014
// -----------
// Types to map state sets (and symbols) to new states (or state sets).
//======================================================================

#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

#include "Mappers.h"


StateSetToStateMapper::StateSetToStateMapper() {
  nextState = 'Z';
} // StateSetToStateMapper::StateSetToStateMapper


StateSetToStateMapper::~StateSetToStateMapper() {
  // nothing to do
} // StateSetToStateMapper::~StateSetToStateMapper


State StateSetToStateMapper::newStateFor(const StateSet &ss) {
  State s = this->operator[](ss);
  if (s == 0) { // new mapping necessary
    if ( (ss.size() == 1) && // ss has only one element ...
         !targetStates.contains(*(ss.begin())) ) { // ... not used as target yet
      s = *(ss.begin()); // map ss == {s} to s
    } else {
      do {
        s = nextState;
        nextState--;
        switch (s) {
          case 'A': nextState = 'z';
                    break;
          case 'a': nextState = '9';
                    break;
          case '/': throw overflow_error("mapper overflow, letters and digits exhausted");
        } // switch
      } while(targetStates.contains(s));
    } // if
    this->operator[](ss) = s;
    targetStates.insert(s);
  } // if
  return s;
} // StateSetToStateMapper::newStateFor


ostream &operator<<(ostream &os, const StateSetToStateMapper &mapper) {
  typedef pair<const StateSet, State> Mapping;
  for (const Mapping &m: mapper) {
    cout << m.first << " -> " << m.second << endl;
  } // for
  return os;
} // operator<<


// === test ============================================================

#if 0

#include "CharSet.cpp"
#include "StateStuff.cpp"

int main(int argc, char *argv[]) {
try {

  cout << "START: StateSetToStateMapper" << endl;
  cout << endl;

  StateSetToStateMapper mapper;

  cout << "mapper: " << endl << mapper << endl;

  cout << "mapper.newStateFor(StateSet()) = " <<
           mapper.newStateFor(StateSet()) << endl;
  cout << "mapper.newStateFor(StateSet(\"{ a }\")) = " <<
           mapper.newStateFor(StateSet("{ a }")) << endl;
  cout << "mapper.newStateFor(StateSet(\"{ b, c }\")) = " <<
           mapper.newStateFor(StateSet("{ b, c }")) << endl;
  cout << "mapper.newStateFor(StateSet()) = " <<
           mapper.newStateFor(StateSet()) << endl;
  cout << endl;

  cout << "mapper: " << endl << mapper << endl;

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

// end of StateSetToStateMapper.cpp
//======================================================================
