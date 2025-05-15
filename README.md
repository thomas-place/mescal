
# 🌵 MeSCaL 🌵

<b>Me</b>mbership and <b>S</b>eparation for <b>C</b>l<b>a</b>sses of <b>L</b>anguages 


The aim of this program is to implement most of the known algorithms for deciding membership and separation for prominent classes of regular languages.

# Installation

A `Makefile` is provided in the src/ folder. It creates an executable file named `mescal`, which can be executed from that directory with the shell command `./mescal`.

## Requirements

Currently, the program is **experimental**. It works on MacOS and Linux. It depends on the following software, which can be installed through  [homebrew](https://brew.sh) on MacOS, or through apt on Ubuntu or Debian Linux.

+ A C compiler (tested on OSX with Apple clang 15.0.0 to 17.0.0, hardlinked to gcc), with the readline library.
+ The [json-c](https://github.com/json-c) library.
+ The [flint](https://flintlib.org) library.
+ flex and bison.
+ [Graphviz](https://graphviz.org).
+ On MacOS: [iTerm](https://iterm2.com) with [imgcat](https://iterm2.com/utilities/imgcat).
+ On Linux: a [sixel capable terminal](https://www.arewesixelyet.com). Tested with [Konsole](https://konsole.kde.org).

# Source code documentation

A Doxygen documentation of the source code is provided [here](https://thomas-place.github.io/mescal/).

# Available commands


## Basic commands

### General commands

    help                          This help.
    timeout                       Displays the current timeout.
    timeout = <value>             Sets the timeout to <value> seconds (0 to cancel timeout).
    limit                         Displays the current limit.
    limit = <value>               Sets the limit to <value>.
    history                       Displays the current history size.
    history = <value>             Sets the history size to <value> entries.
    quit or exit                  Quits.
    classes                       Lists all classes recognized by the program.

### Using variables

    <variable> = <command>        Stores a copy of the returned object in the variable.
    delete(<variable>)            Deletes object.

        Variable types: regular expression, recursive definition of regular expressions, automaton and morphism.
        Variable names: <upper><upper|digits>*

### Filtering commands

    list(<parameters>)            Lists all stored objects according to the parameters.
    regexps(<parameters>)         Lists all stored regular expressions according to the parameters.
    automata(<parameters>)        Lists all stored automata according to the parameters.
    morphisms(<parameters>)       Lists all stored morphisms according to the parameters.
    regexps(<parameters>)         Lists all stored recursive definitions according to the parameters.

**Available parameters:**

    -syntactic                    displays the size of the syntactic monoid.
    -syntactic(<n1>,<n2>)         only lists the objects with a syntactic monoid whose size is between n1 and n2.
    -idempotents                  displays the number of idempotents in the syntactic monoid.
    -idempotents(<n1>,<n2>)       only lists the objects with a syntactic monoid has between n1 and n2 idempotents.
    -minimal                      displays the size of the minimal automaton.
    -minimal(<n1>,<n2>)           only lists the objects with a minimal automaton whose size is between n1 and n2.


### Sorting commands

    sort                          Sorts all objects, lexicographically by variable name.
    sort(<parameter>)             Sorts all objects, according to the ordering given by the parameter.

**Available parameters:**

    -syntactic                    Size of the syntactic monoid
    -minimal                      Size of the minimal automaton

### Saving and loading

    save(<command>,"<filename>")  Saves the object returned by a command in a file.
    open("<filename>")            Opens and returns an object stored in a file.
    savesession("<filename>")     Saves all current objects in a file.
    loadsession("<filename>")     Loads all current objects from a file.

## Using regular expressions variables

### Definition, of a regular expression

    "<regexp>"

    Syntax of regexps:
       e := e + e | e e | e* | !e | (e) | e & e | \1 | \0
     where,
       0 = ∅, 1 = ε, ! = complement, & = ∩

### Manipulation of a regular expression stored in a variable L

    L                             Displays the regular expression.
    L.minimal                     The minimal automaton: works as an automaton variable.
    L.syntactic                   The syntactic morphism: works as a morphism variable.
    thomson(L)                    Uses the Thomson algorithm to compute a NFA.
    glushkov(L)                   Uses the Glushkov algorithm to compute a NFA.

## Using recurvive definitions variables

### Initialization

    initrecursion(<n>,<v1>,...)   Returns an initialized recursive definition variable.
                                  <n> is the largest index that has to be specified by the user.
                                  <v1>,... is a list of variable names denoting the relations used in the recursive definition.
                                  Example: R = initrecursion(0,L)


### Setup of an initialized recursive definition variable R

    E.<v>[<n>] = <regexp>         Defines the initial case <n> of the relation <v>.
                                  (<n> must be smaller than the maximum index specified in the initialization).
                                  Example: R.L[0] = "\1"

    E.<v>[i] = <symbolic regexp>  Defines the recursion of the relation <v>.
                                  The regular expression can use symbolic variables and letters.
                                  Example: R.L[i] = "(a{L}[i-1]b)*"

**Syntax of symbolic regexps:**

    e := e + e | e e | e* | !e | (e) | e & e | \1 | \0 | a[i - <n>] | {<v>}[i - <n>]
        where,
    0 = ∅, 1 = ε, ! = complement, & = ∩, a[i - <n>] = symbolic letter, {<v>}[i - <n>] = symbolic variable

### Usage of a recursive definition variable R

    R                             Displays the recursive definition and checks if it is well-defined.
    R.<v>[<n>]                    Returns the regular expression of index <n> for the relation <v>.
                                  (can be copied into a regular expression variable).

## Using variables representing automata

### Manipulation of an automaton stored in a variable A

    A                             Displays the automaton.
    A.minimal                     The minimal automaton: works as an automaton variable.
    latex(A.minimal)              The minimal automaton, printed in LaTeX.
    A.syntactic                   The syntactic morphism: works as a morphism variable.
    latex(A.syntactic)            The right Cayley graph of the minimal automaton, printed in LaTeX.
    elimepsilon(A)                Builds a new automaton by eliminating the epsilon transitions.
    trim(A)                       Builds a new automaton by eliminating the tates that are not accessible or co-accessible.
    union(A1,A2)                  Builds a new automaton by making a nondeterministic union.
    intersection(A1,A2)           Builds a new automaton by making an intersection.
    concatenation(A1,A2)          Builds a new automaton by making a concatenation.
    kleene(A)                     Builds a new automaton by applying a Kleene star.
    mccluskey(A)                  Uses the Brzozowski-McCluskey algorithm to compute a regular expression.
    brzozowski(A)                 Uses the Brzozowski algorithm to compute a minimal NFA.
    hopcroft(A)                   Uses the Hopcroft algorithm to compute a minimal NFA.
    nfarandom(<n1>,<n2>,<n3>)     Builds a random NFA over an alphabet of size <n1> with at least <n2> states and at most <n3> states.
    dfarandom(<n1>,<n2>,<n3>)     Builds a random complete DFA over an alphabet of size <n1> with at least <n2> states and at most <n3> states.
    run(A,"<word>")               Computes the set of states reached with an input word.
    iextension(A)                 Applies inverse extension to the automaton (does not return an object).
    dyckextension(A)              Builds a new automaton via Dyck extension.
    counterfree(A)                Tests if the automaton is a counterfree DFA.
    permutation(A)                Tests if the automaton is a permutation DFA.

## Using morphisms variables

### Manipulation of a morphism stored in a variable M

    M                             Displays the Green relations of the monoid.
    rcayley(M)                    Displays the right Cayley graph of the morphism.
    lcayley(M)                    Displays the left Cayley graph of the morphism.
    multiplication(M)             Displays the multiplication table of the monoid.
    order(M)                      Displays the ordering defined on the monoid.
    idempotents(M)                Displays the idempotents of the monoid.
    mkernel(M)                    Displays the MOD-kernel of the morphism.
    akernel(M)                    Displays the AMT-kernel of the morphism.
    gkernel(M)                    Displays the GR-kernel of the morphism.
    orbit(𝒞,M)                    Displays the 𝒞-orbits for the morphism (implemented for 𝒞 = DD, MOD⁺, AT).
    orbit(𝒞,M,e)                  Displays the 𝒞-orbit of the idempotent e for the morphism (implemented for 𝒞 = DD, MOD⁺, AT).
    orbit(OP,𝒞,M)                 Displays the OP(𝒞)-orbits for the morphism (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR, GR⁺).
    orbit(OP,𝒞,M,e)               Displays the OP(𝒞)-orbits of the idempotent e for the morphism (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR, GR⁺).
    image(M,"<word>")             Computes the image of an input word.

## Membership tests

    - membership(𝒞,<object>)                            
          Checks if the language recognized by the object belongs to 𝒞 (example: membership(Pol(GR),L)).
    - exall(𝒞,𝒟,<n1>,<n2>)                              
          Computes and stores in variables all DFAs with at most <n1> states (plus a sink state) 
          and <n2> letters which are outside 𝒞 and inside 𝒟.
    - exall(out(𝒞₁,..,𝒞ₙ),in(𝒟₁,..,𝒟ₘ),<n1>,<n2>)
          Computes and stores in variables all DFAs with at most <n1> states (plus a sink state) 
          and <n2> letters which are outside 𝒞₁,..,𝒞ₙ and inside 𝒟₁,..,𝒟ₘ.

### Available classes 𝒞

    - ST      Trivial class.
    - DD      Well-suited extension of ST (DD = ST⁺).
    - MOD     Modulo languages.
    - MODP    Well-suited extension of the modulo languages (MODP = MOD⁺).
    - AMT     Alphabet modulo testable languages.
    - AMTP    Well-suited extension of the alphabet modulo languages (AMTP = AMT⁺).
    - GR      Group languages.
    - GRP     Well-suited extension of the group languages (GRP = GR⁺).
    - AT      Alphabet testable languages.
    - ATT     Alphabet threshold testable languages.
    - LT      Locally testable languages.
    - LTT     Locally threshold testable languages.
    - PPT     Positive piecewise testable languages.
    - PT      Piecewise testable languages.
    - SF      Star-free languages.
    - UL      Unambiguous languages.

### Available operators OP

    - Pol     Polynomial closure : 𝒞 ↦ Pol(𝒞).
    - BPol    Boolean polynomial closure : 𝒞 ↦ BPol(𝒞).
    - UBPol   Combined operator : 𝒞 ↦ UPol(BPol(𝒞)).
    - Pol2    Combined operator : 𝒞 ↦ Pol(BPol(𝒞)).
    - BPol2   Combined operator : 𝒞 ↦ BPol(BPol(𝒞)).
    - UBPol2  Combined operator : 𝒞 ↦ UPol(BPol(BPol(𝒞))).
    - FL      Future unary temporal logic closure : 𝒞 ↦ FL(𝒞).
    - FL2     Combined operator : 𝒞 ↦ FL(FL(𝒞)).
    - PL      Past unary temporal logic closure : 𝒞 ↦ PL(𝒞).
    - PL2     Combined operator : 𝒞 ↦ PL(PL(𝒞)).
    - TL      Unary temporal logic closure (𝒞 ↦ TL(𝒞)).
    - TL2     Combined operator : 𝒞 ↦ TL(TL(𝒞)).
    - SF      Star-free closure : 𝒞 ↦ SF(𝒞).

    chierarchies(<object>)        Summary of membership tests for the language recognized by the object in concatenation hierarchies.
    nhierarchies(𝒞,<object>)      If the language recognized by the object belongs to TL(𝒞), determines its level in the negation hierarchy of TL(𝒞).
    fphierarchies(𝒞,<object>)     If the language recognized by the object belongs to TL(𝒞), determines its level in the future/past hierarchy of TL(𝒞).

### Available input classes 𝒞 for negation hierarchies and future/past hierarchies

    - ST      Trivial class.
    - DD      Well-suited extension of ST (DD = ST⁺).
    - MOD     Modulo languages.
    - MODP    Well-suited extension of the modulo languages (MODP = MOD⁺).
    - GR      Group languages.

## Separation tests

    - separ(𝒞,<o1>,<o2>,(info))     Checks if <o1> is 𝒞-separable from <o2> ("info" is an optional parameter, it asks for more details).

### Available classes 𝒞

    - ST      Trivial class.
    - MOD     Modulo languages.
    - GR      Group languages.

### Available operators OP

    - Pol     Polynomial closure (𝒞 ↦ Pol(𝒞)).

