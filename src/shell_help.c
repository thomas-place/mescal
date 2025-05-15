#include "shell_help.h"

#define PAD1 30
#define PAD2 33
#define PAD3 70
#define PADC 8

void help(void) {
    // FILE *p = popen("less", "w");
    // if (p == NULL)
    FILE *p = stdout;

    print_dtitle_box(100, true, p, 1, "\"Welcome to the help of the MeSCaL program! ");
    print_title_box(100, true, p, 1, "Basic commands");
    fprintf(p, "\n#### General commands:\n\n");
    fprintf(p, "%4s%-*sThis help.\n", "", PAD1, "help");
    fprintf(p, "%4s%-*sDisplays the current timeout.\n", "", PAD1, "timeout");
    fprintf(p, "%4s%-*sSets the timeout to <value> seconds (0 to cancel timeout).\n", "", PAD1, "timeout = <value>");
    fprintf(p, "%4s%-*sDisplays the current limit.\n", "", PAD1, "limit");
    fprintf(p, "%4s%-*sSets the limit to <value>.\n", "", PAD1, "limit = <value>");
    fprintf(p, "%4s%-*sDisplays the current history size.\n", "", PAD1, "history");
    fprintf(p, "%4s%-*sSets the history size to <value> entries.\n", "", PAD1, "history = <value>");
    fprintf(p, "%4s%-*sCalls the test() function.\n", "", PAD1, "test");
    fprintf(p, "%4s%-*sQuits.\n", "", PAD1, "quit or exit");
    fprintf(p, "%4s%-*sLists all classes recognized by the program.\n", "", PAD1, "classes");

    fprintf(p, "\n#### Using variables:\n\n");

    fprintf(p, "%4s%-*sStores a copy of the returned object in the variable.\n", "", PAD1, "<variable> = <command>");
    fprintf(p, "%4s%-*sDeletes object.\n", "", PAD1, "delete(<variable>)");

    fprintf(p, "\n%8sVariable types: regular expression, recursive definition of regular expressions, automaton and morphism.\n", "");
    fprintf(p, "%8sVariable names: <upper><upper|digits>*\n", "");

    fprintf(p, "\n#### Filtering commands:\n\n");
    fprintf(p, "%4s%-*sLists all stored objects according to the parameters.\n", "", PAD1, "list(<parameters>)");
    fprintf(p, "%4s%-*sLists all stored regular expressions according to the parameters.\n", "", PAD1, "regexps(<parameters>)");
    fprintf(p, "%4s%-*sLists all stored automata according to the parameters.\n", "", PAD1, "automata(<parameters>)");
    fprintf(p, "%4s%-*sLists all stored morphisms according to the parameters.\n", "", PAD1, "morphisms(<parameters>)");
    fprintf(p, "%4s%-*sLists all stored recursive definitions according to the parameters.\n\n", "", PAD1, "regexps(<parameters>)");

    fprintf(p, "%8sAvailable parameters:\n", "");
    fprintf(p, "%10s%-*sdisplays the size of the syntactic monoid.\n", "", PAD1, "-syntactic");
    fprintf(p, "%10s%-*sonly lists the objects with a syntactic monoid whose size is between n1 and n2.\n", "", PAD1, "-syntactic(<n1>,<n2>)");
    fprintf(p, "%10s%-*sdisplays the number of idempotents in the syntactic monoid.\n", "", PAD1, "-idempotents");
    fprintf(p, "%10s%-*sonly lists the objects with a syntactic monoid has between n1 and n2 idempotents.\n", "", PAD1, "-idempotents(<n1>,<n2>)");
    fprintf(p, "%10s%-*sdisplays the size of the minimal automaton.\n", "", PAD1, "-minimal");
    fprintf(p, "%10s%-*sonly lists the objects with a minimal automaton whose size is between n1 and n2.\n\n", "", PAD1, "-minimal(<n1>,<n2>)");

    fprintf(p, "\n#### Sorting commands:\n\n");
    fprintf(p, "%4s%-*sSorts all objects, lexicographically by variable name.\n", "", PAD1, "sort");
    fprintf(p, "%4s%-*sSorts all objects, according to the ordering given by the parameter.\n\n", "", PAD1, "sort(<parameter>)");
    fprintf(p, "%8sAvailable parameters:\n", "");
    fprintf(p, "%10s%-*ssize of the syntactic monoid\n", "", PAD3, "-syntactic");
    fprintf(p, "%10s%-*ssize of the minimal automaton\n", "", PAD3, "-minimal");

    fprintf(p, "\n#### Saving and loading:\n\n");

    fprintf(p, "%4s%-*sSaves the object returned by a command in a file.\n", "", PAD1, "save(<command>,\"<filename>\")");
    fprintf(p, "%4s%-*sOpens and returns an object stored in a file.\n", "", PAD1, "open(\"<filename>\")");
    fprintf(p, "%4s%-*sSaves all current objects in a file.\n", "", PAD1, "savesession(\"<filename>\")");
    fprintf(p, "%4s%-*sLoads all current objects from a file.\n", "", PAD1, "loadsession(\"<filename>\")");
    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Using regular expressions variables");
    fprintf(p, "\n#### Definition, of a regular expression\n\n");
    fprintf(p, "%8s\"<regexp>\"\n\n", "");
    fprintf(p, "%8sSyntax of regexps:\n", "");
    fprintf(p, "%12se := e + e | e e | e* | !e | (e) | e & e | \\1 | \\0\n", "");
    fprintf(p, "%8swhere,\n", "");
    fprintf(p, "%12s0 = ∅, 1 = ε, ! = complement, & = ∩\n\n", "");
    fprintf(p, "\n#### Manipulation of a regular expression stored in a variable L:\n\n");
    fprintf(p, "%4s%-*sDisplays the regular expression.\n", "", PAD1, "L");
    fprintf(p, "%4s%-*sThe minimal automaton: works as an automata variable.\n", "", PAD1, "L.minimal");
    fprintf(p, "%4s%-*sThe syntactic morphism: works as a morphism variable.\n", "", PAD1, "L.syntactic");
    fprintf(p, "%4s%-*sUses the Thomson algorithm to compute a NFA.\n", "", PAD1, "thomson(L)");
    fprintf(p, "%4s%-*sUses the Glushkov algorithm to compute a NFA.\n", "", PAD1, "glushkov(L)");

    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Using recurvive definitions variables");

    fprintf(p, "\n#### Initialization:\n\n");
    fprintf(p, "%4s%-*sReturns an initialized recursive definition variable.\n", "", PAD1, "initrecursion(<n>,<v1>,...)");
    fprintf(p, "%4s%-*s<n> is the largest index that has to be specified by the user.\n", "", PAD1, "");
    fprintf(p, "%4s%-*s<v1>,... is a list of variable names denoting the relations used in the recursive definition.\n", "", PAD1, "");
    fprintf(p, "%4s%-*sExample: R = initrecursion(0,L)\n\n", "", PAD1, "");

    fprintf(p, "\n#### Setup of an initialized recursive definition variable R:\n\n");

    fprintf(p, "%4s%-*sDefines the initial case <n> of the relation <v>.\n", "", PAD1, "E.<v>[<n>] = <regexp>");
    fprintf(p, "%4s%-*s(<n> must be smaller than the maximum index specified in the initialization).\n", "", PAD1, "");
    fprintf(p, "%4s%-*sExample: R.L[0] = \"\\1\"\n\n", "", PAD1, "");
    fprintf(p, "%4s%-*sDefines the recursion of the relation <v>.\n", "", PAD1, "E.<v>[i] = <symbolic regexp>");
    fprintf(p, "%4s%-*sThe regular expression can use symbolic variables and letters.\n", "", PAD1, "");
    fprintf(p, "%4s%-*sExample: R.L[i] = \"(a{L}[i-1]b)*\"\n\n", "", PAD1, "");

    fprintf(p, "%8sSyntax of symbolic regexps:\n", "");
    fprintf(p, "%12se := e + e | e e | e* | !e | (e) | e & e | \\1 | \\0 | a[i - <n>] | {<v>}[i - <n>]\n", "");
    fprintf(p, "%8swhere,\n", "");
    fprintf(p, "%12s0 = ∅, 1 = ε, ! = complement, & = ∩, a[i - <n>] = symbolic letter, {<v>}[i - <n>] = symbolic variable\n", "");

    fprintf(p, "\n#### Usage of a recursive definition variable R:\n\n");
    fprintf(p, "%4s%-*sDisplays the recursive definition and checks if it is well-defined.\n", "", PAD1, "R");
    fprintf(p, "%4s%-*sReturns the regular expression of index <n> for the relation <v>.\n", "", PAD1, "R.<v>[<n>]");
    fprintf(p, "%4s%-*s(can be copied into a regular expression variable).\n", "", PAD1, "");

    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Using automata variables");
    fprintf(p, "\n#### Manipulation of an automaton stored in a variable A:\n\n");
    fprintf(p, "%4s%-*sDisplays the automaton.\n", "", PAD1, "A");
    fprintf(p, "%4s%-*sThe minimal automaton: works as an automata variable.\n", "", PAD1, "A.minimal");
    fprintf(p, "%4s%-*sThe syntactic morphism: works as a morphism variable.\n", "", PAD1, "A.syntactic");
    fprintf(p, "%4s%-*sBuilds a new automaton by eliminating the epsilon transitions.\n", "", PAD1, "elimepsilon(A)");
    fprintf(p, "%4s%-*sBuilds a new automaton by eliminating the tates that are not accessible or co-accessible.\n", "", PAD1, "trim(A)");
    fprintf(p, "%4s%-*sBuilds a new automaton by making a nondeterministic union.\n", "", PAD1, "union(A1,A2)");
    fprintf(p, "%4s%-*sBuilds a new automaton by making an intersection.\n", "", PAD1, "intersection(A1,A2)");
    fprintf(p, "%4s%-*sBuilds a new automaton by making a concatenation.\n", "", PAD1, "concatenation(A1,A2)");
    fprintf(p, "%4s%-*sBuilds a new automaton by applying a Kleene star.\n", "", PAD1, "kleene(A)");
    fprintf(p, "%4s%-*sUses the Brzozowski-McCluskey algorithm to compute a regular expression.\n", "", PAD1, "mccluskey(A)");
    fprintf(p, "%4s%-*sUses the Brzozowski algorithm to compute a minimal NFA.\n", "", PAD1, "brzozowski(A)");
    fprintf(p, "%4s%-*sUses the Hopcroft algorithm to compute a minimal NFA.\n", "", PAD1, "hopcroft(A)");
    fprintf(p, "%4s%-*sBuilds a random NFA over an alphabet of size <n1> with at least <n2> states and at most <n3> states.\n", "", PAD1, "nfarandom(<n1>,<n2>,<n3>)");
    fprintf(p, "%4s%-*sBuilds a random complete DFA over an alphabet of size <n1> with at least <n2> states and at most <n3> states.\n", "", PAD1, "dfarandom(<n1>,<n2>,<n3>)");
    fprintf(p, "%4s%-*sComputes the set of states reached with an input word.\n", "", PAD1, "run(A,\"<word>\")");
    fprintf(p, "%4s%-*sApplies inverse extension to the automaton (does not return an object).\n", "", PAD1, "iextension(A)");
    fprintf(p, "%4s%-*sBuilds a new automaton via Dyck extension.\n", "", PAD1, "dyckextension(A)");
    fprintf(p, "%4s%-*sTests if the automaton is a counterfree DFA.\n", "", PAD1, "counterfree(A)");
    fprintf(p, "%4s%-*sTests if the automaton is a permutation DFA.\n", "", PAD1, "permutation(A)");

    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Using morphisms variables");
    fprintf(p, "\n#### Manipulation of a morphism stored in a variable M:\n\n");
    fprintf(p, "%4s%-*sDisplays the Green relations of the monoid.\n", "", PAD1, "M");
    fprintf(p, "%4s%-*sDisplays the right Cayley graph of the morphism.\n", "", PAD1, "rcayley(M)");
    fprintf(p, "%4s%-*sDisplays the left Cayley graph of the morphism.\n", "", PAD1, "lcayley(M)");
    fprintf(p, "%4s%-*sDisplays the multiplication table of the monoid.\n", "", PAD1, "multiplication(M)");
    fprintf(p, "%4s%-*sDisplays the ordering defined on the monoid.\n", "", PAD1, "order(M)");
    fprintf(p, "%4s%-*sDisplays the idempotents of the monoid.\n", "", PAD1, "idempotents(M)");
    fprintf(p, "%4s%-*sDisplays the MOD-kernel of the morphism.\n", "", PAD1, "mkernel(M)");
    fprintf(p, "%4s%-*sDisplays the AMT-kernel of the morphism.\n", "", PAD1, "akernel(M)");
    fprintf(p, "%4s%-*sDisplays the GR-kernel of the morphism.\n", "", PAD1, "gkernel(M)");
    fprintf(p, "%4s%-*sDisplays the 𝒞-orbits for the morphism (implemented for 𝒞 = DD, MOD⁺, AT).\n", "", PAD2, "orbit(𝒞,M)");
    fprintf(p, "%4s%-*sDisplays the 𝒞-orbit of the idempotent e for the morphism (implemented for 𝒞 = DD, MOD⁺, AT).\n", "", PAD2, "orbit(𝒞,M,e)");
    fprintf(p, "%4s%-*sComputes the image of an input word.\n", "", PAD1, "image(M,\"<word>\")");

    fprintf(p, "\n");

    print_title_box(100, true, p, 1, "Membership tests");

    fprintf(p, "%4s%-*sChecks if the language recognized by the object belongs to 𝒞 (example: membership(Pol(GR),L)).\n\n", "", 53, "membership(𝒞,<object>)");

    fprintf(p, "%6sAvailable classes 𝒞:\n", "");
    fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
    fprintf(p, "%8s%-*sWell-suited extension of ST (DD = ST⁺).\n", "", PADC, "DD");
    fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
    fprintf(p, "%8s%-*sWell-suited extension of the modulo languages (MODP = MOD⁺).\n", "", PADC, "MODP");
    fprintf(p, "%8s%-*sAlphabet modulo testable languages.\n", "", PADC, "AMT");
    fprintf(p, "%8s%-*sWell-suited extension of the alphabet modulo languages (AMTP = AMT⁺).\n", "", PADC, "AMTP");
    fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");
    fprintf(p, "%8s%-*sWell-suited extension of the group languages (GRP = GR⁺).\n", "", PADC, "GRP");
    fprintf(p, "%8s%-*sAlphabet testable languages.\n", "", PADC, "AT");
    fprintf(p, "%8s%-*sAlphabet threshold testable languages.\n", "", PADC, "ATT");
    fprintf(p, "%8s%-*sLocally testable languages.\n", "", PADC, "LT");
    fprintf(p, "%8s%-*sLocally threshold testable languages.\n", "", PADC, "LTT");
    fprintf(p, "%8s%-*sPositive piecewise testable languages.\n", "", PADC, "PPT");
    fprintf(p, "%8s%-*sPiecewise testable languages.\n", "", PADC, "PT");
    fprintf(p, "%8s%-*sStar-free languages.\n", "", PADC, "SF");
    fprintf(p, "%8s%-*sUnambiguous languages.\n", "", PADC, "UL");

    fprintf(p, "\n");

    fprintf(p, "%6sAvailable operators OP\n", "");
    fprintf(p, "%8s%-*sPolynomial closure : 𝒞 ↦ Pol(𝒞).\n", "", PADC, "Pol");
    fprintf(p, "%8s%-*sBoolean polynomial closure : 𝒞 ↦ BPol(𝒞).\n", "", PADC, "BPol");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ UPol(BPol(𝒞)).\n", "", PADC, "UBPol");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ Pol(BPol(𝒞)).\n", "", PADC, "Pol2");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ BPol(BPol(𝒞)).\n", "", PADC, "BPol2");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ UPol(BPol(BPol(𝒞))).\n", "", PADC, "UBPol2");
    fprintf(p, "%8s%-*sFuture unary temporal logic closure : 𝒞 ↦ FL(𝒞).\n", "", PADC, "FL");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ FL(FL(𝒞)).\n", "", PADC, "FL2");
    fprintf(p, "%8s%-*sPast unary temporal logic closure : 𝒞 ↦ PL(𝒞).\n", "", PADC, "PL");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ PL(PL(𝒞)).\n", "", PADC, "PL2");
    fprintf(p, "%8s%-*sUnary temporal logic closure (𝒞 ↦ TL(𝒞)).\n", "", PADC, "TL");
    fprintf(p, "%8s%-*sCombined operator : 𝒞 ↦ TL(TL(𝒞)).\n", "", PADC, "TL2");
    fprintf(p, "%8s%-*sStar-free closure : 𝒞 ↦ SF(𝒞).\n", "", PADC, "SF");

    fprintf(p, "\n");

    fprintf(p, "%4s%-*sSummary of membership tests for the language recognized by the object in concatenation hierarchies.\n", "", PAD1, "chierarchies(<object>)");
    fprintf(p, "%4s%-*sIf the language recognized by the object belongs to TL(𝒞), determines its level in the negation hierarchy of TL(𝒞).\n", "", PAD2, "neghierarchies(𝒞,<object>)");
    fprintf(p, "%4s%-*sIf the language recognized by the object belongs to TL(𝒞), determines its level in the future/past hierarchy of TL(𝒞).\n\n", "", PAD2, "fphierarchies(𝒞,<object>)");
    fprintf(p, "%6sAvailable input classes 𝒞 for negation hierarchies and future/past hierarchies:\n", "");
    fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
    fprintf(p, "%8s%-*sWell-suited extension of ST (DD = ST⁺).\n", "", PADC, "DD");
    fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
    fprintf(p, "%8s%-*sWell-suited extension of the modulo languages (MODP = MOD⁺).\n", "", PADC, "MODP");
    fprintf(p, "%8s%-*sAlphabet modulo testable languages.\n", "", PADC, "AMT");
    fprintf(p, "%8s%-*sWell-suited extension of the alphabet modulo languages (AMTP = AMT⁺).\n", "", PADC, "AMTP");
    fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");
    fprintf(p, "%8s%-*sWell-suited extension of the group languages (GRP = GR⁺).\n", "", PADC, "GRP");

    fprintf(p, "\n");

    fprintf(p, "%4s%-*sComputes and stores in variables all DFAs with at most <n1> states (plus a sink state) and <n2> letters which are outside 𝒞 and inside 𝒟.\n", "", 56, "exall(𝒞,𝒟,<n1>,<n2>)");
    fprintf(p, "%4s%-*sComputes and stores in variables all DFAs with at most <n1> states (plus a sink state) and <n2> letters which are outside 𝒞₁,..,𝒞ₙ and inside 𝒟₁,..,𝒟ₘ.\n", "", 70,
            "exall(out(𝒞₁,..,𝒞ₙ),in(𝒟₁,..,𝒟ₘ),<n1>,<n2>)");
    fprintf(p, "%4s%-*sComputes and stores in variables all DFAs with at most <n1> states (plus a sink state) and <n2> letters which are at level <n0> in the negation hierarchy of TL(𝒞).\n", "", 53,
            "negexall(𝒞,<n0>,<n1>,<n2>)");
    fprintf(p, "%4s%-*sComputes and stores in variables all DFAs with at most <n1> states (plus a sink state) and <n2> letters which are at level <n0> in the future/past hierarchy of TL(𝒞).\n\n", "",
            53, "fpexall(𝒞,<n0>,<n1>,<n2>)");

    print_title_box(100, true, p, 1, "Separation tests");

    fprintf(p, "%4s%-*sChecks if <o1> is 𝒞-separable from <o2> (\"info\" is an optional parameter, it asks for more details).\n\n", "", PAD2, "separ(𝒞,<o1>,<o2>,(info))");
    fprintf(p, "%6sAvailable classes 𝒞:\n", "");
    fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
    fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
    fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");

    fprintf(p, "\n");

    fprintf(p, "%6sAvailable operators OP:\n", "");
    fprintf(p, "%8s%-*sPolynomial closure (𝒞 ↦ Pol(𝒞)).\n", "", PADC, "Pol");
    fprintf(p, "\n");

    // if (p != stdout)
    // fclose(p);
}

static void print_class_info_status(classes class, FILE *out) {
    if (class_infos[class]) {
        class_infos[class](out);

        print_dmid_line(100, out);
        if (class_membership[class]) {
            print_dline_box(109, out, " Membership : " ANSI_COLOR_GREEN "Implemented.   " ANSI_COLOR_RESET);
        } else {
            print_dline_box(109, out, " Membership : " ANSI_COLOR_RED "Not implemented." ANSI_COLOR_RESET);
        }
        if (class_separation[class]) {
            print_dline_box(109, out, " Separation : " ANSI_COLOR_GREEN "Implemented." ANSI_COLOR_RESET);
        } else {
            print_dline_box(109, out, " Separation : " ANSI_COLOR_RED "Not implemented." ANSI_COLOR_RESET);
        }
        print_dbot_line(100, out);
    }
}

void print_classes(void) {
    for (uint i = 0; i < CL_END; i++) {
        print_class_info_status(i, stdout);
    }
}
