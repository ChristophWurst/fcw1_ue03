// Grammar.cpp:                                           HDO, 2004-2015
// -----------
// Objects of class Grammar represent context free grammars.
// =====================================================================

#include <cstdarg>
#include <cstddef>
#include <cstring>

#include <iostream>
#include <fstream>
#include <queue>
#include <stdexcept>
#include <sstream>

using namespace std;

#include "Grammar.h"


#ifndef CHECK_FUNCTION_DEFINED
#define CHECK_FUNCTION_DEFINED
static inline void check(void *ptr, const char *errMsg) {
  if (ptr == nullptr)
    throw invalid_argument(errMsg);
} // check
#endif


void Grammar::initialize(NTSymbol *root) {
  vNtDelDefined = false;
  vNtDel.clear();
  this->root = root;
  insertIntoVNt(root);
  // rest of vNt and vT filled by rest of constructor or via addRule
} // Grammar::initialize


void Grammar::readGrammar(istream &is) {
  string line, sy, rootNt, nt, ntSy, arrowSy;
  bool firstNonEmptyLine;
  unordered_map<string, NTSymbol *> ntMap;
  unordered_map<string,  TSymbol *>  tMap;

  SymbolPool *sp = SymbolPool::getInstance();

  // 1. read file and map nonterminals
  firstNonEmptyLine = true;
  while (!is.eof()) {
    line = "";
    getline(is, line);
    if (is.fail())
      break;
    sy = "";
    istringstream(line) >> sy;
    if (sy == "" || sy.substr(0, 2) == "//") // skip empty or comment line
      continue;
    if (sy.substr(0, 3) == "---") // start of opt. symbol information: skip
      break;
    if (firstNonEmptyLine) {     // sy should look like "G(...):"
      firstNonEmptyLine = false;
      if ( (sy.substr(0,               2) != "G(") ||
           (sy.substr(sy.length() - 2, 2) != "):") )
        throw runtime_error("grammar does not start with \"G(...):\"");
      rootNt = sy.substr(2, sy.length() - 4);
      if ( (rootNt == "") || (rootNt.length() > 20) )
        throw runtime_error("invalid root nonterminal \"" + rootNt + "\"");
    } else {                     // additional non empty line, sy should be a nt
      if (ntMap[sy] != nullptr)
        throw runtime_error("duplicate nonterminal \"" + sy + "\"");
      ntMap[sy] = sp->ntSymbol(sy);
    } // else
  } // while

  // 2. initialize Grammar object according to default constructor
  root = ntMap[rootNt];
  if (root == nullptr)
    throw runtime_error("rule for root nonterminal \"" + rootNt +
                      "\" missing");
  initialize(root);

  // 3. re-read istream and generate terminals, sequences and rules
  is.clear();                    // reset flags (especially eof and fail)
  is.seekg(0);                   // rewind to the beginning
  firstNonEmptyLine = true;
  Sequence *seq = nullptr;
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
    if (sy == "" || sy.substr(0, 2) == "//")  // skip empty or comment line
      continue;
    if (sy.substr(0, 3) == "---") // start of opt. symbol information
      break;
    if (firstNonEmptyLine) {     // sy should be "G(...):"
      firstNonEmptyLine = false;
      continue;                  // skip
    } // if
    ntSy = sy;                   // non empty line starting with nt
    arrowSy = "";
    ls >> arrowSy;
    if (arrowSy != "->") {
      ostringstream lnrs;        // line number stream
      lnrs << lnr;
      throw runtime_error("syntax error in line " + lnrs.str() +
                          ": -> missing");
    } // if
    if (seq == nullptr)
      seq = new Sequence();
    while (!ls.eof()) {
      sy = "";
      ls >> sy;
      if (sy == "|") {
        addRule(ntMap[ntSy], seq);
        seq = new Sequence();
      } else if (sy != "" ) { // sy != "|" && sy != ""
        if ( (sy == "EPS") || (sy == "EPSILON") ||
             (sy == "eps") || (sy == "epsilon") )
          ; // nothing to do: seq is epsilon
        else if (ntMap[sy] != nullptr) // sy is a nonterminal
          seq->append(ntMap[sy]);
        else {                   // sy is a terminal
          if (tMap[sy] == nullptr)
            tMap[sy] = sp->tSymbol(sy);
          seq->append(tMap[sy]);
        } // else
      } // else
    } // while
    addRule(ntMap[ntSy], seq);
    seq = nullptr;
  } // while
} // Grammar::readGrammar


bool Grammar::insertIntoVNt(NTSymbol *ntSy) {
  Symbol *sy = v.symbolFor(ntSy->name);
  if (sy != nullptr) {
    if (sy->isT())
      throw invalid_argument("name clash for NT: a T is named \"" +
                             ntSy->name + "\"");
    return false;
  } else { // sy == nullptr
    vNt.insert(ntSy);
    v.  insert(ntSy);
    return true;
  } // else
} // Grammar::insertIntoVNt

