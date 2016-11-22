#ifndef Moore_h
#define Moore_h
#include <map>
#include <string>
#include "DFA.h"

class Moore : public DFA {
  public:
    map<char, char> transMap; // transformation table
    Moore(const char *dfaStr) : DFA(dfaStr) {}
    std::string transformer(const TapeSymbol tape[]) {
        std::string result = "";
        int        i  = 0;
        TapeSymbol sy = tape[0];
        State      s  = s1;
        while (sy != EOT_SYMBOL) {
            s = delta[s][sy]; // STATE CHANGE
            if (!defined(s)) {
                return "";
            }
            // TRANSFROM AFTER STATE CHANGE
            result.push_back(this->transMap[sy]);
            i++;
            sy = tape[i];
        }
        return result;
    }
};

#endif




