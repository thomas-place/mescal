#include "nfa_patterns.h"
#include "printing.h"

void view_pattern_positive(FILE* out) {
    fprintf(out, "#### The pattern equation is satisfied.\n");
}

void view_pattern_example(dfa* A, uint nb_states, uint* states, uint nb_words, char* vars, uint** words, bool print_num, FILE* out) {

    fprintf(out, "#### The pattern equation fails for ");
    if (print_num) {
        fprintf(out, "n = %d, ", nb_states - 1);
    }

    for (uint i = 0; i < nb_states; i++) {
        if (nb_words == 0 && i == nb_states - 1) {
            fprintf(out, " and ");
        }
        else if (i > 0) {
            fprintf(out, ", ");
        }
        fprintf(out, "x");
        fprint_subsc_utf8(i, out);
        fprintf(out, " = ");
        dfa_print_state(A, states[i], out);
    }

    for (uint i = 0; i < nb_words; i++) {
        if (i == nb_words - 1) {
            fprintf(out, " and ");
        }
        else {
            fprintf(out, ", ");
        }
        fprintf(out, "%c = ", vars[i]);
        if (words[i]) {
            uint j = 0;
            if (words[i][j] == UINT_MAX) {
                fprintf(out, "ε");
            }
            else {
                while (words[i][j] != UINT_MAX) {
                    fprint_letter_utf8(A->alphabet[words[i][j]], out);
                    j++;
                }
            }
        }
        else {
            fprintf(out, "NULL");
        }
    }
    fprintf(out, ".\n");

}

void view_pattern_intro_default(FILE* out, const char* name) {
    fprintf(out, "#### Checking if the %s satisfies the following pattern equation:\n", name);
}
void view_pattern_intro_strict(FILE* out, const char* name) {
    fprintf(out, "#### Checking if the %s satisfies the following pattern equation for strict matchings:\n", name);
}

bool make_pattern_conclusion_default(bool res, FILE* out) {
    if (out) {
        if (res) {
            fprintf(out, "#### The pattern equation is satisfied.\n");

        }
        else {
            fprintf(out, "#### The pattern equation is not satisfied.\n");
        }
    }
    return res;
}


void view_pattern_dd(void) {
    view_image(" ./patterns/dd.png");
}

void view_pattern_commutative(void) {
    view_image(" ./patterns/commutative.png");
}

void view_pattern_idem(void) {
    view_image(" ./patterns/idem.png");
}

void view_pattern_ddidem(void) {
    view_image(" ./patterns/orbidem.png");
}

void view_pattern_ddcomm(void) {
    view_image(" ./patterns/orbcommutative.png");
}

void view_pattern_lttcomm(void) {
    view_image(" ./patterns/orbgencomm.png");
}

void view_pattern_group(void) {
    view_image(" ./patterns/grp.png");
}
void view_pattern_grplus(void) {
    view_image(" ./patterns/grplus.png");
}

void view_pattern_rtrivialgp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/rtriv.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/rtrivmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/rtrivamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/rtrivgr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_rtrivialgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/rtrivplus.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/rtrivmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/rtrivamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/rtrivgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_rtrivialbpgp(basis mode) {
    switch (mode)
    {
    case BA_ST:
        view_image(" ./patterns/rtrivbpst.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/rtrivbpmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/rtrivbpamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/rtrivbpgr.png");
        break;

    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
        break;
    }
}


void view_pattern_rtrivialbpgpplus(basis mode) {
    switch (mode)
    {
    case BA_ST:
        view_image(" ./patterns/rtrivbpdd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/rtrivbpmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/rtrivbpamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/rtrivbpgrplus.png");
        break;

    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
        break;
    }
}





void view_pattern_ltrivialgp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/ltrivial.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/ltrivialmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/ltrivialamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/ltrivialgr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_ltrivialgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/ltrivialdd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/ltrivialmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/ltrivialamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/ltrivialgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_ltrivialbpgp(basis mode) {
    switch (mode)
    {
    case BA_ST:
        view_image(" ./patterns/ltrivialbpst.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/ltrivialbpmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/ltrivialbpamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/ltrivialbpgr.png");
        break;

    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
        break;
    }
}


void view_pattern_ltrivialbpgpplus(basis mode) {
    switch (mode)
    {
    case BA_ST:
        view_image(" ./patterns/ltrivialbpdd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/ltrivialbpmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/ltrivialbpamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/ltrivialbpgrplus.png");
        break;

    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
        break;
    }
}



void view_pattern_dagp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/da.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/damod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/daamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/dagr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}
void view_pattern_dagpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/dadd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/damodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/daamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/dagrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_dabpgp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/dabpst.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/dabpmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/dabpamt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/dabpgr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}
void view_pattern_dabpgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/dabpdd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/dabpmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/dabpamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/dabpgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}


void view_pattern_cfreegp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/counter.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/countermod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/counteramt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/countergrp.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}




void view_pattern_polgp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/ppt.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/polmod.png");
        break;
    case BA_GR:
        view_image(" ./patterns/polgr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}


void view_pattern_polgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/pol2dd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/polmodp.png");
        break;
    case BA_GR:
        view_image(" ./patterns/polgrp.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_pol2gp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/pol2st.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/pol2mod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/pol2amt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/pol2gr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_pol2gpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/pol2dd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/pol2modp.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/pol2amtp.png");
        break;
    case BA_GR:
        view_image(" ./patterns/pol2grp.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}


void view_pattern_bpolgp(basis mode) {
    switch (mode) {
    case BA_MOD:
        view_image(" ./patterns/bpolmod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/bpolamt.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_bpolgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/bpoldd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/bpolmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/bpolamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/bpolgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_knastgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/bpoldd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/knastmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/knastamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/knastgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}

void view_pattern_knastat(void) {
    view_image(" ./patterns/bpol2st.png");
}

void view_pattern_upolgpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/upoldd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/upolmodplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/upolamtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/upolgrplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}



void view_pattern_upolbp2gp(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/upb2st.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/upb2mod.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/upb2amt.png");
        break;
    case BA_GR:
        view_image(" ./patterns/upb2gr.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}
void view_pattern_upolbp2gpplus(basis mode) {
    switch (mode) {
    case BA_ST:
        view_image(" ./patterns/upb2dd.png");
        break;
    case BA_MOD:
        view_image(" ./patterns/upb2modplus.png");
        break;
    case BA_AMT:
        view_image(" ./patterns/upb2amtplus.png");
        break;
    case BA_GR:
        view_image(" ./patterns/upb2grplus.png");
        break;
    default:
        fprintf(stderr, "Error: Invalid group prevariety mode.\n");
        exit(EXIT_FAILURE);
    }
}