bool Grammar::insertIntoVT(TSymbol *tSy) {
  Symbol *sy = v.symbolFor(tSy->name);
  if (sy != nullptr) {
    if (sy->isNT())
      throw invalid_argument("name clash for T: a NT is named \"" +
                             tSy->name + "\"");
    return false;
  } else { // sy == nullptr
    vT.insert(tSy);
    v. insert(tSy);
    return true;
  } // else
} // Grammar::insertIntoVT


Grammar::Grammar(NTSymbol *root) {
  check(root, "invalid nullptr for root nonterminal");
  initialize(root);
} // Grammar::Grammar

Grammar::Grammar(const string &fileName) {
  ifstream ifs(fileName);
  if (!ifs.good())
    throw invalid_argument("file \"" + fileName + "\" not found");
  readGrammar(ifs);
} // Grammar::Grammar

Grammar::Grammar(const char *grammarStr) {
  istringstream iss(string(grammarStr, strlen(grammarStr)));
  readGrammar(iss);
} // Grammar::Grammar


Grammar::~Grammar() {
  // nothing to do
} // Grammar::~Grammar


bool Grammar::addRule(NTSymbol *nt, Sequence *seq) {
  check(nt,  "invalid nullptr for nonterminal");
  check(seq, "invalid nullptr for sequence");
  insertIntoVNt(nt);
  vNtDelDefined = false;
  auto ir = rules[nt].insert(seq); // ir: insertion result
  if (!ir.second) { // seq is a duplicate
    delete seq;
    return false; // seq not inserted, so deleted
  } else { // grammar takes ownership of seq
    for (Symbol *sy: *seq) {
      if (sy->isT())
        insertIntoVT (dynamic_cast< TSymbol *>(sy));
      else // sy->isNT()
        insertIntoVNt(dynamic_cast<NTSymbol *>(sy));
    } // for
    return true; // seq inserted
  } // else
} // Grammar::addRule


void Grammar::addRule(NTSymbol *nt,
                      int n, ... /* n params. of type Sequence * */ ) {
  if (n <= 0)
    throw invalid_argument("invalid number of sequences");
  va_list vaList;
  va_start(vaList, n);
  for (int i = 0; i < n; i++) {
    Sequence *seq = va_arg(vaList, Sequence *);
    check(seq, "invalid nullptr for sequence");
    addRule(nt, seq);
  } // for
  va_end(vaList);
} // Grammar::addRule

#ifdef INITIALIZER_LIST_AVAILABLE
void Grammar::addRule(NTSymbol *nt, initializer_list<Sequence*> seqs) {
  for (auto seq: seqs) {
    check(seq, "invalid nullptr for sequence");
    addRule(nt, seq);
  } // for
} // Grammar::addRule
#endif


Sequence *Grammar::newSequenceOf(const string &str) const {
  istringstream iss(str);
  string syName;
  Sequence *seq = new Sequence();
  while (!iss.eof()) {
    syName.clear();
    iss >> syName;
    if (syName.length() > 0) {
      Symbol *sy = v.symbolFor(syName);
      check(sy, ("invalid symbol name \"" + syName +
                "\" in string").c_str());
      seq->append(sy);
    } // if
  } // while
  return seq;
} // Grammar::newSequenceOf


void Grammar::identifyDeletableNTs() const {
  if (vNtDelDefined)
    return;
  vNtDel.clear();

  // 1. look for NTs with an empty sequence
  for (auto &rule: rules) {
    for (Sequence *seq: rule.second) {
      if (seq->isEpsilon()) {
        vNtDel.insert(const_cast<NTSymbol *>(rule.first));
        break;
      } // if
    } // for
  } // for

  // 2. look for NTs having at least one deletable sequence
  size_t oldSize;
  do {
    oldSize = vNtDel.size();
    for (auto &rule: rules) {
      if (!vNtDel.contains(const_cast<NTSymbol *>(rule.first))) {
        for (Sequence *seq: rule.second) {
          bool seqIsDeletable = true;
          for (const Symbol *sy: *seq) {
            if (   sy->isT () ||
                 ( sy->isNT() &&
                  !vNtDel.contains(dynamic_cast<NTSymbol *>(
                                     const_cast<Symbol   *>(sy))) ) ) {
              seqIsDeletable = false;
              break;
            } // if
          } // for
          if (seqIsDeletable) {
            vNtDel.insert(const_cast<NTSymbol *>(rule.first));
            break;
          } // if
        } // for
      } // if
    } // for
  } while (vNtDel.size() > oldSize);

  vNtDelDefined = true;
} // Grammar::findDeletableNTs


