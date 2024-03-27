/*****************************************/
/* Fonctions spécifiques sur les classes */
/*      (affichage principalement)       */
/*****************************************/

#include "shell_sclass.h"
#define MESSAGESIZE 200

/*********************/
/* Affichage partagé */
/*********************/

void print_info_input(int i, FILE *out)
{
    switch (objects[i]->type)
    {
    case LANGUAGE: {
        language *lang = objects[i]->theob.lan;
        print_title_box(10, true, out, 1, "Input: a regular language.");
        if (lang->type == SPE_REG)
        {
            fprintf(out, "#### Regular expression  : ");
            reg_print(lang->spe.reg);
        }
        else if (lang->type == SPE_NFA)
        {
            fprintf(out, "#### Specified by a NFA.\n");
        }
        morphism *morp = objects[lang->syntmor]->theob.mor;
        fprintf(out, "#### Syntactic monoid    :\n");
        print_full_green(morp->cayley, morp->green, false, out);
        fprintf(out, "#### Syntactic morphism  :\n");
        cayley_print_morphism(morp->cayley, out);
    }
    break;
    case MORPHISM: {
        morphism *morp = objects[i]->theob.mor;
        print_title_box(10, true, out, 1, "Input: a morphism into a finite monoid.");
        fprintf(out, "#### The monoid          :\n");
        print_full_green(morp->cayley, morp->green, false, out);
        fprintf(out, "#### The morphism        :\n");
        cayley_print_morphism(morp->cayley, out);
    }
    break;
    case AUTOMATA: {
        automata *thenfa = objects[i]->theob.aut;
        print_title_box(10, true, out, 1, "Input: an automaton.");
        fprintf(out, "#### The automaton  :\n");
        view_nfa(thenfa->nfa);
    }
    break;

    default:
        break;
    }
}

void print_start_comp(FILE *out, char *class, object_type thetype)
{
    char mess[MESSAGESIZE];
    switch (thetype)
    {
    case LANGUAGE:
        sprintf(mess, "Deciding membership of the input language in %s.", class);
        break;
    case MORPHISM:
        sprintf(mess, "Deciding whether the input is an %s-morphism.", class);
        break;
    default:
        return;
        break;
    }
    print_dtitle_box(10, true, out, 1, mess);
}

void print_conclusion_comp(FILE *out, bool res, char *namec, object_type thetype)
{
    char mess[MESSAGESIZE];
    switch (thetype)
    {
    case LANGUAGE:
        if (res)
        {
            sprintf(mess, "The input language is in %s.", namec);
        }
        else
        {
            sprintf(mess, "The input language is NOT in %s.", namec);
        }
        break;
    case MORPHISM:
        if (res)
        {
            sprintf(mess, "The input morphism is an %s-morphism.", namec);
        }
        else
        {
            sprintf(mess, "The input morphism is NOT an %s-morphism.", namec);
        }
        break;
    default:
        return;
        break;
    }

    print_dtitle_box(10, true, out, 1, mess);
}

/*********************************************/
/* Affichage des informations sur une classe */
/*********************************************/

// Alphabet testable
void print_infoclass_at(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership : alphabet testable languages (AT).");
    print_dline_box(0, out,
                    " Definition of AT  : Boolean combinations of languages of the form A*aA* where a ∊ A is a letter.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - One variable first-order logic (FO¹(∅)).");
    print_dline_box(0, out, "  - Languages with an idempotent and commutative syntactic monoid.");
    print_dbot_line(100, out);
}

// Alphabet threshold testable
void print_infoclass_att(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: alphabet threshold testable languages (ATT).");
    print_dline_box(0, out,
                    " Definition of ATT : Boolean combinations of languages of the form (A*aA*)ᵏ where a ∊ A is a letter");
    print_dline_box(0, out, "    and k ≥ 1 is a number.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with equality only (FO(∅)).");
    print_dline_box(0, out, "  - Languages with an aperiodic and commutative syntactic monoid.");
    print_dbot_line(100, out);
}

// Locally testable
void print_infoclass_lt(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: locally testable languages (LT).");
    print_dline_box(0, out,
                    " Definition of LT : Boolean combinations of languages of the form wA*, A*w and A*wA* where w ∊ A*");
    print_dline_box(0, out, "    is an arbitrary word.");
    print_dline_box(0, out, " Characterization :");
    print_dline_box(0, out,
                    "  - Languages with a syntactic morphism whose DD-orbits are idempotent and commutative.");
    print_dbot_line(100, out);
}

// Locally threshold testable
void print_infoclass_ltt(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: locally threshold testable languages (LTT).");
    print_dline_box(0, out,
                    " Definition of LTT : Boolean combinations of languages of the form wA*, A*w and F(w,k) where w ∊ A*");
    print_dline_box(0, out,
                    "    is an arbitrary word and k ≥ 1 (F(w,k) consists of all words with k occurrences of the infix w).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with successor (FO(+1)).");
    print_dline_box(0, out,
                    "  - Languages with an aperiodic syntactic semigroup S satisfying the equation erfsetf = etfserf");
    print_dline_box(0, out, "    for all elements r,s,t ∊ S and all idempotents e,f ∊ E(S).");
    print_dbot_line(100, out);
}

// Star-free
void print_infoclass_sf(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: star-free languages (SF).");
    print_dline_box(0, out,
                    " Definition of SF : Languages that can be defined by a star-free expression (the Kleene star is");
    print_dline_box(0, out, "    disallowed but complement is allowed instead).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with the linear order (FO(<)).");
    print_dline_box(0, out, "  - Linear temporal logic (LTL).");
    print_dline_box(0, out, "  - Languages with an aperiodic syntactic monoid.");
    print_dline_box(0, out, "  - Languages with a counter-free minimal automaton.");
    print_dbot_line(100, out);
}

