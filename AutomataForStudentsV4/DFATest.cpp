// DFATest.cpp:                                           HDO, 2006-2014
// -----------
// Test program for deterministic finite automata.
//======================================================================

#include <cstdio>  // for getchar only
#include <cstdlib> // for system only

#include <iostream>
#include <map>
#include <set>
#include <string>

using namespace std;

#include "SymbolStuff.h"
#include "StateStuff.h"
#include "FA.h"
#include "DFA.h"
#include "Moore.h"


// Activation (1) allows simple building via command line
//   * for GNU       use:  g++ -std=c++11 DFATest.cpp
//   * for Microsoft use:  cl DFATest.cpp
#if 1
  #include "CharSet.cpp"
  #include "SymbolStuff.cpp"
  #include "StateStuff.cpp"
  #include "Mappers.cpp"
  #include "FA.cpp"
  #include "DFA.cpp"
#endif


// recognition of simple identifers via hard coded automation:

bool acceptsIdentifier(const TapeSymbol tape[]) {

  int        i  = 0;       // index of first symbol
  TapeSymbol sy = tape[0]; // fetch first symbol
  State      s  = 'B';     // start state

  while (sy != EOT_SYMBOL) { // EOT = end of tape
    switch (s) {
      case 'B': if (sy == 'b')
                  s = 'R';
                else
                  return false; // invalid symbol, not accepted
                break;
      case 'R': if ( (sy == 'b') || (sy == 'z') )
                  s = 'R';
                else
                  return false; // invalid symbol, not accepted
                break;
      default:
                return false;   // undefined state
    } // switch
    i++;
    sy = tape[i];               // fetch next symbol
  } // while

  return s == 'R'; // accepted <==> s is an end state

} // acceptsIdentifier


int main(int argc, char *argv[]) {

  cout << "START: DFATest" << endl;
  cout << endl;

try {

// *** test case selection ***
#define ACCEPT_TESTCASE 1
// ***************************

  cout << "ACCEPT_TESTCASE " << ACCEPT_TESTCASE << endl << endl;

#if ACCEPT_TESTCASE == 1 // Identifier

#if 0 // programmatical grammar construction
  DFA idDfa;
  idDfa.S = StateSet("{B,R}");
  idDfa.V = SymbolSet("{b,z}");
  idDfa.delta['B']['b'] = 'R';
  idDfa.delta['R']['b'] = 'R';
  idDfa.delta['R']['z'] = 'R';
  idDfa.s1 = 'B';
  idDfa.F  = StateSet("{R}");
#endif
#if 0 // grammar construction from file
  DFA idDfa(string("IdDFA.txt"));
#endif
#if 1 // grammar construction from C string literal
  DFA idDfa(
    "-> B -> b R        \n\
     () R -> b R | z R     ");
#endif

  cout << idDfa << endl;

  cout << "idDfa.accepts(\"bzb\")     = " << idDfa.accepts("bzb")     << endl;
  cout << "idDfa.accepts(\"z\")       = " << idDfa.accepts("z")       << endl;
  cout << endl;

  cout << "acceptsIdentifier(\"bzb\") = " << acceptsIdentifier("bzb") << endl;
  cout << "acceptsIdentifier(\"z\")   = " << acceptsIdentifier("z")   << endl;
  cout << endl;

  cout << "Moore test" << endl;
    Moore moore(
    "-> B -> b R        \n\
     () R -> b R | z R     ");
    moore.transMap['b'] = 'c';
    moore.transMap['z'] = 'd';
  cout << "moore transform(\"bzzb\") => cddc    = " << moore.transformer("bzzb")     << endl;
  cout << "moore no transform(\"z\") => empty       = " << moore.transformer("z")       << endl;




#else // none of the ACCEPT_TESTCASEs above

  cerr << "ERROR: invalid ACCEPT_TESTCASE == " << ACCEPT_TESTCASE << endl;

#endif

// *** test case selection ***
#define MINIMIZATION_TESTCASE 1
// ***************************

  cout << "MINIMIZATION_TESTCASE " << MINIMIZATION_TESTCASE << endl << endl;

  DFA dfa;

#if MINIMIZATION_TESTCASE == 1 // see slides page 69

  dfa.S = StateSet("{S,A,B,C,E}");
  dfa.V = SymbolSet("{0,1}");
  dfa.delta['S']['0'] = 'A';
  dfa.delta['S']['1'] = 'B';
  dfa.delta['A']['0'] = 'E';
  dfa.delta['A']['1'] = 'B';
  dfa.delta['B']['0'] = 'C';
  dfa.delta['B']['1'] = 'B';
  dfa.delta['C']['0'] = 'E';
  dfa.delta['C']['1'] = 'S';
  dfa.delta['E']['0'] = 'E';
  dfa.delta['E']['1'] = 'E';
  dfa.s1 = 'S';
  dfa.F  = StateSet("{E}");

#else // none of the MINIMIZATION_TESTCASEs above

  cerr << "ERROR: invalid MINIMIZATION_TESTCASE == " << MINIMIZATION_TESTCASE << endl;

#endif

  cout << "DFA:" << endl << dfa << endl;
  cout << "writing automaton to DFA.gv" << endl;
  dfa.generateGraphVizFile("DFA.gv");

  DFA minDfa = dfa.minimalDfaOf();
  cout << "Minimal DFA:" << endl << minDfa << endl;

  cout << "writing minimal automaton to DFAmin.gv" << endl;
  minDfa.generateGraphVizFile("DFAmin.gv");

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  cout << "END DFATest" << endl;

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main


// end of DFATest.cpp
//======================================================================
