// SequenceStuff.cpp:                                     HDO, 2004-2015
// -----------------
// Classes Sequence and SeqeuenceSet for sets of Sequence objects.
// Sequence objects represent (possibly empty) sequences of
//   pointers to either T- or NTymbol objects.
// SequenceSet objects take ownership of their sequences.
// =====================================================================

#include <cstdarg>

#include <iostream>
#include <stdexcept>

using namespace std;

#include "SequenceStuff.h"


#ifndef CHECK_FUNCTION_DEFINED
#define CHECK_FUNCTION_DEFINED
static inline void check(void *ptr, const char *errMsg) {
  if (ptr == nullptr)
    throw invalid_argument(errMsg);
} // check
#endif

// === class Sequence ==================================================

Sequence::Sequence() {
  // nothing to do for the empty sequence
} // Sequence::Sequence

Sequence::Sequence(Symbol *sy) {
  ::check(sy, "invalid nullptr for symbol");
  push_back(sy);
} // Sequence::Sequence

Sequence::Sequence(const Sequence &seq)
  : Base(seq), ObjectCounter<Sequence>() {
  // nothing left to do
} // Sequence::Sequence

Sequence::Sequence(int n, ... /* n params. of type Symbol* */ ) {
  if (n <= 0)
    throw invalid_argument("invalid number of elements");
  va_list vaList;
  va_start(vaList, n);
  for (int i = 0; i < n; i++) {
    Symbol *sy = va_arg(vaList, Symbol*);
    ::check(sy, "invalid nullptr for symbol");
    push_back(sy);
  } // for
  va_end(vaList);
} // Sequence::Sequence

#ifdef INITIALIZER_LIST_AVAILABLE
Sequence::Sequence(initializer_list<Symbol *> il) {
  for (auto sy: il) {
    ::check(sy, "invalid nullptr for symbol");
    push_back(sy);
  } // for
} // Sequence::Sequence
#endif

Sequence::~Sequence() {
  // nothing left to do
} // Sequence::~Sequence

void Sequence::check(int idx) const {
  if ( (0 <= idx) && (idx < length()) )
    return;
  throw range_error("sequence index out of range");
} // Sequence::check

void Sequence::check(iterator it) const {
  if ( (begin() <= it) && (it < end()) )
    return;
  throw range_error("sequence iterator out of range");
} // Sequence::check

int Sequence::length() const {
  return size();
} // Sequence::length

void Sequence::append(Symbol *sy) {
  ::check(sy, "invalid nullptr for symbol");
  push_back(sy);
} // Sequence::append

void Sequence::append(Sequence *seq) {
  ::check(seq, "invalid nullptr for sequence");
  for (auto sy: *seq) {
    append(sy);
  } // for
} // Sequence::append

Symbol *&Sequence::operator[](int idx) {
  check(idx);
  return Base::operator[](idx);
} // Sequence::operator[]

Symbol *Sequence::symbolAt(int idx) const {
  check(idx);
  return *(begin() + idx);
} // Sequence::symbolAt

Symbol *Sequence::symbolAt(iterator it) const {
  check(it);
  return *it;
} // Sequence::symbolAt

void Sequence::replaceSymbolAt(int idx, Symbol *sy) {
  check(idx);
  replaceSymbolAt(begin() + idx, sy); // see below
} // Sequence::replaceSymbolAt

void Sequence::replaceSymbolAt(iterator it, Symbol *sy) {
  check(it);
  ::check(sy, "invalid nullptr for symbol");
  *it = sy;
} // Sequence::replaceSymbolAt

void Sequence::replaceSymbolAt(int idx, Sequence *seq) {
  check(idx);
  replaceSymbolAt(begin() + idx, seq); // see below
} // Sequence::replaceElementAt

void Sequence::replaceSymbolAt(iterator it, Sequence *seq) {
  check(it);
  ::check(seq, "invalid nullptr for sequence");
  it = erase(it);
  if (seq->size() > 0)
    insert(it, seq->begin(), seq->end());
} // Sequence::replaceElementAt

bool Sequence::hasTerminalsOnly() const {
  for (const Symbol *sy: *this) {
    if (sy->isNT())
      return false;
  } // for
  return true;
} // Sequence::hasTerminalsOnly

bool Sequence::isEpsilon() const {
  return size() == 0;
} // Sequence::isEpsilon


bool operator<(const Sequence &seq1, const Sequence &seq2) {
  if (&seq1 == &seq2)
    return false;
  Sequence::const_iterator it1 = seq1.begin();
  Sequence::const_iterator it2 = seq2.begin();
  while ( (it1 != seq1.end()) &&
          (it2 != seq2.end()) ) {
    int cmpRes = (*it1)->name.compare((*it2)->name);
    if (cmpRes != 0)
      return (cmpRes < 0);
    // cmpRes == 0, so continue
    it1++;
    it2++;
  } // while
  return ( (it1 == seq1.end()) &&
           (it2 != seq2.end()) );
} // operator<


bool operator==(const Sequence &seq1, const Sequence &seq2) {
  if (&seq1 == &seq2)
    return true;
  if (seq1.size() != seq2.size())
    return false;
  return equal(seq1.begin(), seq1.end(),
               seq2.begin(),
               EqualForSymbolPtrs());
} // operator==


