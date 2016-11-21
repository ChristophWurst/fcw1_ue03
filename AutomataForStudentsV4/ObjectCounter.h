// ObjectCounter.h:                                       HDO, 2013-2015
// ---------------
// Simple mechanism to count objects of any user defined class (UDC) and
// to report garbage on program termination. Usage:
//   class UDC: ..., private ObjectCounter<UDC> { ... }; // UDC
// ObjectCounter should be applied only once within a class hierarchy.
// Implementation based on CRTP (curiously recurring template pattern),
// see en.wikipedia.org/wiki/Curiously_recurring_template_pattern.
// =====================================================================

#ifndef ObjectCounter_h
#define ObjectCounter_h


#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>


// Activation or deactivation of object counting with #define or #undef
#define DO_OBJECT_COUNTING


#ifndef DO_OBJECT_COUNTING  // no object counting


  template <class UDC>
  class ObjectCounter {     // dummy object counter, does nothing
  }; // ObjectCounter<UDC>


#else                       // do object counting


template <class UDC>
class ObjectCounterData;    // data for ObjectCounter, implementation see below

template <class UDC>        // UDC is a user defined class ...
class ObjectCounter {       //   ... privately derived from ObjectCounter ...
  // ... which counts objects of class UDC using an ObjectCounterData object

  private:

    static ObjectCounterData<UDC> ocd;

    void CountConstruction() {
      auto ir = ocd.om.insert(std::make_pair(this, ++ocd.nConstr));
      if (!ir.second)       // this already has been an element of om
        throw std::logic_error(std::string("re-construction of object of class ") +
                               typeid(UDC).name());
    } // CountConstruction

  protected:

    ObjectCounter() {       // called by UDC's default constr.
      CountConstruction();
    } // ObjectCounter

    ObjectCounter(const ObjectCounter &/*oc*/) { // called by UDC's copy constr.
      CountConstruction();
    } // ObjectCounter

    virtual ~ObjectCounter() { // called by UDC's destr.
      auto ec = ocd.om.erase(this);
      if (ec == 1) // ok, this has been an element of om
        ocd.nDestr++;
      else // (ec == 0) => this has not been registered in om
        ocd.nDestrUnreg++;
  } // ~ObjectCounter

}; // ObjectCounter<UDC>

template <class UDC>
ObjectCounterData<UDC> ObjectCounter<UDC>::ocd(std::string(typeid(UDC).name()));


template <class UDC>
class ObjectCounterData final {

  friend class ObjectCounter<UDC>;

  private:

    const std::string className; // format depends on RTTI, roughly "...UDC..."
    int nConstr, nDestr;    // number of constructions and destructions
    int nDestrUnreg;        // number of destructions of unregistered objects
    std::unordered_map<void *, int> om; // object map: address -> nConstr

    ObjectCounterData();    // prohibit default construction, ...
    ObjectCounterData(const ObjectCounterData &ocd); // ... copy construction and ...
    ObjectCounterData &operator=(const ObjectCounterData &ocd); // ... assignment

    ObjectCounterData(const std::string &className)
    : className(className), nConstr(0), nDestr(0), nDestrUnreg(0), om() {
      // nothing to do
    } // ObjectCounterData

    ~ObjectCounterData() {
    int nAlive = nConstr - nDestr;
      if (!std::cout.good())
        return;
      std::cout << std::endl;
      std::cout << "ObjectCounter<" << className << ">: " <<
                   "+" << nConstr << " -" << nDestr << " = " << nAlive << " alive";
      if (nDestrUnreg > 0)
        std::cout << ", destr. of unreg. obj. = " << nDestrUnreg;
      std::cout << std::endl;
      if (nAlive > 0) {
        std::unordered_map<int, void *> iom; // inverted om: nConstr -> address
        for (auto &e: om) {
          iom[e.second] = e.first;
        } // for
        for (auto &e: iom) {
          std::cout << "  constrNr=" << e.first << ", address=" << e.second;
  #if 0 // activate only if all classes UDC have operator<< defined
          std::cout << ": " << *((UDC*)e.second);
  #endif
          std::cout << std::endl;
        } // for
      } // if
    } // ~ObjectCounterData

}; // ObjectCounterData<UDC>


#endif // DO_OBJECT_COUNTING


#endif


// end of ObjectCounter.h
// =====================================================================
