// FA.cpp:                                                HDO, 2011-2014
// ------
// Class FA is the abstract base class for finite automata DFA and NFA.
// Defined mainly to provide common methods for input and output.
//======================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>

using namespace std;

#include "SymbolStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"


static string stringOf(int i) {
  ostringstream is;
  is << i;
  return is.str();
} // stringOf


static string stringOf(TapeSymbol sy) {
  switch (sy) {
    case EOT_SYMBOL:
      return "EOT";
    case EPSILON:
      return "EPSILON";
    default:
      return string(1, sy);
  } // switch
} // stringOf


static bool isNfa(const FA &fa) {
  return (typeid(NFA)   ==   typeid(fa)) ||
         (typeid(NFA).before(typeid(fa)));
} // isNfa


static string messageOf(int lnr, const string &msg) {
  return "error in line " + stringOf(lnr) + ": " + msg;
} // messageOf


void FA::initFromStream(istream &is) {
  string line, sy, state, arrowSy, destState;
  bool isStartState, isFinalState;
  s1 = UNDEF_STATE;
  int lnr = 0;
  while (!is.eof()) {
    line = "";
    getline(is, line);
    if (is.fail())
      break;
    lnr++;
    istringstream ls(line);      // line stream
    sy = "";
    ls >> sy;
    if (sy == "" || sy.substr(0, 2) == "//") // skip empty or comment line
      continue;
    isStartState = (sy == "->");
    isFinalState = (sy == "()");
    if (sy == "->()")            // ->() indicates start state being final
      isStartState = isFinalState = true;
    if (isStartState) {
      if (!isFinalState) {
        ls >> state;
        if (state == "()") {     // -> () also allowed for start & final state
          isFinalState = true;
          ls >> state;
        } // if
      } else // isStartState && isFinalState
        ls >> state;
    } else if (isFinalState)
      ls >> state;
    else // sy is already a state
      state = sy;
    ls >> arrowSy;
    if (arrowSy != "->")
      throw runtime_error(messageOf(lnr, "-> missing"));
    if (isStartState) {
      if (s1 != UNDEF_STATE)
        throw runtime_error(messageOf(lnr, "redef. of start state"));
      s1 = state[0];
    } // if
    if (isFinalState)
      F.insert(state[0]);
    S.insert(state[0]);
    while (!ls.eof()) {
      sy = "";                   // necessary for correct end of line handling
      ls >> sy;                  // terminal or |
      if (sy == "")
        break;
      if (sy == "|") {
        sy = "";                 // see above
        ls >> sy;                // terminal
        if (sy == "")
          throw runtime_error(messageOf(lnr, "no symbol for transition from " + state));
      } // if
      if ( (sy[0] == EPSILON) || (sy == "EPS") || (sy == "EPSILON") )
        sy = string(1, EPSILON);
      else
        V.insert(sy[0]);
      ls >> destState;
      if (destState == "|")
        throw runtime_error(messageOf(lnr, "dest. state missing for symbol " + sy));
      S.insert(destState[0]);
      try {
        addTransition(state[0], sy[0], destState[0]);
      } catch (...) {
        throw runtime_error(messageOf(lnr, "redef. of dest. state for symbol " + sy));
      } // catch
    } // while
  } // while
  if (s1 == UNDEF_STATE)
    throw runtime_error(messageOf(lnr, "no start state defined"));
  if (F.size() == 0)
    throw runtime_error(messageOf(lnr, "no final state(s) defined"));
} // initFromStream


FA::FA() {
  // nothing to do
} // FA::FA


FA::~FA() {
  // nothing to do
} // FA::~FA


void FA::generateGraphVizFile(const string &file) {
  ofstream ofs(file);
  if (!ofs.good())
    throw runtime_error("error in opening output file \"" + file + "\"");
  ofs << "digraph finite_state_machine {" << endl;
  ofs << endl;
  ofs << "  rankdir = LR;" << endl;
  ofs << endl;
  ofs << "  node [shape = doublecircle];";
  for (State f: F) {
    ofs << " " << f;
  } // for
  ofs << ";" << endl;
  ofs << "  node [shape = circle];" << endl;
  ofs << endl;
  ofs << "  null [shape = plaintext label=\"\"]" << endl;
  ofs << "  null -> " << s1 << ";" << endl;
  ofs << endl;
  SymbolSet Vtmp = V;
  if (isNfa(*this))
    Vtmp.insert(EPSILON);        // trick to respect epsilon transitions
  for (State s: S) {
    for (TapeSymbol sy: Vtmp) {
      StateSet destSs = deltaAt(s, sy);
      if (defined(destSs)) {
        for (State dest: destSs) {
          ofs << "  " << s << " -> " << dest <<
                  " [label = \"" << stringOf(sy) << "\"]; " << endl;
        } // for
      } //if
    } // for
  } // for
  ofs << endl;
  ofs << "}" << endl;
  ofs.close();
} // FA::generateGraphVizFile


ostream &operator<<(ostream &os, const FA &fa) {

#if (1) // new version: writes fa in the format used by initFromFile

  StateSet  Fcheck = fa.F;
  SymbolSet Vtmp   = fa.V;
  if (isNfa(fa))
    Vtmp.insert(EPSILON);        // trick to respect epsilon transitions
  for (State s: fa.S) {
    SymbolSet::const_iterator syIt = Vtmp.begin();
    while (syIt != Vtmp.end()) {
      TapeSymbol sy = *syIt;
      StateSet destSs = fa.deltaAt(s, sy);
      if (defined(destSs)) {
        if (s == fa.s1)
          os << "-> ";           // mark start state
        if (fa.F.contains(s))
          os << "() ";           // mark final state
        if ((s != fa.s1) && !fa.F.contains(s))
          os << "   " ;          // normal state, no mark
        os << s << " -> ";
        bool first = true;
        for (State destS: destSs) {
          if (!first)
            os << " | ";
          first = false;
          os << stringOf(sy) << " " << destS;
        } // for
        Fcheck.remove(s);
        syIt++;
        while (syIt != Vtmp.end()) {
          sy = *syIt;
          destSs = fa.deltaAt(s, sy);
          if (defined(destSs)) {
            for (State destS: destSs) {
              os << " | " << stringOf(sy) << " " << destS;
            } // for
          } // if
          syIt++;
        } // while
        os << endl;
      } else // !defined
        syIt++;
    } // while
  } // for
  for (State s: Fcheck) {
    os << "() " << s << endl;
  } // for

#else // old version, writes delta suitable for programmatical init.

  os << "S  = " << fa.S  << endl;
  os << "V  = " << fa.V  << endl;
  os << "s1 = " << fa.s1 << endl;
  os << "F  = " << fa.F  << endl;
  SymbolSet Vtmp = fa.V;
  if (isNfa(fa))
    Vtmp.insert(EPS);            // trick to respect EPS transitions
  for (State s: fa.S) {
    for (TapeSymbol sy: Vtmp) {
      StateSet destSs = fa.deltaAt(s, sy);
      if (defined(destSs))      // defined transitions only
        os << "delta[" << s << "][" << stringOf(sy) << "] = "
           << destSs << endl;
    } // for
  } // for

#endif

  os << endl;
  return os;
} // operator<<


// end of FA.cpp
//======================================================================
