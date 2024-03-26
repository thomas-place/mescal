#ifndef REGAUT_H_
#define REGAUT_H_

#include "nfa.h"
#include "nfa_determi.h"
#include "regexp.h"
#include "nfa_intersec.h"

nfa *reg2nfa(regexp *r);

nfa *reg2dfa(regexp *r);

#endif // REGAUT_H_