bool LessForSequencePtrs::operator()(const Sequence* seq1,
                                     const Sequence* seq2) const {
    return (*seq1) < (*seq2);
} // LessForSequencePtrs::operator()

bool EqualForSequencePtrs::operator()(const Sequence* seq1,
                                      const Sequence* seq2) const {
    return (*seq1) == (*seq2);
} // EqualForSequencePtrs::operator()


ostream &operator<<(ostream &os, const Sequence &seq) {
  if (seq.isEpsilon())
    os << "EPS";
  else {
    bool first = true;
    for (const Symbol *sy: seq) {
      if (!first)
        os << " ";
      os << *sy;
      first = false;
    } // for
  } // else
  return os;
} // operator<<


// === class SequenceSet ===============================================

SequenceSet::SequenceSet() {
  // nothing to do
} // SequenceSet::SequenceSet

SequenceSet::SequenceSet(const SequenceSet &ss) {
  for (auto &seq: ss) {
    insert(new Sequence(*seq));
  } // for
} // SequenceSet::SequenceSet

SequenceSet::~SequenceSet() {
  for (auto &seq: *this) {
    delete seq;
  } // for
} // SequenceSet::~SequenceSet

void SequenceSet::insertOrDelete(Sequence *&s) {
  ::check(s, "invalid nullptr for sequence");
  auto ir = insert(s);
  if (!ir.second) { // s has not been inserted, it's a duplicate
    delete s;
    s = nullptr;
  } // if
} // SequenceSet::insertOrDelete


bool operator==(const SequenceSet &ss1, const SequenceSet &ss2) {
  if (&ss1 == &ss2)
    return true;
  if (ss1.size() != ss2.size())
    return false;
  return equal(ss1.begin(), ss1.end(),
               ss2.begin(),
               EqualForSequencePtrs());
} // operator==

ostream &operator<<(ostream &os, const SequenceSet &ss) {
  os << "{ ";
  bool first = true;
  for (auto &seq: ss) {
    if (!first)
      os << ", ";
    os << *seq;
    first = false;
  } // for
  os << " }" << endl;
  return os;
} // operator<<



// === test ============================================================

#if 0

#include <cstdio>
#include <typeinfo>

#include "SymbolStuff.cpp"

int main(int argc, char *argv[]) {
try {

  SymbolPool *sp = SymbolPool::getInstance();

  Sequence *epsilon = new Sequence();
  cout << "epsilon = " << *epsilon << endl;

   TSymbol *tSy  = sp->tSymbol("terminal");
   TSymbol *aSy  = sp->tSymbol("a");
   TSymbol *bSy  = sp->tSymbol("b");
  NTSymbol *ntSy = sp->ntSymbol("nonterminal");
  NTSymbol *ASy  = sp->ntSymbol("A");
  NTSymbol *BSy  = sp->ntSymbol("B");

  Sequence *seq1    = new Sequence(tSy);
  seq1->append(ntSy);

  Sequence *seq2;

#ifdef INITIALIZER_LIST_AVAILABLE
  seq2 = new Sequence(  {ntSy, tSy});
#else
  seq2 = new Sequence(2, ntSy, tSy);
#endif

  cout << "seq1 = " << *seq1 << endl;
  cout << "seq2 = " << *seq2 << endl;

  seq1->append(seq2);
  cout << "new seq1 = " << *seq1 << endl;

  cout << endl;

  delete epsilon;
  delete seq1;
  delete seq2;

#ifdef INITIALIZER_LIST_AVAILABLE
  seq1 = new Sequence(  {aSy, ASy});
  seq2 = new Sequence(  {aSy, BSy});
#else
  seq1 = new Sequence(2, aSy, ASy);
  seq2 = new Sequence(2, aSy, BSy);
#endif

  cout << "seq1 = " << *seq1 << endl;
  cout << "seq2 = " << *seq2 << endl;

  cout << "seq1 <  seq2 == " << (*seq1 <  *seq2) << endl;
  LessForSequencePtrs lfsp;
  cout << "seq1 <  seq2 == " << lfsp(seq1, seq2) << endl;
  cout << "seq1 == seq2 == " << (*seq1 == *seq2) << endl;
  cout << "seq1 >  seq2 == " << (*seq1 >  *seq2) << endl;

  cout << endl;

  SequenceSet *ss1 = new SequenceSet;
  ss1->insert(new Sequence());
  ss1->insert(seq1);
  ss1->insert(seq2);

  Sequence *seq3 = new Sequence(*seq2);
  ss1->insertOrDelete(seq3); // a duplicate
  if (seq3 != nullptr)
    cerr << "ERROR: seq3 should have been deleted" << endl;

  cout << "sequence set = " << *ss1;

  SequenceSet *ss2 = new SequenceSet(*ss1);
  cout << "copy of sequence set = " << *ss2;
  cout << "original == copy ? : " << (*ss1 == *ss2) << endl;
  delete ss2;

  delete ss1;

  delete sp;

} catch(const exception &e) {
  cerr <<  "ERROR (" << typeid(e).name() << "): " << e.what() << endl;
} // catch

  // cout << "type CR to continue ...";
  // getchar();

  return 0;
} // main

#endif


// end of SequenceStuff.cpp
//======================================================================