// Group
void print_infoclass_gr(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: group languages (GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Languages with a syntactic monoid which is a group.");
    print_dline_box(0, out,
                    "  - Languages with a minimal automaton which is a permutation automaton.");
    print_dbot_line(100, out);
}

// Alphabet modulo
void print_infoclass_amt(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: alphabet modulo testable languages (AMT).");
    print_dline_box(0, out,
                    " Definition of AMT : Boolean combinations of languages which count the occurrences of some letter");
    print_dline_box(0, out, "    a ∊ A modulo an integer.");
    print_dline_box(0, out, " Characterization  :");
    print_dline_box(0, out, "  - Languages with a syntactic monoid which is a commuative group.");
    print_dbot_line(100, out);
}

// Modulo
void print_infoclass_mod(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: modulo languages (MOD).");
    print_dline_box(0, out,
                    " Definition of MOD : Finite unions of languages of the form (Aⁿ)*Aᵏ where n > k ≥ 1 (membership of");
    print_dline_box(0, out,
                    "    a word in the language depends only on its length modulo a fixed integer).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out,
                    "  - Languages with a syntactic morphism into a group which maps every a ∊ A to the same element.");
    print_dline_box(0, out,
                    "  - Languages with a minimal automaton which a permutation automaton in which every letter a ∊ A");
    print_dline_box(0, out, "    has the same action on the states.");
    print_dbot_line(100, out);
}

// Trivial class
void print_infoclass_st(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: trivial languages (ST).");
    print_dline_box(0, out,
                    " Definition of the class ST : trivial class consisting of the languages ∅ and A*.");
    print_dbot_line(100, out);
}

// Piecewise testable
void print_infoclass_pt(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: piecewise testable languages (PT).");
    print_dline_box(0, out,
                    " Definition of PT  : Boolean combinations of marked concatenations of the language A*.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the Straubing-Thérien hierarchy (BPol(ST)).");
    print_dline_box(0, out,
                    "  - Level BΣ₁(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Languages with a J-trivial syntactic monoid.");
    print_dbot_line(100, out);
}

// Positive piecewise testable
void print_infoclass_ppt(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: positive piecewise testable languages (PPT).");
    print_dline_box(0, out,
                    " Definition of PPT : Finite unions of marked concatenations of the language A*.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level ½ in the Straubing-Thérien hierarchy (Pol(ST)).");
    print_dline_box(0, out,
                    "  - Level Σ₁(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out,
                    "  - Languages with an syntactic ordered monoid M satisfying the equation 1 ≤ s for all s ∊ M.");
    print_dbot_line(100, out);
}

// Unambiguous
void print_infoclass_ul(FILE *out)
{
    print_dtitle_box(10, false, out, 1, "Membership: unambiguous languages (UL).");
    print_dline_box(0, out,
                    " Definition of UL  : Finite disjoint unions of unambiguous marked products of languages of the form");
    print_dline_box(0, out, "    B* where B ⊆ A is a sub-alphabet.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order (FO²(<)).");
    print_dline_box(0, out,
                    "  - Level Δ₂(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Unary temporal logic with finally and previously (TL = F + P).");
    print_dline_box(0, out, "  - Languages with a syntactic monoid in DA.");
    print_dbot_line(100, out);
}

/***********************************************/
/* Affichage des informations sur un opérateur */
/***********************************************/

static void print_oper_name(com_keyword op, FILE *out)
{
    switch (op)
    {
    case OP_SFC:
        print_dline_box(0, out,
                        " Star-free closure (𝒞 ↦ SF(𝒞)) : SF(𝒞) is the least class containing 𝒞 and closed under union,");
        print_dline_box(0, out, "    complement and marked concatenation.");
        break;
    case OP_TLC2:
        print_dline_box(0, out,
                        " Combined operator (𝒞 ↦ TL₂(𝒞)) : by definition, TL₂(𝒞) = TL(TL(𝒞)).");
        return;
    case OP_TLC:
        print_dline_box(0, out,
                        " Unary temporal logic (𝒞 ↦ TL(𝒞)) : TL(𝒞) consists of all languages that can be defined by a formula");
        print_dline_box(0, out,
                        "    of unary temporal logic with modalities parameterized by languages in 𝒞.");
        break;
    case OP_FLC2:
        print_dline_box(
            0, out,
            " Combined operator (𝒞 ↦ FL₂(𝒞)) : by definition, FL₂(𝒞) = FL(FL(𝒞)).");
        break;
    case OP_FLC:
        print_dline_box(0, out,
                        " Future unary temporal logic (𝒞 ↦ FL(𝒞)) : FL(𝒞) consists of all languages that can be defined by a");
        print_dline_box(0, out,
                        "     formula of future unary temporal logic with modalities parameterized by languages in 𝒞.");
        break;
    case OP_PLC2:
        print_dline_box(0, out,
                        " Combined operator (𝒞 ↦ PL₂(𝒞)) : by definition, PL₂(𝒞) = PL(PL(𝒞)).");
        break;
    case OP_PLC:
        print_dline_box(0, out,
                        " Past unary temporal logic (𝒞 ↦ PL(𝒞)) : PL(𝒞) consists of all languages that can be defined by a");
        print_dline_box(0, out,
                        "     formula of past unary temporal logic with modalities parameterized by languages in 𝒞.");
        break;
    case OP_POL:
        print_dline_box(0, out,
                        " Polynomial closure (𝒞 ↦ Pol(𝒞)) : Pol(𝒞) consists of finite unions of marked products of languages");
        print_dline_box(0, out, "     in 𝒞.");
        break;
    case OP_POL2:
        print_dline_box(0, out,
                        " Combined operator (𝒞 ↦ Pol₂(𝒞)) : by definition, Pol₂(𝒞) = Pol(BPol(𝒞)).");
        print_dline_box(0, out,
                        " Polynomial closure (𝒞 ↦ Pol(𝒞)) : Pol(𝒞) consists of finite unions of marked products of languages");
        print_dline_box(0, out, "     in 𝒞.");
        break;
    case OP_BPOL:
        print_dline_box(0, out,
                        " Boolean polynomial closure (𝒞 ↦ BPol(𝒞)) : BPol(𝒞) consists of Boolean combinations of marked");
        print_dline_box(0, out, "     products of languages in 𝒞.");
        break;
    case OP_UBPOL:
        print_dline_box(0, out, " Combined operator (𝒞 ↦ UPol(BPol(𝒞)).");
        print_dline_box(0, out,
                        " Unambiguous polynomial closure (𝒞 ↦ UPol(𝒞)) : UPol(𝒞) consists of finite disjoint unions of");
        print_dline_box(0, out, "     unambiguous marked products of languages in 𝒞.");
        print_dline_box(0, out,
                        " Boolean polynomial closure (𝒞 ↦ BPol(𝒞)) : BPol(𝒞) consists of Boolean combinations of marked");
        print_dline_box(0, out, "     products of languages in 𝒞.");
        break;
    case OP_UBPOL2:
        print_dline_box(0, out,
                        " Combined operator (𝒞 ↦ UPol(BPol₂(𝒞)) : by definition, UPol(BPol₂(𝒞)) = UPol(BPol(BPol(𝒞))).");
        print_dline_box(0, out,
                        " Unambiguous polynomial closure (𝒞 ↦ UPol(𝒞)) : UPol(𝒞) consists of finite disjoint unions of");
        print_dline_box(0, out, "     unambiguous marked products of languages in 𝒞.");
        print_dline_box(0, out,
                        " Boolean polynomial closure (𝒞 ↦ BPol(𝒞)) : BPol(𝒞) consists of Boolean combinations of marked");
        print_dline_box(0, out, "     products of languages in 𝒞.");
        break;
    default:
        break;
    }
}

static void print_input_name(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dline_box(0, out,
                        " Applied to the input class ST : trivial class consisting of the languages ∅ and A*.");
        break;
    case CL_DD:
        print_dline_box(0, out,
                        " Applied to the input class DD : class consisting of the languages ∅, {ε}, A⁺ and A*.");
        break;
    case CL_AT:
        print_dline_box(0, out,
                        " Applied to the input class AT : alphabet testable languages, membership of a word in the language");
        print_dline_box(0, out,
                        "    depends only on the set of letters occuring inside this word.");
        break;
    case CL_MOD:
        print_dline_box(0, out,
                        " Applied to the input class MOD : modulo languages, membership of a word in the language depends");
        print_dline_box(0, out, "    only on its length modulo a fixed number.");
        break;
    case CL_MODP:
        print_dline_box(0, out,
                        " Applied to the input class MOD⁺ : well-suited extension of the modulo languages, membership of a");
        print_dline_box(0, out,
                        "    word in the language depends only on its length modulo a fixed number and on whether this word");
        print_dline_box(0, out, "    is empty or not. In this case, SF(MOD⁺) = SF(MOD).");
        break;
    case CL_GR:
        print_dline_box(0, out,
                        " Applied to the input class GR : group languages (recognized by a finite group).");
        break;
    default:
        fprintf(out, "#### This input class is not supported.\n\n");
        break;
    }
}

