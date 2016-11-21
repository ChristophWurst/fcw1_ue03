// NFATest.cpp:                                           HDO, 2006-2014
// -----------
// Test program for nondeterministic finite automaton.
//======================================================================

#include <cstdio>  // for getchar only
#include <cstdlib> // for system only

#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

#include "SymbolStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"
#include "NFA.h"

// Activation (1) allows simple building via command line
//   * for GNU       use:  g++ -std=c++11 NFATest.cpp
//   * for Microsoft use:  cl NFATest.cpp
#if 1
  #include "CharSet.cpp"
  #include "SymbolStuff.cpp"
  #include "StateStuff.cpp"
  #include "Mappers.cpp"
  #include "FA.cpp"
  #include "DFA.cpp"
  #include "NFA.cpp"
#endif


// *** main function: test program for NFA ***

int main(int argc, char *argv[]) {

try {

  cout << "START: NFATest" << endl;
  cout << endl;


// *** test case selection ***
#define ACCEPT_TESTCASE 1
// ***************************

  cout << "ACCEPT_TESTCASE " << ACCEPT_TESTCASE << endl << endl;

  NFA nfa;

#if ACCEPT_TESTCASE == 1

#if 0 // programmatical grammar construction
  nfa.S = StateSet("{S,A,E}");
  nfa.V = SymbolSet("{0,1}");
  nfa.delta['S']['0'] = StateSet("{S,A}");
  nfa.delta['S']['1'] = StateSet("{S}");
  nfa.delta['A']['1'] = StateSet("{E}");
  nfa.s1 = 'S';
  nfa.F  = StateSet("{E}");
#endif
#if 0 // grammar construction from file
  nfa = NFA(string("NFA1.txt"));
#endif
#if 1 // grammar construction from C string literal
  nfa = NFA(
    "-> S -> 0 S | 0 A | 1 S   \n\
        A -> 1 E               \n\
     () E ->                     ");
#endif

  cout << nfa << endl;

  cout << "writing automaton to NFA1.gv" << endl;
  nfa.generateGraphVizFile("NFA1.gv");

  cout << "nfa.accepts (\"101\") = " << nfa.accepts ("101") << endl;
  cout << endl;
  cout << "nfa.accepts (\"011\") = " << nfa.accepts ("011") << endl;
  cout << endl;

  cout << "nfa.accepts2(\"101\") = " << nfa.accepts2("101") << endl;
  cout << endl;
  cout << "nfa.accepts2(\"011\") = " << nfa.accepts2("011") << endl;
  cout << endl;

#else // none of the ACCEPT_TESTCASEs above

  cerr << "ERROR: invalid ACCEPT_TESTCASE == " << ACCEPT_TESTCASE << endl;

#endif


// --- TESTCASES for NFA->DFA (subset construction) ---

// *** test case selection ***
#define CONVERSION_TESTCASE 1
// ***************************

  cout << "CONVERSION_TESTCASE = " << CONVERSION_TESTCASE << endl;
  cout << endl;

#if CONVERSION_TESTCASE == 1 // example for NFA motivation, see slides page 50

  nfa.S = StateSet("{S,A,B,C,R}");
  nfa.V = SymbolSet("{a,b,c}");
  nfa.delta['S']['a'] = StateSet("{S,A}");
  nfa.delta['S']['b'] = StateSet("{S,B}");
  nfa.delta['S']['c'] = StateSet("{S,C}");
  nfa.delta['A']['a'] = StateSet("{A,R}");
  nfa.delta['A']['b'] = StateSet("{A}");
  nfa.delta['A']['c'] = StateSet("{A}");
  nfa.delta['B']['a'] = StateSet("{B}");
  nfa.delta['B']['b'] = StateSet("{B,R}");
  nfa.delta['B']['c'] = StateSet("{B}");
  nfa.delta['C']['a'] = StateSet("{C}");
  nfa.delta['C']['b'] = StateSet("{C}");
  nfa.delta['C']['c'] = StateSet("{C,R}");
  nfa.s1 = 'S';
  nfa.F  = StateSet("{R}");

#else // none of the CONVERSION_TESTCASEs above

  cerr << "ERROR: invalid CONVERSION_TESTCASE == " << CONVERSION_TESTCASE << endl;

#endif

  cout << "NFA:" << endl << nfa << endl;
  DFA dfa = nfa.dfaOf();
  cout << "DFA:" << endl << dfa << endl;
  dfa.generateGraphVizFile("DfaOfNfa.gv");

  DFA minDfa = dfa.minimalDfaOf();
  cout << "minimal DFA:" << endl << minDfa << endl;
  dfa.generateGraphVizFile("DfaOfNfaMinimized.gv");

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  cout << "END NFATest" << endl;

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main


// end of NFATest.cpp
//======================================================================

