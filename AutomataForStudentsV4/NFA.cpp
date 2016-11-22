// NFA.cpp:                                               HDO, 2006-2014
// -------
// Objects of class NFA represent nondeterministic finite automata.
//======================================================================

#include <cstring>

#include <iostream>
#include <fstream>
#include <map>
#include <sstream>

using namespace std;

#include "SymbolStuff.h"
#include "StateStuff.h"
#include "Mappers.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"

#undef DEBUG


void NFA::initialize() {
  // initialize delta with undefined transitions only
  for (State s = 0; s < STATES; s++) {
    for (TapeSymbol sy = 0; sy < SYMBOLS; sy++) { // includes EPS (== '�')
      delta[s][sy] = StateSet(); // empty set, {}
    } // for
  } // for
} //  NFA::initialize


void NFA::addTransition(State src, TapeSymbol sy, State dest) {
  delta[src][sy].insert(dest);
} // NFA::addTransition


StateSet NFA::deltaAt(State src, TapeSymbol sy) const {
  return delta[src][sy];
} // NFA::deltaAt


// --- implementation of class NFA ---


NFA::NFA() {
  initialize(); // delta only
} // NFA::NFA

NFA::NFA(const NFA &nfa)
: FA(nfa) {
  for (State s = 0; s < STATES; s++) {
    for (TapeSymbol sy = 0; sy < SYMBOLS; sy++) { // includes EPS (== '�')
      delta[s][sy] = nfa.delta[s][sy];
    } // for
  } // for
} // NFA::NFA

NFA::NFA(const string &fileName) {
  initialize(); // delta only
  ifstream ifs(fileName);
  if (!ifs.good())
    throw invalid_argument("file \"" + fileName + "\" not found");
  initFromStream(ifs);
 } // NFA::NFA

NFA::NFA(const char *nfaStr) {
  initialize(); // delta only
  istringstream iss(string(nfaStr, strlen(nfaStr)));
  initFromStream(iss);
} // NFA::NFA


NFA::~NFA() {
  // nothing to do
} // ~NFA::NFA


// NFA::accepts: uses backtracking to simulate nondeterminism
//-------------

bool NFA::accepts(const TapeSymbol tape[]) const {
  int numCalls =0;
  auto result = accepts(s1, tape, 0, numCalls);
  cout << "Number of Calls in accepts: " << numCalls << endl;
  return result;
} // NFA::accepts


bool NFA::accepts(State s, const TapeSymbol tape[], int i, int &numCalls) const {
  numCalls++;
  for (State epsDest: delta[s][EPSILON]) {
    if ( accepts(epsDest, tape, i, numCalls) ) // recursive call
      return true;
  } // for
  TapeSymbol sy = tape[i];
  if (sy == EOT_SYMBOL)   // end of tape?
    return F.contains(s); // accepted <==> s element F
  StateSet syDestStates = delta[s][sy];
  if (syDestStates.empty())
    return false; // not accepted as no next state defined
  for (State syDest: syDestStates) {
    if ( accepts(syDest, tape, i + 1, numCalls) ) // recursive call
      return true;
  } //for
  return false;   /*not accepted as no call succeeded*/
} // NFA::accepts


// NFA::epsilonClosureOf (cf. Aho/Sethi/Ullman, p. 119):
//----------------------

StateSet NFA::epsilonClosureOf(State src) const {
  return epsilonClosureOf(StateSet(src));
} // NFA::epsilonClosureOf

StateSet NFA::epsilonClosureOf(const StateSet &src) const {
  StateSet ec  = src; // initialize epsilon closure
  StateSet stc = src; // initialize states to check
  while (!stc.empty()) {
    StateSet dest = delta[stc.getAndRemove()][EPSILON];
    for (State ds: dest) {
      auto ir = ec.insert(ds);
      if (ir.second) // new ds inserted into ec
        stc.insert(ds);
    } // for
  } // while
  return ec;
} // NFA::epsilonClosureOf


