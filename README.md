
# 🌵 mescal 🌵

**ME**mbership and **S**eparation for **C**l**A**sses of **L**anguages Version 0.1


The aim of this program is to implement most of the known algorithms for deciding membership and separation for prominent classes of regular languages.

# Available commands

## Basic commands

+ `help` This help.
+ `quit` or `exit` Quits.
+  `languages` Lists stored languages.
+  `automata` Lists stored automata.
+  `morphisms` Lists stored morphisms.
+  `sort` Sorts all objects, lexicographically by name.
+  `delete(<variable>)` Deletes object (language, automaton or morphism).
+  `savesession("<filename>")` Saves all current objects in a file.
+ `loadsession("<filename>")` Loads all current objects from a file.
+ Variable names: `<upper><upper|digits>*`.

## Commands for language variables

### Definition, storing a language in a variable

+ Specification by a regular expression:\
  `<variable> = "<regexp>"`\
   Syntax of regexps:\
    e := e + e | e e | e* | !e | (e) | e & e | [d] | 1 | 0 where 0 = ∅, 1 = ε, ! = complement, & = ∩, [d] = a + b + c + d.
+ Specification by an automaton:\
  `<variable> = link(<variable_name_of_automaton>)`\
  Creates a link with the NFA in the automata variable A (A becomes protected).

#### Manipulation of languages:

+ `L` Displays the regular expression or the automaton used to define L.
+ `L.minimal` The minimal automaton of L: works as an automata variable.
+ `L.syntactic` The syntactic morphism of L: works as a morphism variable.

## Commands for automata variables

### Definition, storing an automaton in a variable A:

+ `A = B` Makes a copy of the automaton B.
+ `open("filename")` Opens the NFA stored in a file.
+ `thomson(L)` Thomson's algorithm (L must be a language specified by a regexp).
+ `elimepsilon(B)` Elimination of the epsilon transitions in the automaton B.
+ `trim(B)` Elimination of all states that are not accessible or co-accessible in the automaton B.
+ `union(B1,B2)` Nondeterministic union of the automata B1 and B2.
+ `intersection(B1,B2)` Intersection of the automata B1 and B2.
+ `concatenation(B1,B2)` Concatenation of the automata B1 and B2.
+ `kleene(B1,B2)` Subset construction applied to the automaton B.
+ `minimal(B1,B2)` Minimization applied to the automaton B.

### Manipulation:

+ `A` Displays the automaton.
+ `A.run(word)` Computes the set of states reached with an input word.
+ `save(A,"filename")` Saves the automaton A in a file.

## Commands for morphisms variables

### Manipulation

+ `M` Displays the Green relations of M.
+ `M.rcayley` The right Cayley graph of M.
+ `M.lcayley` The left Cayley graph of M.
+ `M.multiplication` The multiplication table of M.
+ `M.image(word)` Computes the image of a word.
+ `M.order` The ordering of M.
+ `M.idempotents` The idempotents of M.
+ `M.kernel(𝒞)` The 𝒞-kernel of M (implemented for 𝒞 = MOD, GR).
+ `M.orbits(𝒞)` The 𝒞-orbits for M (implemented for 𝒞 = DD, MOD⁺, AT).
+ `M.orbit(𝒞,e)` The 𝒞-orbit of the idempotent e for M (implemented for 𝒞 = DD, MOD⁺, AT).
+ `M.orbits(OP,𝒞)` The OP(𝒞)-orbits for M (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR).
+ `M.orbit(OP,𝒞,e)` The OP(𝒞)-orbits of the idempotent e for M (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR).

## Membership tests

+ `membership(𝒞,L)` Checks if the language L belongs to 𝒞.
  - Available classes:
    * `ST` Trivial class.
    * `MOD` Modulo languages.
    * `AMT` Alphabet modulo testable languages.
    * `GR` Group languages.
    * `AT` Alphabet testable languages.
    * `ATT` Alphabet threshold testable languages.
    * `LT` Locally testable languages.
    * `LTT` Locally threshold testable languages.
    * `PPT` Positive piecewise testable languages.
    * `PT` Piecewise testable languages.
    * `SF` Star-free languages.
    * `UL` Unambiguous languages.

+ `membership(OP,𝒞,L)` Checks if the language L belongs to OP(𝒞).

  - Available input classes:
    * `ST` Trivial class.
    * `DD` Well-suited extension of ST (DD = ST⁺).
    * `MOD` Modulo languages.
    * `MODP` Well-suited extension of the modulo languages (MODP = MOD⁺).
    * `GR` Group languages.
    * `AT` Alphabet testable languages.
  - Available operators:
    * `Pol` Polynomial closure (𝒞 ↦ Pol(𝒞)).
    * `Bpol` Boolean polynomial closure (𝒞 ↦ BPol(𝒞))
    * `UBPol` Combined operator 𝒞 ↦ UPol(BPol(𝒞)) (does not work with AT).
    * `Pol2` Combined operator 𝒞 ↦ Pol(BPol(𝒞)) (does not work with AT).
    * `UBPol2` Combined operator 𝒞 ↦ UPol(BPol(BPol(𝒞))) (does not work with AT).
    * `FL` Future unary temporal logic closure (𝒞 ↦ FL(𝒞)).
    * `FL2` Combined operator 𝒞 ↦ FL(FL(𝒞)) (does not work with AT).
    * `PL` Past unary temporal logic closure (𝒞 ↦ PL(𝒞)).
    * `PL2` Combined operator 𝒞 ↦ PL(PL(𝒞)) (does not work with AT).
    * `TL` Unary temporal logic closure (𝒞 ↦ TL(𝒞)).
    * `TL2` Combined operator 𝒞 ↦ TL(TL(𝒞)) (does not work with AT).
    * `SF` Star-free closure.
  
+ `concatenation(L)` Summary of membership tests for the language L in concatenation hierarchies.
+ `neghiera(𝒞,L)` If L belongs to TL(𝒞), determines its level in the negation hierarchy of TL(𝒞).
+ `fphiera(𝒞,L)` If L belongs to TL(𝒞), determines its level in the future branch and the past branch of the future/past hierarchy of TL(𝒞).

  - Available input classes for negation hierarchies and future/past hierarchies::
    * `ST` Trivial class.
    * `DD` Well-suited extension of ST (DD = ST⁺).
    * `MOD` Modulo languages.
    * `MODP` Well-suited extension of the modulo languages (MODP = MOD⁺).
    * `GR` Group languages.

## Separation tests

+ `separ(𝒞,L1,L2,(info))` Checks if L1 is 𝒞-separable from L2 (\"info\" is an optional parameter for asking more details).
  - Available classes:
    * `ST` Trivial class.
    * `MOD` Modulo languages.
    * `GR` Group languages.

+ `separ(OP,𝒞,L1,L2,(info))` Checks if L1 is OP(𝒞)-separable from L2 (\"info\" is an optional parameter for asking more details).

  - Available input classes:
    * `ST` Trivial class.
    * `MOD` Modulo languages.
    * `GR` Group languages.
  - Available operators:
    * `Pol` Polynomial closure (𝒞 ↦ Pol(𝒞)).
