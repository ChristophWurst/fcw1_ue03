// DFA.cpp:                                               HDO, 2006-2014
// -------
// Objects of class DFA represent deterministic finite automata.
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
#include "DFA.h"

#define DEBUG


static void printNeTable(StateSet S, bool ne[STATES][STATES]) {
  cout << "ne ";
  for (State s: S) {
    cout << s << " ";
  } // for
  cout << endl;
  // no range-based for possible as iterator itSi is necessary for inner loop
  for (StateSet::iterator itSi =  S.begin(); itSi != S.end(); itSi++) {
    State si = *itSi;
    cout << si << ": ";
    for (StateSet::iterator itSj =  S.begin(); itSj != itSi; itSj++) {
      State sj = *itSj;
      if (ne[si][sj])
        cout << "t ";
      else
        cout << "f ";
    } // for
    cout << endl;
  } // for
} // printNeTable


// --- implementation of class DFA ---


void DFA::initialize() {
  // initialize delta with undefined transitions only
  for (State s = 0; s < STATES; s++) {
    for (TapeSymbol sy = 0; sy < SYMBOLS; sy++) {
      delta[s][sy] = UNDEF_STATE;
    } // for
  } // for
} // DFA::initialize


void DFA::addTransition(State src, TapeSymbol sy, State dest) {
  if (delta[src][sy] != UNDEF_STATE)
    throw runtime_error("redefinition of transition");
  delta[src][sy] = dest;
} // DFA::addTransition


StateSet DFA::deltaAt(State src, TapeSymbol sy) const {
   State dest = delta[src][sy];
   if (dest == UNDEF_STATE)
     return StateSet(); // emtpy StateSet signals undefined transition
   else
     return StateSet(dest);
} // DFA::deltaAt


DFA::DFA() {
  initialize(); // delta only
} // DFA::DFA

DFA::DFA(const DFA &dfa)
: FA(dfa) {
  cout << "DFA::DFA(const DFA &dfa): copying " << sizeof(delta) << " bytes" << endl;
  memcpy(delta, dfa.delta, sizeof(delta));
} // DFA::DFA

DFA::DFA(const string &fileName) {
  initialize(); // delta only
  ifstream ifs(fileName);
  if (!ifs.good())
    throw invalid_argument("file \"" + fileName + "\" not found");
  initFromStream(ifs);
 } // DFA::DFA

DFA::DFA(const char *dfaStr) {
  initialize(); // delta only
  istringstream iss(string(dfaStr, strlen(dfaStr)));
  initFromStream(iss);
} // DFA::DFA


DFA::~DFA() {
  // nothing to do
} // DFA::~DFA


bool DFA::accepts(const TapeSymbol tape[]) const {

  int        i  = 0;         // index of first symbol
  TapeSymbol sy = tape[0];   // fetch first terminal symbol
  State      s  = s1;        // start state

  while (sy != EOT_SYMBOL) { // EOT = end of tape
#ifdef DEBUG
    cout << "state = " << s << ", symbol = " << sy << endl;
#endif
    s = delta[s][sy];
    if (!defined(s))
      return false;          // s undefined, no acceptance*/
    i++;
    sy = tape[i];            // fetch next symbol
  } // while

  return F.contains(s);      // accepted <==> s element of F

} // DFA::accepts


// State Minimization (cf. Asteroth/Baier, p. 270
// ------------------      and Hopcroft/Motwani/Ullmann, p. 171):

DFA DFA::minimalDfaOf() {

 cout << "DFA::minimalDfaOf {" << endl;

  // 1. "table filling algorithm"

  bool ne[STATES][STATES]; // non-equivalent = different states
  // initialize ne table
  for (State si = 0; si < STATES;  si++)
    for (State sj = 0; sj < STATES;  sj++)
      ne[si][sj] = false;
  // mark {final, nonfinal} pairs of states as non equal
  for (State f: F) {
    for (State s: S) {
      if ( (f != s) /*&& !F.contains(s)*/ ) {
        ne[f][s] = true;
        ne[s][f] = true;
      } // if
    } // for
  } // for
  // printNeTable(states, ne); // for debugging

  // compute (non)equal states
  bool anyChange = true;
  while (anyChange) {
    anyChange = false;
    for (StateSet::iterator itSi = S.begin(); itSi != S.end(); itSi++) {
      State si = *itSi;
      for (StateSet::iterator itSj = S.begin(); itSj != itSi; itSj++) {
        State sj = *itSj;
        if (!ne[si][sj]) {
          for (TapeSymbol sy: V) {
            State destSiSy = delta[si][sy];
            State destSjSy = delta[sj][sy];
            if ( (destSiSy != destSjSy) &&
                 ( !defined(destSiSy) ||
                   !defined(destSjSy) ||
                   ne[destSiSy][destSjSy] ) ) {
              ne[si][sj] = true;
              ne[sj][si] = true;
              anyChange  = true;
            } // if
          } // for
        } // if
      } // for
    } // for
  } // while
  printNeTable(S, ne);

  // 2. create a partition (set of blocks) from ne table

  SetOfStateSets partition;
  for (State si: S) {
    StateSet block = StateSet(si);
    for (State sj: S) {
      if ( (     si != sj  ) &&
           (!ne[ si ][ sj ]) )
        block.insert(sj);
    } // for
    partition.insert(block);
  } // for
  // cout << "partition = " << partition << endl; // for debugging

  // 3. construct transitions of minimal automaton within minDeltaMap

  StateSetAndSymbolToStateSetMapper minDeltaMap; // (src, sy) -> dest
  for (const StateSet &srcStateSet: partition) {
    for (TapeSymbol sy: V) {
      State    destState = delta[srcStateSet.anElement()][sy];
      StateSet destStateSet;
      for (const StateSet &ss: partition) {
        if (ss.contains(destState)) {
          destStateSet = ss;
          break;
        } // if
      } // for
      if (!destStateSet.empty())
        minDeltaMap[make_pair(srcStateSet, sy)] = destStateSet;
    } // for
  } // for

  // 4. initialize V for minimal DFA minDfa

  DFA minDfa;
  minDfa.V  = V;

  // 5. assign map minDeltaMap to delta array of minDfa

  StateSetToStateMapper mapper;
  for (/*StateSetAndSymbolToStateSetMapping*/ auto m: minDeltaMap) {
    StateSet   srcStateSet  = m.first.first;
    TapeSymbol sy           = m.first.second;
    StateSet destStateSet = m.second;
    cout << "minDfa.delta[" << srcStateSet << "][" << sy << "] = "
         << destStateSet << endl;
    minDfa.delta[mapper.newStateFor(srcStateSet)][sy] =
                 mapper.newStateFor(destStateSet);
  } // for

  // 6. map state sets to new states and compute s1 and F for minDfa

  minDfa.S = StateSet();
  minDfa.s1 = UNDEF_STATE;
  for (const StateSet &ss: partition) {
    if (ss.contains(s1)) {
      minDfa.s1 = mapper.newStateFor(ss);
      break;
    } // if
  } // for
  minDfa.F = StateSet();
  for (/*StateSetToStateMapping*/ auto m: mapper) {
    StateSet ss = m.first;
    State    s  = m.second;
    minDfa.S.insert(s);
    cout << "newStateFor(" << ss << ") = " << s << endl;
    for (State f: F) {
      if (ss.contains(f))
        minDfa.F.insert(s);
    } // for
  } // for

 cout << "} // DFA::minimalDfaOf" << endl << endl;
 return minDfa;

} // DFA::minimalDfaOf


// end of DFA.cpp
//======================================================================