bool Grammar::isDeletable(NTSymbol *sy) const {
  if (!vNtDelDefined)
    identifyDeletableNTs();
  return vNtDel.contains(sy);
} // Grammar::isDeletable


bool Grammar::isEpsilonFree() const {
  for (auto &rule: rules) {
    if (rule.first != root)  // empty seq. allowed for root only
      for (Sequence *seq: rule.second) {
        if (seq->isEpsilon())
          return false;
      } // for
  } // for
  return true;
} //  Grammar::isEpsilonFree


bool Grammar::rootHasEmptySequence() const {
  for (Sequence *seq: rules.find(root)->second) {
    if (seq->isEpsilon()) // root -> ... | EPS | ...
     return true;
  } // for
  return false;
} // Grammar::rootHasEmptySequence


bool Grammar::isRegular() const {
  bool rootHasEmptySeq = rootHasEmptySequence();
  for (auto &rule: rules) {
    for (Sequence *seq: rule.second) {
      if (seq->isEpsilon()) {
        if (rule.first != root)        // empty seq. allowed for root only
          return false;
      } else if (seq->length() == 1) { // required: NT -> T
        if (!seq->at(0)->isT())
          return false;
      } else if (seq->length() == 2) { // required: NT -> T NT
        if ( !seq->at(0)->isT()  ||
             !seq->at(1)->isNT() ||
             (rootHasEmptySeq && seq->at(1) == root) )
          return false;
      } else // seq->length() > 2
        return false;
    } // for
  } // for
  return true;
} // Grammar::isRegular


bool Grammar::isInCnf() const {
  bool rootHasEmptySeq = rootHasEmptySequence();
  for (auto &rule: rules) {
    for (Sequence *seq: rule.second) {
      if (seq->isEpsilon()) {
        if (rule.first != root)        // empty seq. allowed for root only
          return false;
      } else if (seq->length() == 1) { // required: NT -> T
        if (!seq->at(0)->isT())
          return false;
      } else if (seq->length() == 2) { // required: NT -> NT NT
        if (!seq->at(0)->isNT() ||
            !seq->at(1)->isNT() ||
            (rootHasEmptySeq && (seq->at(0) == root ||
                                 seq->at(1) == root)) )
          return false;
      } else // seq->length() > 2
        return false;
    } // for
  } // for
  return true;
} // Grammar::isInCnf()


ostream &operator<<(ostream &os, const Grammar &g) {
  os << endl;
  os << "G(" << *g.root << "):" << endl;
  for (auto &rule: g.rules) {
    os << *rule.first << " -> ";
    bool first = true;
    for (const Sequence *seq: rule.second) {
      if (!first)
        os << " | ";
      os << *seq;
      first = false;
    } // for
    os << endl;
  } // for
  os << "---" << endl;
  os << "VNT = " << g.vNt;
  if (g.vNtDelDefined)
    os << ", deletable: " << g.vNtDel;
  os << endl;
  os << "VT  = " << g.vT  << endl;
  os << endl;
  return os;
} // operator<<


// === test ============================================================

#if 0

#include <cstdio>

#include <typeinfo>

#include "SymbolStuff.cpp"
#include "SequenceStuff.cpp"

int main(int argc, char *argv[]) {
try {

  SymbolPool *sp = SymbolPool::getInstance();

  Grammar *g = nullptr;

// 1. Grammar construction programmatically

  // start with an empty grammar:
  // S ->

  NTSymbol *S = sp->ntSymbol("S");

  g = new Grammar(S);

  cout << "Empty grammar = " << endl << *g << endl;

  // additions to empty grammar:
  // S -> EPS | A
  // A -> a | A a

  NTSymbol *A = sp->ntSymbol("A");
   TSymbol *a = sp-> tSymbol("a");

  g->addRule(S, new Sequence());  // epsilon alternative
  g->addRule(S, new Sequence(A));
#ifndef INITIALIZER_LIST_AVAILABLE
  g->addRule(A, 2, new Sequence(a), new Sequence(2, A, a ) );
#else
  g->addRule(A,   {new Sequence(a), new Sequence(  {A, a})});
#endif

  cout << "Non empty Grammar = " << endl << *g << endl;

  delete g;

// 2. Grammar construction from C string

  g = new Grammar(
    "G(S):             \n\
     S -> A ;          \n\
     A -> a B | B B b  \n\
     B -> b | a b      ");

  cout << "Grammar from C string = " << endl << *g << endl;

  delete g;

// 3. Grammar construction from text file

  string path = "U:\\0Data\\Programme\\_Cpp\\#FormalLanguages\\";
  string fileName = "Grammar1.txt";
  g = new Grammar(path + fileName);

  cout << "Grammar from file " << fileName << " = " << endl << *g << endl;

  delete g;

  delete sp;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of Grammar.cpp
//======================================================================
