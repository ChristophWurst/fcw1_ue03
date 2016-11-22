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
#include "SequenceStuff.h"
#include "Vocabulary.h"
#include "Grammar.h"

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
  #include "SequenceStuff.cpp"
  #include "Grammar.cpp"
#endif


NFA *nfaOf(const Grammar *g) {
    if (!g->isRegular()) {
        cout << "No Regular Grammar" << endl;
        return new NFA();
    }
    map<string,string> states;
    for (auto e : g->rules) {
        bool isEndState = false;
        // add state from NTSymbol
        states[e.first->name] = e.first->name + " -> ";
        int counter = 0;
        for (auto f : e.second) {
            std::ostringstream stream;
            stream << *f;
            if (f->hasTerminalsOnly()) {
                // add State to transition to TSymbol
                stream << " " << e.first->name;
                if (!isEndState) {
                    // add () to mark end state
                    isEndState = true;
                    states[e.first->name] = "() " + states[e.first->name];
                }
            }
            states[e.first->name] += stream.str() + (counter < e.second.size() - 1 ? " | " : "");
            counter++;
        }
        // mark start state
        if (e.first == g->root) {
            states[e.first->name] = "-> " + states[e.first->name];
        }
    }
    // add linebreak
    for (auto e : states) {
        states[e.first] = e.second + " \n";
    }
    // create nfaString
    string nfaString = "";
    for (auto e : states) {
        nfaString += e.second;
    }
    return new NFA(nfaString.c_str());
}

Grammar *grammarOf(const NFA *nfa) {
    string grammarString = "G(" + string(1, nfa->s1) + "):\n";
    cout << grammarString << endl;
    return nullptr;
}

// *** main function: test program for NFA ***

int main() {

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

  // try to accept sequences
  cout << "nfa.accepts (\"bbbbbbbbc\") = " << nfa.accepts ("bbbbbbbbc") << endl;
  cout << endl;
  cout << "nfa.accepts (\"cbbbbbbbbc\") = " << nfa.accepts ("cbbbbbbbbc") << endl;
  cout << endl;
  cout << "nfa.accepts (\"aba\") = " << nfa.accepts ("aba") << endl;
  cout << endl;
  cout << "nfa.accepts (\"ab\") = " << nfa.accepts ("ab") << endl;
  cout << endl;
  cout << "nfa.accepts (\"bbbbbbbbc\") = " << nfa.accepts2 ("bbbbbbbbc") << endl;
  cout << endl;
  cout << "nfa.accepts (\"cbbbbbbbbc\") = " << nfa.accepts2 ("cbbbbbbbbc") << endl;
  cout << endl;
  cout << "nfa.accepts (\"caaaaaaaac\") = " << nfa.accepts2 ("caaaaaaaac") << endl;
  cout << endl;
  cout << "nfa.accepts (\"aba\") = " << nfa.accepts2 ("aba") << endl;
  cout << endl;
  cout << "nfa.accepts (\"ab\") = " << nfa.accepts2 ("ab") << endl;
  cout << endl;

  DFA dfa = nfa.dfaOf();
  cout << "DFA:" << endl << dfa << endl;
  dfa.generateGraphVizFile("DfaOfNfa.gv");

  DFA minDfa = dfa.minimalDfaOf();
  cout << "minimal DFA:" << endl << minDfa << endl;
  dfa.generateGraphVizFile("DfaOfNfaMinimized.gv");

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  auto g = new Grammar(
    "G(S):              \n\
     S -> a | a B | c C \n\
     C -> a B           \n\
     B -> b                ");

//  auto g = new Grammar(
//    "G(A):              \n\
//     A -> a A | b       ");

  NFA* myNfa = nfaOf(g);
  cout << "myNfa.accepts (\"a\") = " << myNfa->accepts("a") << endl;
  cout << "MyGrammar: " << endl;
  cout << *g << endl;
  cout << "MyNfa: " << endl;
  cout << *myNfa << endl;
  g = grammarOf(myNfa);
  cout << "MyGrammar: " << endl;
  cout << g << endl;
  delete g;
  delete myNfa;

  cout << "END NFATest" << endl;

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main


// end of NFATest.cpp
//======================================================================

