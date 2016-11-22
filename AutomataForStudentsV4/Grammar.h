// Grammar.h:                                             HDO, 2004-2015
// ---------
// Objects of class Grammar represent context free grammars, where
//   G(root) = { root -> Sequence | ... , NT -> ... | ... , ... }
// =====================================================================

#ifndef Grammar_h
#define Grammar_h

#if ( (__GNUG__ >= 4) && (__GNUC_MINOR__ >= 8) ) || \
    ( _MSC_VER >= 1800 )
  #define INITIALIZER_LIST_AVAILABLE
#endif

#include <iosfwd>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>
#ifdef INITIALIZER_LIST_AVAILABLE
  #include <initializer_list>
#endif


#include "SymbolStuff.h"
#include "Vocabulary.h"
#include "SequenceStuff.h"
#include "ObjectCounter.h"


typedef Vocabulary<NTSymbol> VNt;
typedef Vocabulary< TSymbol> VT;
typedef Vocabulary<  Symbol> V;


class Grammar: // no public base class
               private ObjectCounter<Grammar> {

  friend std::ostream &operator<<(std::ostream &os, const Grammar &g);

  private:

    mutable bool  vNtDelDefined;  // initially false
    mutable VNt   vNtDel;         // deletable nts, subset of vNt,
                                  //   filled by identifyDeletableNTs

    Grammar(const Grammar *g);    // prohibit copy construction and ...
    Grammar &operator=(const Grammar *g); // ... assignment

    void initialize(NTSymbol *root);    // do first part of constructors work

    void readGrammar(std::istream &is); // init. rest or grammar from stream

    bool insertIntoVNt(NTSymbol *ntSy); // true if inserted else a duplicate
    bool insertIntoVT ( TSymbol * tSy); // true if inserted else a duplicate

  public:

    // Rules take ownership of their Sequences (as SequenceSets do)
    typedef std::pair<const NTSymbol *, SequenceSet> Rule;
    typedef std::map <      NTSymbol *, SequenceSet, LessForSymbolPtrs> RulesMap;

    NTSymbol  *root;    // no ownership: SymbolPool is the owner
    RulesMap   rules;   // has at least an empty rule for root
    VNt        vNt;     // all nonterminals including root
    VT         vT;      // all terminals
    V          v;       // all nonterminals and terminals

    Grammar(NTSymbol *root); // for "manual" construction of grammar
      // via subsequent calls of addRule

    Grammar(const std::string &fileName); // construct grammar from file or
    Grammar(const char *grammarStr);      // construct grammar from string
      // syntax as generatd by operator<< with one rule/line and one line/rule:
      //   G(S):
      //   S -> ... | ...

    virtual ~Grammar();

    bool addRule(NTSymbol *nt, Sequence *seq); // inserts a Sequence
      // into the rule for nt, returns
      //   true  if seq was a new one, that has been added
      //   false if seq was a duplicate, so addRule deleted seq

    void addRule(NTSymbol *nt,
                 int n, ... /* n params. of type Sequence * */ );

#ifdef INITIALIZER_LIST_AVAILABLE
    void addRule(NTSymbol *nt, initializer_list<Sequence *> seqs);
#endif

    Sequence *newSequenceOf(const string &str) const; // based on vT and vNt,
      // e.g., "a B" => Sequence(2, sp->tSymbol("a"), sp->ntSymbol("B")))

    void identifyDeletableNTs() const;    // inserts del. NTs into vNtDel
    bool isDeletable(NTSymbol *sy) const; // uses vNtDel

    bool isEpsilonFree() const; // only root may have an empty sequence
    bool rootHasEmptySequence() const;

    bool isRegular() const;    // check for regularity: A -> a | a B and S -> EPS

    bool isInCnf() const;      // check for Chomsky normal form

}; // Grammar

std::ostream &operator<<(std::ostream &os, const Grammar &g);


#endif


// end of Grammar.h
//======================================================================
