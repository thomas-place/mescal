#include "shell_help.h"

#define PAD1 30
#define PAD2 33
#define PADC 8

void help(void)
{
        // FILE *p = popen("less", "w");
        // if (p == NULL)
        FILE* p = stdout;

        print_dtitle_box(100, true, p, 1, "\"Welcome to the help of the MeSCaL program! ");
        print_title_box(100, true, p, 1, "Basic commands");
        fprintf(p, "%4s%-*sThis help.\n", "", PAD1, "help");
        fprintf(p, "%4s%-*sQuits.\n", "", PAD1, "quit or exit");
        fprintf(p, "%4s%-*sLists stored languages.\n", "", PAD1, "languages");
        fprintf(p, "%4s%-*sLists stored automata.\n", "", PAD1, "automata");
        fprintf(p, "%4s%-*sLists stored morphisms.\n", "", PAD1, "morphisms");
        fprintf(p, "%4s%-*sSorts all objects, lexicographically by name.\n", "", PAD1, "sort");
        fprintf(p, "%4s%-*sDeletes object (language, automaton or morphism).\n", "", PAD1, "delete(<variable>)");
        fprintf(p, "%4s%-*sSaves all current objects in a file.\n", "", PAD1, "savesession(\"<filename>\")");
        fprintf(p, "%4s%-*sLoads all current objects from a file.\n", "", PAD1, "loadsession(\"<filename>\")");
        fprintf(p,
                "\n"
                "%4sVariable names: <upper><upper|digits>*\n", ""
        );
        fprintf(p, "\n");
        print_title_box(100, true, p, 1, "Using language variables");
        fprintf(p, "\n#### Definition, storing a language in a variable:\n\n");
        fprintf(p, "%4s## Specification by a regular expression:\n\n", "");
        fprintf(p, "%8s<variable> = \"<regexp>\"\n\n", "");
        fprintf(p, "%8sSyntax of regexps:\n", "");
        fprintf(p, "%12se := e + e | e e | e* | !e | (e) | e & e | [<carac>] | 1 | 0\n", "");
        fprintf(p, "%8swhere,\n", "");
        fprintf(p, "%12s0 = ∅, 1 = ε, ! = complement, & = ∩, [d] = a + b + c + d.\n\n", "");
        fprintf(p, "%4s## Specification by an automaton:\n\n", "");
        fprintf(p, "%8s<variable> = link(<variable_name_of_automaton>)\n", "");
        fprintf(p, "%8sCreates a link with the NFA in the automata variable A (A becomes protected).\n", "");
        fprintf(p, "\n#### Manipulation of languages:\n\n");
        fprintf(p, "%4s%-*sDisplays the regular expression or the automaton used to define L.\n", "", PAD1, "L");
        fprintf(p, "%4s%-*sThe minimal automaton of L: works as an automata variable.\n", "", PAD1, "L.minimal");
        fprintf(p, "%4s%-*sThe syntactic morphism of L: works as a morphism variable.\n", "", PAD1, "L.syntactic");

        fprintf(p, "\n");

        print_title_box(100, true, p, 1, "Using automata variables");
        fprintf(p, "\n#### Definition, storing an automaton in a variable A:\n\n");
        fprintf(p, "%4s%-*sMakes a copy of the automaton B.\n", "", PAD1, "A = B");
        fprintf(p, "%4s%-*sOpens the NFA stored in a file.\n", "", PAD1, "open(\"filename\")");
        fprintf(p, "%4s%-*sThomson's algorithm (L must be a language specified by a regexp).\n", "", PAD1, "thomson(L)");
        fprintf(p, "%4s%-*sElimination of the epsilon transitions in the automaton B.\n", "", PAD1, "elimepsilon(B)");
        fprintf(p, "%4s%-*sElimination of all states that are not accessible or co-accessible in the automaton B.\n", "", PAD1, "trim(B)");
        fprintf(p, "%4s%-*sNondeterministic union of the automata B1 and B2.\n", "", PAD1, "union(B1,B2)");
        fprintf(p, "%4s%-*sIntersection of the automata B1 and B2.\n", "", PAD1, "intersection(B1,B2)");
        fprintf(p, "%4s%-*sConcatenation of the automata B1 and B2.\n", "", PAD1, "concatenation(B1,B2)");
        fprintf(p, "%4s%-*sSubset construction applied to the automaton B.\n", "", PAD1, "kleene(B1,B2)");
        fprintf(p, "%4s%-*sMinimization applied to the automaton B.\n", "", PAD1, "minimal(B1,B2)");

        fprintf(p, "\n#### Manipulation:\n\n");
        fprintf(p, "%4s%-*sDisplays the automaton.\n", "", PAD1, "A");
        fprintf(p, "%4s%-*sComputes the set of states reached with an input word.\n", "", PAD1, "A.run(word)");
        fprintf(p, "%4s%-*sSaves the automaton A in a file.\n", "", PAD1, "save(A,\"filename\")");



        print_title_box(100, true, p, 1, "Using morphisms variables");
        fprintf(p, "\n#### Manipulation:\n\n");
        fprintf(p, "%4s%-*sDisplays the Green relations of M.\n", "", PAD1, "M");
        fprintf(p, "%4s%-*sThe right Cayley graph of M.\n", "", PAD1, "M.rcayley");
        fprintf(p, "%4s%-*sThe left Cayley graph of M.\n", "", PAD1, "M.lcayley");
        fprintf(p, "%4s%-*sThe multiplication table of M.\n", "", PAD1, "M.multiplication");
        fprintf(p, "%4s%-*sComputes the image of a word.\n", "", PAD1, "M.image(word)");
        fprintf(p, "%4s%-*sThe ordering of M.\n", "", PAD1, "M.order");
        fprintf(p, "%4s%-*sThe idempotents of M.\n", "", PAD1, "M.idempotents");
        fprintf(p, "%4s%-*sThe 𝒞-kernel of M (implemented for 𝒞 = MOD, GR).\n", "", PAD2, "M.kernel(𝒞)");
        fprintf(p, "%4s%-*sThe 𝒞-orbits for M (implemented for 𝒞 = DD, MOD⁺, AT).\n", "", PAD2, "M.orbits(𝒞)");
        fprintf(p, "%4s%-*sThe 𝒞-orbit of the idempotent e for M (implemented for 𝒞 = DD, MOD⁺, AT).\n", "", PAD2, "M.orbit(𝒞,e)");
        fprintf(p, "%4s%-*sThe OP(𝒞)-orbits for M (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR).\n", "", PAD2, "M.orbits(OP,𝒞)");
        fprintf(p, "%4s%-*sThe OP(𝒞)-orbits of the idempotent e for M (implemented for OP = BPol, TL and 𝒞 = ST, DD, MOD, MOD⁺, GR).\n", "", PAD2, "M.orbit(OP,𝒞,e)");
        fprintf(p, "\n");

        print_title_box(100, true, p, 1, "Membership tests");

        fprintf(p, "\n#### Commands:\n\n");
        fprintf(p, "%4s%-*sChecks if the language L belongs to 𝒞.\n\n", "", PAD2, "membership(𝒞,L)");
        fprintf(p, "%6sAvailable classes 𝒞:\n", "");
        fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
        fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
        fprintf(p, "%8s%-*sAlphabet modulo testable languages.\n", "", PADC, "AMT");
        fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");
        fprintf(p, "%8s%-*sAlphabet testable languages.\n", "", PADC, "AT");
        fprintf(p, "%8s%-*sAlphabet threshold testable languages.\n", "", PADC, "ATT");
        fprintf(p, "%8s%-*sLocally testable languages.\n", "", PADC, "LT");
        fprintf(p, "%8s%-*sLocally threshold testable languages.\n", "", PADC, "LTT");
        fprintf(p, "%8s%-*sPositive piecewise testable languages.\n", "", PADC, "PPT");
        fprintf(p, "%8s%-*sPiecewise testable languages.\n", "", PADC, "PT");
        fprintf(p, "%8s%-*sStar-free languages.\n", "", PADC, "SF");
        fprintf(p, "%8s%-*sUnambiguous languages.\n", "", PADC, "UL");

        fprintf(p, "\n");

        fprintf(p, "%4s%-*sChecks if the language L belongs to OP(𝒞).\n\n", "", PAD2, "membership(OP,𝒞,L)");
        fprintf(p, "%6sAvailable input classes 𝒞:\n", "");
        fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
        fprintf(p, "%8s%-*sWell-suited extension of ST (DD = ST⁺).\n", "", PADC, "DD");
        fprintf(p, "%8s%-*sAlphabet testable languages.\n", "", PADC, "AT");
        fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
        fprintf(p, "%8s%-*sWell-suited extension of the modulo languages (MODP = MOD⁺).\n", "", PADC, "MODP");
        fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");

        fprintf(p, "\n");

        fprintf(p, "%6sAvailable operators OP\n", "");
        fprintf(p, "%8s%-*sPolynomial closure (𝒞 ↦ Pol(𝒞)).\n", "", PADC, "Pol");
        fprintf(p, "%8s%-*sBoolean polynomial closure (𝒞 ↦ BPol(𝒞))\n", "", PADC, "BPol");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ UPol(BPol(𝒞)) (does not work with AT).\n", "", PADC, "UBPol");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ Pol(BPol(𝒞)) (does not work with AT).\n", "", PADC, "Pol2");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ UPol(BPol(BPol(𝒞))) (does not work with AT).\n", "", PADC, "UBPol2");
        fprintf(p, "%8s%-*sFuture unary temporal logic closure (𝒞 ↦ FL(𝒞)).\n", "", PADC, "FL");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ FL(FL(𝒞)) (does not work with AT).\n", "", PADC, "FL2");
        fprintf(p, "%8s%-*sPast unary temporal logic closure (𝒞 ↦ PL(𝒞)).\n", "", PADC, "PL");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ PL(PL(𝒞)) (does not work with AT).\n", "", PADC, "PL2");
        fprintf(p, "%8s%-*sUnary temporal logic closure (𝒞 ↦ TL(𝒞)).\n", "", PADC, "TL");
        fprintf(p, "%8s%-*sCombined operator 𝒞 ↦ TL(TL(𝒞)) (does not work with AT).\n", "", PADC, "TL2");
        fprintf(p, "%8s%-*sStar-free closure.\n", "", PADC, "SF");

        fprintf(p, "\n");

        fprintf(p, "%4s%-*sSummary of membership tests for the language L in concatenation hierarchies.\n", "", PAD1, "concatenation(L)");
        fprintf(p, "%4s%-*sIf L belongs to TL(𝒞), determines its level in the negation hierarchy of TL(𝒞).\n", "", PAD2, "neghiera(𝒞,L)");
        fprintf(p, "%4s%-*sIf L belongs to TL(𝒞), determines its level in the future branch and the past branch of the future/past hierarchy of TL(𝒞).\n\n", "", PAD2, "fphiera(𝒞,L)");
        fprintf(p, "%6sAvailable input classes 𝒞 for negation hierarchies and future/past hierarchies:\n", "");
        fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
        fprintf(p, "%8s%-*sWell-suited extension of ST (DD = ST⁺).\n", "", PADC, "DD");
        fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
        fprintf(p, "%8s%-*sWell-suited extension of the modulo languages (MODP = MOD⁺).\n", "", PADC, "MODP");
        fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");

        fprintf(p, "\n");

        print_title_box(100, true, p, 1, "Separation tests");

        fprintf(p, "\n#### Commands:\n\n");

        fprintf(p, "%4s%-*sChecks if L1 is 𝒞-separable from L2 (\"info\" is an optional parameter for asking more details).\n\n", "", PAD2, "separ(𝒞,L1,L2,(info))");
        fprintf(p, "%6sAvailable classes 𝒞:\n", "");
        fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
        fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
        fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");

        fprintf(p, "\n");

        fprintf(p, "%4s%-*sChecks if L1 is OP(𝒞à-separable from L2 (\"info\" is an optional parameter for asking more details).\n\n", "", PAD2, "separ(OP,𝒞,L1,L2,(info))");
        fprintf(p, "%6sAvailable input classes 𝒞:\n", "");
        fprintf(p, "%8s%-*sTrivial class.\n", "", PADC, "ST");
        fprintf(p, "%8s%-*sModulo languages.\n", "", PADC, "MOD");
        fprintf(p, "%8s%-*sGroup languages.\n", "", PADC, "GR");
        fprintf(p, "\n");
        fprintf(p, "%6sAvailable operators OP\n", "");
        fprintf(p, "%8s%-*sPolynomial closure (𝒞 ↦ Pol(𝒞)).\n", "", PADC, "Pol");
        fprintf(p, "\n");

        // if (p != stdout)
        // fclose(p);
}