// Star-free closure
void print_infooper_sf(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
    case CL_DD:
    case CL_AT:
        switch (c)
        {
        case CL_ST:
            print_dtitle_box(10, false, out, 1, "Membership: star-free closure of ST (SF(ST)).");
            break;
        case CL_DD:
            print_dtitle_box(10, false, out, 1, "Membership: star-free closure of DD (SF(DD)).");
            break;
        case CL_AT:
            print_dtitle_box(10, false, out, 1, "Membership: star-free closure of AT (SF(AT)).");
            break;
        default:
            // Should not happen
            return;
        }
        print_oper_name(OP_SFC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Original class of star-free languages.");
        print_dline_box(0, out, "  - First-order logic with the linear order (FO(<)).");
        print_dline_box(0, out, "  - Linear temporal logic (LTL).");
        print_dline_box(0, out, "  - Languages with an aperiodic syntactic monoid.");
        print_dline_box(0, out, "  - Languages with a counter-free minimal automaton.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
    case CL_MODP:
        switch (c)
        {
        case CL_MOD:
            print_dtitle_box(10, false, out, 1, "Membership: star-free closure of MOD (SF(MOD)).");
            break;
        case CL_MODP:
            print_dtitle_box(10, false, out, 1,
                             "Membership: star-free closure of MOD⁺ (SF(MOD⁺)).");
            break;
        default:
            // Should not happen
            return;
        }
        print_oper_name(OP_SFC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - First-order logic with the linear order and modular predicates (FO(<,MOD)).");
        print_dline_box(0, out, "  - Linear temporal logic with modular modalities (LTL(MOD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD-kernel is aperiodic.");
        print_dline_box(0, out,
                        "  - Languages with a minimal automaton whose MOD-kernel is counter-free.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1, "Membership: star-free closure of GR (SF(GR)).");
        print_oper_name(OP_SFC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - First-order logic with the linear order and group predicates (FO(<,GR)).");
        print_dline_box(0, out, "  - Linear temporal logic with group modalities (LTL(GR)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose GR-kernel is aperiodic.");
        print_dline_box(0, out,
                        "  - Languages with a minimal automaton whose GR-kernel is counter-free.");
        print_dbot_line(100, out);
        break;
    default:
        return;
        break;
    }
}

// Temporal closure
void print_infooper_tl(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of ST (TL(ST)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Unambiguous languages (UL).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order (FO²(<)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<) in the alternation hierarchy of first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Unary temporal logic with finally and previously (TL = F + P).");
        print_dline_box(0, out, "  - Languages with a syntactic monoid in DA.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of DD (TL(DD)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in the dot-depth hierarchy (UPol(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order and the successor (FO²(<,+1)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,+1) in the alternation hierarchy of first-order logic with the linear order and the");
        print_dline_box(0, out, "    successor.");
        print_dline_box(0, out,
                        "  - Unary temporal logic with finally, next, previously and yesterday (TLX = F + X + P + Y).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose DD-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of AT (TL(AT)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with alphabetic predicates (FO²(AT)).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis ST (TL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(ST)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of MOD (TL(MOD)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in modulo hierarchy (UPol(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order and the modular predicates (FO²(<,MOD)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,MOD) in the alternation hierarchy of first-order logic with the linear order and the");
        print_dline_box(0, out, "    modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD-kernel is in DA.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of MOD⁺ (TL(MOD⁺)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of BPol(MOD⁺) (UPol(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order, the successor and the modular predicates");
        print_dline_box(0, out, "    (FO²(<,+1,MOD)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,MOD) in the alternation hierarchy of first-order logic with the linear order, the");
        print_dline_box(0, out, "    successor and the modular predicates.");
        print_dline_box(0, out, "  - Extended unary temporal logic closure of MOD (TLX(MOD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD⁺-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: unary temporal logic closure of GR (TL(GR)).");
        print_oper_name(OP_TLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in group hierarchy (UPol(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order and the group predicates (FO²(<,GR)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,GR) in the alternation hierarchy of first-order logic with the linear order and the");
        print_dline_box(0, out, "    group predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose GR-kernel is in DA.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Nested temporal closure
void print_infooper_tl2(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested temporal hierarchy of basis ST (TL₂(ST)).");
        print_oper_name(OP_TLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with alphabetic predicates (FO²(AT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis AT (TL(AT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis BPol(ST) (TL(BPol(ST))).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis ST (TL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(ST)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(ST)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested temporal hierarchy of basis DD (TL₂(DD)).");
        print_oper_name(OP_TLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis LT (TL(LT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis BPol(DD) (TL(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis DD (TL₂(DD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose LT-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(DD)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(DD)-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested temporal hierarchy of basis MOD (TL₂(MOD)).");
        print_oper_name(OP_TLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis BPol(MOD) (TL(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis MOD (TL₂(MOD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(MOD)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(MOD)-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested temporal hierarchy of basis MOD⁺ (TL₂(MOD⁺)).");
        print_oper_name(OP_TLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis BPol(MOD⁺) (TL(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis MOD⁺ (TL₂(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(MOD⁺)-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested temporal hierarchy of basis GR (TL₂(GR)).");
        print_oper_name(OP_TLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested temporal hierarchy of basis BPol(GR) (TL(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Level two in the nested temporal hierarchy of basis GR (TL₂(GR)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(GR)-orbits are in DA.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(GR)-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Future temporal closure
void print_infooper_fl(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of ST (FL(ST)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Right polynomial closure of BPol(ST) (RPol(BPol(ST))).");
        print_dline_box(0, out, "  - Unary temporal logic with finally (FL = F).");
        print_dline_box(0, out, "  - Languages with an L-trivial syntactic monoid.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of DD (FL(DD)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Right polynomial closure of BPol(DD) (RPol(BPol(DD))).");
        print_dline_box(0, out, "  - Unary temporal logic with finally and next (FLX = F + X).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose DD-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of AT (FL(AT)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level two in the future nested temporal hierarchy of basis ST (FL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of MOD (FL(MOD)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Right polynomial closure of BPol(MOD) (RPol(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD-kernel is L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of MOD⁺ (FL(MOD⁺)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Right polynomial closure of BPol(MOD⁺) (RPol(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD⁺-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: future unary temporal logic closure of GR (FL(GR)).");
        print_oper_name(OP_FLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Right polynomial closure of BPol(GR) (RPol(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose GR-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Nested future temporal closure
void print_infooper_fl2(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested future temporal hierarchy of basis ST (FL₂(ST)).");
        print_oper_name(OP_FLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis AT (FL(AT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis BPol(ST) (FL(BPol(ST))).");
        print_dline_box(0, out,
                        "  - Level two in the nested future temporal hierarchy of basis ST (FL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(ST)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(ST)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested future temporal hierarchy of basis DD (FL₂(DD)).");
        print_oper_name(OP_FLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis LT (FL(LT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis BPol(DD) (FL(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested future temporal hierarchy of basis DD (FL₂(DD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose LT-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(DD)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose FL(DD)-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested future temporal hierarchy of basis MOD (FL₂(MOD)).");
        print_oper_name(OP_FLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis BPol(MOD) (FL(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested future temporal hierarchy of basis MOD (FL₂(MOD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(MOD)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose FL(MOD)-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested future temporal "
                         "hierarchy of basis MOD⁺ (FL₂(MOD⁺)).");
        print_oper_name(OP_FLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis BPol(MOD⁺) (FL(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Level two in the nested future temporal hierarchy of basis MOD⁺ (FL₂(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose FL(MOD⁺)-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested future temporal hierarchy of basis GR (FL₂(GR)).");
        print_oper_name(OP_FLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested future temporal hierarchy of basis BPol(GR) (FL(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Level two in the nested future temporal hierarchy of basis GR (FL₂(GR)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(GR)-orbits are L-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose FL(GR)-orbits are L-trivial.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Past temporal closure
void print_infooper_pl(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of ST (PL(ST)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Left polynomial closure of BPol(ST) (LPol(BPol(ST))).");
        print_dline_box(0, out, "  - Unary temporal logic with previously (PL = P).");
        print_dline_box(0, out, "  - Languages with a R-trivial syntactic monoid.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of DD (PL(DD)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Left polynomial closure of BPol(DD) (LPol(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Unary temporal logic with previously and yesterday (PLX = P + Y).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose DD-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of AT (PL(AT)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level two in the past nested temporal hierarchy of basis ST (PL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of MOD (PL(MOD)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Left polynomial closure of BPol(MOD) (LPol(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD-kernel is R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of MOD⁺ (PL(MOD⁺)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Left polynomial closure of BPol(MOD⁺) (LPol(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD⁺-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: past unary temporal logic closure of GR (PL(GR)).");
        print_oper_name(OP_PLC, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Left polynomial closure of BPol(GR) (LPol(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose GR-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    default:
        return;
        break;
    }
}

// Nested past temporal closure
void print_infooper_pl2(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested past temporal hierarchy of basis ST (PL₂(ST)).");
        print_oper_name(OP_PLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of basis AT (PL(AT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of basis BPol(ST) (PL(BPol(ST))).");
        print_dline_box(0, out,
                        "  - Level two in the nested past temporal hierarchy of basis ST (PL₂(ST)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose TL(ST)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(ST)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested past temporal hierarchy of basis DD (PL₂(DD)).");
        print_oper_name(OP_PLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of basis LT (PL(LT)).");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of basis BPol(DD) (PL(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested past temporal hierarchy of basis DD (PL₂(DD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose LT-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose BPol(DD)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose PL(DD)-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested past temporal "
                         "hierarchy of basis MOD (PL₂(MOD)).");
        print_oper_name(OP_PLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of "
                        "basis BPol(MOD) (PL(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Level two in the nested past temporal hierarchy of "
                        "basis MOD (PL₂(MOD)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(MOD)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "PL(MOD)-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested past temporal "
                         "hierarchy of basis MOD⁺ (PL₂(MOD⁺)).");
        print_oper_name(OP_PLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of "
                        "basis BPol(MOD⁺) (PL(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Level two in the nested past temporal hierarchy of "
                        "basis MOD⁺ (PL₂(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(MOD⁺)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "PL(MOD⁺)-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level two in the nested past temporal "
                         "hierarchy of basis GR (PL₂(GR)).");
        print_oper_name(OP_PLC2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the nested past temporal hierarchy of "
                        "basis BPol(GR) (PL(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Level two in the nested past temporal hierarchy of "
                        "basis GR (PL₂(GR)).");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(GR)-orbits are R-trivial.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "PL(GR)-orbits are R-trivial.");
        print_dbot_line(100, out);
        break;
    default:
        return;
        break;
    }
}

// Polynomial closure
void print_infooper_pol(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of ST (Pol(ST)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Class PPT of positive piecewise testable languages.");
        print_dline_box(
            0, out,
            "  - Level 1/2 in the Straubing-Thérien hierarchy (Pol(ST)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic ordered monoid satisfying "
                        "the equation 1 ≤ s for all s.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of DD (Pol(DD)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 1/2 in the dot-depth hierarchy (Pol(DD)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(<,+1) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    successor.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose DD-orbits "
                        "satisfy the equation 1 ≤ s for all s.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of AT (Pol(AT)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(
            0, out,
            "  - Level 1/2 in the concatenation hierarchy of basis AT (Pol(AT)).");
        print_dline_box(
            0, out,
            "  - Level 3/2 in the Straubing-Thérien hierarchy (Pol₂(ST)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(AT) in the alternation hierarchy of "
                        "first-order logic with alphabetic predicates.");
        print_dline_box(0, out,
                        "  - Level Σ₂(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits "
                        "satisfy the equation 1 ≤ s for all s.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(ST)-orbits satisfy the equation 1 ≤ s for all s.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of MOD (Pol(MOD)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 1/2 in the modulo hierarchy (Pol(MOD)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD-kernel "
                        "satisfies the equation 1 ≤ s for all s.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of MOD⁺ (Pol(MOD⁺)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 1/2 in the concatenation hierarchy of basis "
                        "MOD⁺ (Pol(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(<,+1,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order, the");
        print_dline_box(0, out, "    successor and the modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose MOD⁺-orbits "
                        "satisfy the equation 1 ≤ s for all s.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: polynomial closure of GR (Pol(GR)).");
        print_oper_name(OP_POL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 1/2 in the group hierarchy (Pol(GR)).");
        print_dline_box(0, out,
                        "  - Level Σ₁(<,GR) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    group predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic monoid M which satisfies "
                        "the equation 1 ≤ e for all e ∊ E(M).");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Boolean polynomial closure
void print_infooper_bpol(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of ST (BPol(ST)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Class PT of piecewise testable languages.");
        print_dline_box(
            0, out,
            "  - Level one in the Straubing-Thérien hierarchy (BPol(ST)).");
        print_dline_box(0, out,
                        "  - Level BΣ₁(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a J-trivial syntactic monoid.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of DD (BPol(DD)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the dot-depth hierarchy (BPol(DD)).");
        print_dline_box(0, out,
                        "  - Level BΣ₁(<,+1) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the successor.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic semigroup S which "
                        "satisfies Knast's equation: for every q,r,s,t ∊ S");
        print_dline_box(0, out,
                        "    and every e,f ∊ E(S), we have (eqfre)ʷ(esfte)ʷ = "
                        "(eqfre)ʷqft(esfte)ʷ.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of AT (BPol(AT)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(
            0, out,
            "  - Level one in the concatenation hierarchy of basis AT (BPol(AT)).");
        print_dline_box(
            0, out,
            "  - Level two in the Straubing-Thérien hierarchy (BPol₂(ST)).");
        print_dline_box(0, out,
                        "  - Level BΣ₂(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism which satsifies "
                        "the two following equations:");
        print_dline_box(0, out,
                        "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all "
                        "q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
        print_dline_box(0, out,
                        "       {r,e,f}, {s,e,f} and {t,e,f} are AT-sets.");
        print_dline_box(0, out,
                        "     • (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ for all s,t ∊ M "
                        "and e ∊ E(M) such that (e,s) is an AT-pair.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of MOD (BPol(MOD)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the modulo hierarchy (BPol(MOD)).");
        print_dline_box(0, out,
                        "  - Level BΣ₁(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order");
        print_dline_box(0, out, "    and the modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism which satisfies "
                        "the equation (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ");
        print_dline_box(0, out,
                        "    for all q,r,s,t ∊ M such that (q,s) is a MOD-pair.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of MOD⁺ (BPol(MOD⁺)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level one in the concatenation hierarchy of basis "
                        "MOD⁺ (BPol(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Level BΣ₁(<,+1,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order,");
        print_dline_box(0, out, "    the successor and the modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose strict "
                        "MOD-kernel S satisfies Knast's equation:");
        print_dline_box(0, out,
                        "    for all q,r,s,t ∊ S and all e,f ∊ E(S), we have "
                        "(eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(
            10, false, out, 1,
            "Membership: Boolean polynomial closure of GR (BPol(GR)).");
        print_oper_name(OP_BPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(
            0, out,
            "  - Level one in the concatenation hierarchy of basis GR (BPol(GR)).");
        print_dline_box(0, out,
                        "  - Level BΣ₁(<,GR) in the alternation hierarchy of "
                        "first-order logic with the linear order");
        print_dline_box(0, out, "    and the group predicates.");
        print_dline_box(
            0, out,
            "  - Languages with a syntactic monoid which is a block group.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Unambiguous Boolean polynomial closure
void print_infooper_ubpol(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1, "Membership: class UPol(BPol(ST)).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out, "  - Unambiguous languages (UL).");
        print_dline_box(
            0, out,
            "  - Two-variable first-order logic with the linear order (FO²(<)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(
            0, out,
            "  - Unary temporal logic with finally and previously (TL = F + P).");
        print_dline_box(0, out, "  - Languages with a syntactic monoid in DA.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1, "Membership: class UPol(BPol(DD)).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in "
                        "the dot-depth hierarchy (UPol(BPol(DD))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order "
                        "and the successor (FO²(<,+1)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,+1) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    successor.");
        print_dline_box(0, out,
                        "  - Unary temporal logic with finally, next, previously "
                        "and yesterday (TLX = F + X + P + Y).");
        print_dline_box(
            0, out,
            "  - Languages with a syntactic morphism whose DD-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_AT:
        print_dtitle_box(10, false, out, 1, "Membership: class UPol(BPol(AT)).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level two in "
                        "Straubing-Thérien hierarchy");
        print_dline_box(0, out, "    (UPol(BPol₂(ST))).");
        print_dline_box(0, out,
                        "  - Level Δ₂(AT) in the alternation hierarchy of "
                        "first-order logic with alphabetic predicates.");
        print_dline_box(0, out,
                        "  - Level Δ₃(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(
            0, out,
            "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an AT-pair.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol(MOD)).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in "
                        "modulo hierarchy (UPol(BPol(MOD))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order "
                        "and the modular predicates (FO²(<,MOD)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    modular predicates.");
        print_dline_box(0, out,
                        "  - Unary temporal logic closure of MOD (TL(MOD)).");
        print_dline_box(
            0, out,
            "  - Languages with a syntactic morphism whose MOD-kernel is in DA.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol(MOD⁺).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(
            0, out,
            "  - Unambiguous polynomial closure of BPol(MOD⁺) (UPol(BPol(MOD⁺))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order, "
                        "the successor and the modular predicates");
        print_dline_box(0, out, "    (FO²(<,+1,MOD)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order, the");
        print_dline_box(0, out, "    successor and the modular predicates.");
        print_dline_box(
            0, out,
            "  - Extended unary temporal logic closure of MOD (TLX(MOD)).");
        print_dline_box(
            0, out,
            "  - Languages with a syntactic morphism whose MOD⁺-orbits are in DA.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1, "Membership: class UPol(BPol(GR)).");
        print_oper_name(OP_UBPOL, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Unambiguous polynomial closure of the level one in "
                        "group hierarchy (UPol(BPol(GR))).");
        print_dline_box(0, out,
                        "  - Two-variable first-order logic with the linear order "
                        "and the group predicates (FO²(<,GR)).");
        print_dline_box(0, out,
                        "  - Level Δ₂(<,GR) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    group predicates.");
        print_dline_box(0, out,
                        "  - Unary temporal logic closure of GR (TL(GR)).");
        print_dline_box(
            0, out,
            "  - Languages with a syntactic morphism whose GR-kernel is in DA.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Nested polynomail closure
void print_infooper_pol2(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level 3/2 in the concatenation hierarchy of "
                         "basis ST (Pol₂(ST)).");
        print_oper_name(OP_POL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(
            0, out,
            "  - Level 3/2 in the Straubing-Thérien hierarchy (Pol₂(ST)).");
        print_dline_box(
            0, out,
            "  - Level 1/2 in the concatenation hierarchy of basis AT (Pol(AT)).");
        print_dline_box(0, out,
                        "  - Level Σ₂(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Level Σ₁(AT) in the alternation hierarchy of "
                        "first-order logic with alphabetic predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(ST)-orbits satisfy the equation 1 ≤ s.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose AT-orbits "
                        "satisfy the equation 1 ≤ s.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level 3/2 in the concatenation hierarchy of "
                         "basis DD (Pol₂(DD)).");
        print_oper_name(OP_POL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 3/2 in the dot-depth hierarchy (Pol₂(DD)).");
        print_dline_box(0, out,
                        "  - Level Σ₂(<,+1) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the successor.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(DD)-orbits satisfy the equation 1 ≤ s.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level 3/2 in the concatenation hierarchy of "
                         "basis MOD (Pol₂(MOD)).");
        print_oper_name(OP_POL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 3/2 in the modulo hierarchy (Pol₂(MOD)).");
        print_dline_box(0, out,
                        "  - Level Σ₂(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the modulo hierarchy.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(MOD)-orbits satisfy the equation 1 ≤ s.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level 3/2 in the concatenation hierarchy of "
                         "basis MOD⁺ (Pol₂(MOD⁺)).");
        print_oper_name(OP_POL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 3/2 in the concatenation hierarchy of basis "
                        "MOD⁺ (Pol₂(MOD⁺)).");
        print_dline_box(0, out,
                        "  - Level Σ₂(<,+1,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order, the");
        print_dline_box(0, out, "    successor and the modulo hierarchy.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(MOD⁺)-orbits satisfy the equation 1 ≤ s.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: level 3/2 in the concatenation hierarchy of "
                         "basis GR (Pol₂(GR)).");
        print_oper_name(OP_POL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level 3/2 in the group hierarchy (Pol₂(GR)).");
        print_dline_box(0, out,
                        "  - Level Σ₂(<,GR) in the alternation hierarchy of "
                        "first-order logic with the linear order and the");
        print_dline_box(0, out, "    group predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism whose "
                        "BPol(GR)-orbits satisfy the equation 1 ≤ s.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Nested unambiguous Boolean polynomial closure
void print_infooper_ubpol2(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol₂(ST)).");
        print_oper_name(OP_UBPOL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level Δ₃(<) in the alternation hierarchy of "
                        "first-order logic with the linear order.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(0, out,
                        "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an "
                        "BPol(ST)-pair.");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol₂(DD)).");
        print_oper_name(OP_UBPOL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level Δ₃(<,+1) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the successor.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(0, out,
                        "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an "
                        "BPol(DD)-pair.");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol₂(MOD)).");
        print_oper_name(OP_UBPOL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level Δ₃(<,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(0, out,
                        "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an "
                        "BPol(MOD)-pair.");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol₂(MOD⁺)).");
        print_oper_name(OP_UBPOL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level Δ₃(<,+1,MOD) in the alternation hierarchy of "
                        "first-order logic with the linear order, the");
        print_dline_box(0, out, "    successor and the modular predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(0, out,
                        "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an "
                        "BPol(MOD⁺)-pair.");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Membership: class UPol(BPol₂(GR)).");
        print_oper_name(OP_UBPOL2, out);
        print_dmid_line(100, out);
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " Characterizations :");
        print_dline_box(0, out,
                        "  - Level Δ₃(<,GR) in the alternation hierarchy of "
                        "first-order logic with the linear order and");
        print_dline_box(0, out, "    the group predicates.");
        print_dline_box(0, out,
                        "  - Languages with a syntactic morphism satisfying the "
                        "equation (esete)ʷ⁺¹ = (esete)ʷete(esete)ʷ");
        print_dline_box(0, out,
                        "    for all s,t ∊ M and e ∊ E(M) such that (e,s) is an "
                        "BPol(GR)-pair.");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Negation hierarchies
void print_infooper_neghiera(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(
            10, false, out, 1,
            "Negation hierarchy of unary temporal logic (TL = TL(ST)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out,
                        " For each natural number n ≥ 1, the level n corresponds "
                        "to the following classes:");
        print_dline_box(0, out,
                        "  - Level TLₙ in the negation hierarchy of TL.");
        print_dline_box(0, out,
                        "  - Level BΣ²ₙ(<) in the alternation hierarchy of "
                        "two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order.");
        print_dline_box(0, out,
                        "  - Level MPolₖ(BPol(ST)) for k = n - 1 in the "
                        "deterministic hierarchy of basis BPol(ST).");
        print_dbot_line(100, out);
        break;
    case CL_DD:
        print_dtitle_box(10, false, out, 1,
                         "Negation hierarchy of the unary temporal logic closure "
                         "of DD (TL(DD)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out,
                        " For each natural number n ≥ 1, the level n corresponds "
                        "to the following classes:");
        print_dline_box(0, out,
                        "  - Level TLₙ(DD) in the negation hierarchy of TL(DD).");
        print_dline_box(0, out,
                        "  - Level BΣ²ₙ(<,+1) in the alternation hierarchy of "
                        "two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order and the successor.");
        print_dline_box(0, out,
                        "  - Level MPolₖ(BPol(DD)) for k = n - 1 in the "
                        "deterministic hierarchy of basis BPol(DD).");
        print_dbot_line(100, out);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, out, 1,
                         "Negation hierarchy of the unary temporal logic closure "
                         "of MOD (TL(MOD)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out,
                        " For each natural number n ≥ 1, the level n corresponds "
                        "to the following classes:");
        print_dline_box(0, out,
                        "  - Level TLₙ(MOD) in the negation hierarchy of TL(MOD).");
        print_dline_box(0, out,
                        "  - Level BΣ²ₙ(<,MOD) in the alternation hierarchy of "
                        "two-variable first-order logic with");
        print_dline_box(0, out,
                        "    the linear order and the modular predicates.");
        print_dline_box(0, out,
                        "  - Level MPolₖ(BPol(MOD)) for k = n - 1 in the "
                        "deterministic hierarchy of basis BPol(MOD).");
        print_dbot_line(100, out);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, out, 1,
                         "Negation hierarchy of the unary temporal logic closure "
                         "of MOD (TL(MOD⁺)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out,
                        " For each natural number n ≥ 1, the level n corresponds "
                        "to the following classes:");
        print_dline_box(
            0, out,
            "  - Level TLₙ(MOD⁺) in the negation hierarchy of TL(MOD⁺).");
        print_dline_box(0, out,
                        "  - Level BΣ²ₙ(<,+1,MOD) in the alternation hierarchy of "
                        "two-variable first-order logic with");
        print_dline_box(
            0, out,
            "    the linear order, the successor and the modular predicates.");
        print_dline_box(0, out,
                        "  - Level MPolₖ(BPol(MOD⁺)) for k = n - 1 in the "
                        "deterministic hierarchy of basis BPol(MOD⁺).");
        print_dbot_line(100, out);
        break;
    case CL_GR:
        print_dtitle_box(10, false, out, 1,
                         "Negation hierarchy of the unary temporal logic closure "
                         "of GR (TL(GR)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out,
                        " For each natural number n ≥ 1, the level n corresponds "
                        "to the following classes:");
        print_dline_box(0, out,
                        "  - Level TLₙ(GR) in the negation hierarchy of TL(MOD).");
        print_dline_box(0, out,
                        "  - Level BΣ²ₙ(<,GR) in the alternation hierarchy of "
                        "two-variable first-order logic with");
        print_dline_box(0, out,
                        "    the linear order and the group predicates.");
        print_dline_box(0, out,
                        "  - Level MPolₖ(BPol(GR)) for k = n - 1 in the "
                        "deterministic hierarchy of basis BPol(GR).");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Future/past hierarchies
void print_infooper_fphiera(com_keyword c, FILE *out)
{
    switch (c)
    {
    case CL_ST:
        print_dtitle_box(
            10, false, stdout, 1,
            "Future/Past hierarchy of unary temporal logic (TL = TL(ST)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
                        " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level RPolₙ(BPol(ST)) in the "
                        "deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout,
                        "  - If n is even, level LPolₙ(BPol(ST)) in the "
                        "deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout,
                        " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level LPolₙ(BPol(ST)) in the "
                        "deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout,
                        "  - If n is even, level RPolₙ(BPol(ST)) in the "
                        "deterministic hierarchy of basis BPol(ST).");
        print_dbot_line(100, stdout);
        break;
    case CL_DD:
        print_dtitle_box(10, false, stdout, 1,
                         "Future/Past hierarchy of the unary temporal logic "
                         "closure of DD (TL(DD)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
                        " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level RPolₙ(BPol(DD)) in the "
                        "deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout,
                        "  - If n is even, level LPolₙ(BPol(DD)) in the "
                        "deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout,
                        " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level LPolₙ(BPol(DD)) in the "
                        "deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout,
                        "  - If n is even, level RPolₙ(BPol(DD)) in the "
                        "deterministic hierarchy of basis BPol(DD).");
        print_dbot_line(100, stdout);
        break;
    case CL_MOD:
        print_dtitle_box(10, false, stdout, 1,
                         "Future/Past hierarchy of the unary temporal logic "
                         "closure of MOD (TL(MOD)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
                        " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level RPolₙ(BPol(MOD)) in the "
                        "deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout,
                        "  - If n is even, level LPolₙ(BPol(MOD)) in the "
                        "deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout,
                        " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level LPolₙ(BPol(MOD)) in the "
                        "deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout,
                        "  - If n is even, level RPolₙ(BPol(MOD)) in the "
                        "deterministic hierarchy of basis BPol(MOD).");
        print_dbot_line(100, stdout);
        break;
    case CL_MODP:
        print_dtitle_box(10, false, stdout, 1,
                         "Future/Past hierarchy of the unary temporal logic "
                         "closure of MOD⁺ (TL(MOD⁺)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
                        " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level RPolₙ(BPol(MOD⁺)) in the "
                        "deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout,
                        "  - If n is even, level LPolₙ(BPol(MOD⁺)) in the "
                        "deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout,
                        " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level LPolₙ(BPol(MOD⁺)) in the "
                        "deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout,
                        "  - If n is even, level RPolₙ(BPol(MOD⁺)) in the "
                        "deterministic hierarchy of basis BPol(MOD⁺).");
        print_dbot_line(100, stdout);
        break;
    case CL_GR:
        print_dtitle_box(10, false, stdout, 1,
                         "Future/Past hierarchy of the unary temporal logic "
                         "closure of GR (TL(GR)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
                        " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level RPolₙ(BPol(GR)) in the "
                        "deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout,
                        "  - If n is even, level LPolₙ(BPol(GR)) in the "
                        "deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout,
                        " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout,
                        "  - If n is odd, level LPolₙ(BPol(GR)) in the "
                        "deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout,
                        "  - If n is even, level RPolₙ(BPol(GR)) in the "
                        "deterministic hierarchy of basis BPol(GR).");
        print_dbot_line(100, stdout);
        break;
    default:
        break;
    }
}