// NFA::allDestinationsFor (function move from Aho/Sethi/Ullman, p. 118):
//------------------------

StateSet NFA::allDestinationsFor(const StateSet &src, TapeSymbol sy) const {
  StateSet ad;
  for (State ss: src) {
    for (State ds: delta[ss][sy]) {
      ad.insert(ds);
    } // for
  } // for
  return ad;
} // NFA::allDestinationsFor


// NFA::accepts2: tracing of state sets to simulate determinism
//--------------

bool NFA::accepts2(const TapeSymbol tape[]) const {
  int numberOfStates = 0;
  int        i  = 0;         // index of first symbol
  TapeSymbol sy = tape[0];   // fetch first symbol
  StateSet   ss = epsilonClosureOf(StateSet(s1));

  while (sy != EOT_SYMBOL) { // EOT = end of tape
    StateSet ad = allDestinationsFor(ss, sy);
    if (ad.empty()) {
      cout << "Number of states considered in accepts2: no acceptance" << endl;
      return false;          // undefined, no acceptance
    }
    ss = epsilonClosureOf(ad);
    numberOfStates += ss.size();
    i++;
    sy = tape[i];
  } // while

  cout << "Number of states considered in accepts2: " << numberOfStates << endl;
  return !intersection(ss, F).empty(); // accepted <==> intersection(ss,  F) != {}

} // NFA::accepts2


// NFA::dfaOf (cf. Aho/Sethi/Ullman, p. 118):
//-----------

DFA NFA::dfaOf() const {

 cout << "NFA::dfaOf {" << endl;

  // 1. construct DFA within a map

  StateSetAndSymbolToStateSetMapper dfaDeltaMap; // (src, sy) -> dest
  SetOfStateSets                    dfaStateSets;
  SetOfStateSets                    unmarkedStateSets;

  StateSet startStateSet = epsilonClosureOf(s1);
  dfaStateSets     .insert(startStateSet);
  unmarkedStateSets.insert(startStateSet);
  while (!unmarkedStateSets.empty()) {
    StateSet srcStateSet = unmarkedStateSets.getAndRemove();
    for (TapeSymbol sy: V) {
      StateSet destStateSet =
        epsilonClosureOf(allDestinationsFor(srcStateSet, sy));
      if (!destStateSet.empty()) {
        if (!dfaStateSets.contains(destStateSet)) {
          dfaStateSets     .insert(destStateSet);
          unmarkedStateSets.insert(destStateSet);
        } // if
        dfaDeltaMap[make_pair(srcStateSet, sy)] = destStateSet;
      } // if
    } // for
  } // while

  // 2. initialize DFA

  DFA dfa;
  dfa.V  = V;

  // 3. assign dfaDeltaMap to dfa.delta, mapping StateSets to new States

  StateSetToStateMapper mapper;
  for (/*StateSetAndSymbolToStateSetMapping*/ auto m: dfaDeltaMap) {
    StateSet   srcStateSet  = m.first.first;
    TapeSymbol sy           = m.first.second;
    StateSet   destStateSet = m.second;
    cout << "dfa.delta[" << srcStateSet << "][" << sy << "] = "
         << destStateSet << endl;
    dfa.delta[mapper.newStateFor(srcStateSet)][sy] =
              mapper.newStateFor(destStateSet);
  } // for

  // 4. compute dfa.s1, dfa.S and dfa.F

  dfa.s1 = mapper.newStateFor(epsilonClosureOf(s1));
  dfa.S  = StateSet();
  for (/*StateSetToStateMapping*/ auto m: mapper) {
    StateSet ss = m.first;
    State    s  = m.second;
    dfa.S.insert(s);
    cout << "newStateFor(" << ss << ") = " << s << endl;
    for (State f: F) {
      if (ss.contains(f))
        dfa.F.insert(s);
    } // for
  } // for

  cout << "} // NFA::dfaOf" << endl << endl;
  return dfa;

} // NFA::dfaOf


// end of NFA.cpp
//======================================================================


