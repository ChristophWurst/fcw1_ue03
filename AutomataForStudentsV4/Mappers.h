// Mappers.h:                                             HDO, 2006-2014
// ---------
// Types to map state sets (and symbols) to new states (or state sets).
//======================================================================

#ifndef Mappers_h
#define Mappers_h


#include <iosfwd>
#include <map>

#include "ObjectCounter.h"
#include "SymbolStuff.h"
#include "StateStuff.h"


typedef std::pair<const StateSet, State>
  StateSetToStateMapping;

class StateSetToStateMapper: public  std::map<StateSet, State>,
                             private ObjectCounter<StateSetToStateMapper> {

  private:

    StateSet targetStates; // used target states
    State    nextState;    // next State for StateSet: Z, Y, X, ...

  public:

    StateSetToStateMapper();

    virtual ~StateSetToStateMapper();

    State newStateFor(const StateSet &ss);

}; // StateSetToStateMapper

std::ostream &operator<<(std::ostream &os, const StateSetToStateMapper &mapper);


typedef std::pair<const std::pair<StateSet, TapeSymbol>, StateSet>
  StateSetAndSymbolToStateSetMapping;

typedef std::map<       std::pair<StateSet, TapeSymbol>, StateSet>
  StateSetAndSymbolToStateSetMapper;


#endif

// end of Mappers.h
//======================================================================
