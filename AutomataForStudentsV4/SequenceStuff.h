// SequenceStuff.h:                                       HDO, 2004-2015
// ---------------
// Classes Sequence and SequenceSet for sets of Sequence objects.
// Sequence objects represent (possibly empty) sequences of
//   pointers to either T- or NTSymbol objects.
// SequenceSet objects take ownership of their sequences.
// =====================================================================

#ifndef SequenceStuff_h
#define SequenceStuff_h


#if ( (__GNUG__ >= 4) && (__GNUC_MINOR__ >= 8) ) || \
    ( _MSC_VER >= 1800 )
  #define INITIALIZER_LIST_AVAILABLE
#endif

#include <memory>
#include <set>
#include <vector>
#ifdef INITIALIZER_LIST_AVAILABLE
  #include <initializer_list>
#endif

#include "ObjectCounter.h"
#include "SymbolStuff.h"


// === class Sequence ==================================================

class Sequence: public  std::vector<Symbol *>,
                private ObjectCounter<Sequence> {

  private:

    typedef std::vector<Symbol *> Base;

    Sequence &operator=(const Sequence &seq);

    void check(int      idx) const;
    void check(iterator it ) const;

  public:

    typedef Base::iterator iterator;

    Sequence(); // empty Sequence aka epsilon
    Sequence(Symbol *sy);
    Sequence(const Sequence &seq);
    Sequence(int n, ... /* n params. of type Symbol * */);
#ifdef INITIALIZER_LIST_AVAILABLE
    Sequence(initializer_list<Symbol *> il);
#endif

    virtual ~Sequence();

    int length() const; // in number of Symbols

    void append(Symbol   *sy);
    void append(Sequence *seq);

    Symbol *&operator[](int      idx);
    Symbol * symbolAt  (int      idx) const;
    Symbol * symbolAt  (iterator it ) const;

    void replaceSymbolAt(int      idx, Symbol   *sy);
    void replaceSymbolAt(iterator it , Symbol   *sy);

    void replaceSymbolAt(int      idx, Sequence *seq);
    void replaceSymbolAt(iterator it , Sequence *seq);

    bool hasTerminalsOnly() const;

    bool isEpsilon() const; // <=> length() == 0

}; // Sequence

bool operator< (const Sequence &seq1, const Sequence &seq2);
bool operator==(const Sequence &seq1, const Sequence &seq2);

struct LessForSequencePtrs { // uses operator<
  bool operator()(const Sequence *seq1, const Sequence *seq2) const;
}; // LessForSequencePtrs

struct EqualForSequencePtrs { // uses operator==
  bool operator()(const Sequence *seq1, const Sequence *seq2) const;
}; // EqualForSequencePtrs

std::ostream &operator<<(std::ostream &os, const Sequence &seq);


// === class SequenceSet ===============================================

class SequenceSet: public  std::set<Sequence *, LessForSequencePtrs>,
                   private ObjectCounter<SequenceSet> {

  private:

    typedef std::set<Sequence *, LessForSequencePtrs> Base;

    SequenceSet &operator=(const SequenceSet &ss); // ... and assignment

  public:

    SequenceSet();
    SequenceSet(const SequenceSet &ss); // MS cl needs the copy constructor

    ~SequenceSet(); // deletes the elements

    void insertOrDelete(Sequence *&s); // delete s if it's already in set

}; // SequenceSet

bool operator==(const SequenceSet &ss1, const SequenceSet &ss2);

std::ostream &operator<<(std::ostream &os, const SequenceSet &ss);


#endif


// end of SequenceStuff.h
//======================================================================
