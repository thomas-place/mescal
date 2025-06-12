/*****************************/
/* Gestionnaire de commandes */
/*****************************/

#include "shell_commands.h"
#include "monoid_display.h"
#include "shell_autoprops.h"
#include "shell_memb.h"
#include "shell_morprops.h"
#include "shell_filtering.h"
#include "files.h"

sub_level optimization_level = LV_REG;

int com_get_object(int i, string_chain* thechain) {
    while (thechain != NULL && i != -1) {
        switch (string_to_keyword(thechain->string)) {
        case KY_MINI:
            i = shell_compute_minimal(i);
            break;
        case KY_SYNT:
            i = shell_compute_syntac(i, false);
            break;
        default:
            return -1;
            break;
        }
        thechain = thechain->next;
    }
    return i;
}

void com_setrec_command(char* varname, char* subname, char* par, com_command* thecom) {

    int i = object_get_from_name(varname);
    if (i == -1) {
        shell_error_unknownvar(varname);
        return;
    }
    if (objects[i]->type != RECDEF) {
        shell_error_notrecvar(varname);
        return;
    }

    uchar h = shell_rec_getnum(objects[i]->rec, subname);
    if (h >= objects[i]->rec->num) {
        shell_error_unknownrel(varname, subname);
        return;
    }
    if (strlen(par) == 1 && par[0] == 'i') {
        if (thecom->thetype != CMT_RAW) {
            shell_error_notrecdef(thecom);
            return;
        }
        symbolic_count = objects[i]->rec->num;
        symbolic_names = objects[i]->rec->names;
        regexp* myexp = parse_string_regexp(thecom->main->string);
        symbolic_count = 0;
        symbolic_names = NULL;
        if (myexp == NULL) {
            shell_error_notrecdef(thecom);
            return;
        }
        shell_rec_defadd(i, h, myexp);
        return;
    }

    char* end;
    ushort nb = strtol(par, &end, 10);
    if (*end != '\0') {
        shell_error_syntax();
        return;
    }
    if (nb >= objects[i]->rec->init) {
        shell_error_wrongrecindex(nb, varname);
        return;
    }

    bool save;
    int j = com_apply_command(thecom, NULL, MODE_DEFAULT, &save);
    if (j == -1 || objects[j]->type != REGEXP) {
        shell_error_notregexp(thecom);
        if (save && j != -1) {
            object_free(j);
        }
        return;
    }
    shell_rec_iniadd(i, h, nb, reg_copy(objects[j]->exp));

    if (save) {
        object_free(j);
    }
}

int com_apply_command(com_command* thecom, char* varname, com_mode mode, bool* saved) {

    if (thecom == NULL || thecom->main == NULL) {
        shell_error_syntax();
        return -1;
    }
    if (varname && !check_varname(varname)) {
        shell_error_disallowedvar(varname);
        return -1;
    }

    // Initialize the saved variable if it is not NULL (depends on the chosen mode).
    if (saved) {
        switch (mode) {
        case MODE_PRINT:
            *saved = false;
            break;
        case MODE_DEFAULT:
        case MODE_COPY:
            *saved = true;
            break;
        default:
            break;
        }
    }

    // We check if the command starts with a keyword.
    com_keyword key = string_to_keyword(thecom->main->string);

    // If the command does not start with a keyword.
    if (key == KY_NULL) {

        // In that case there should be no parameters.
        if (thecom->params) {
            shell_error_syntax();
            return -1;
        }

        // First case: the command is raw text, we look for a regexp.
        if (thecom->thetype == CMT_RAW) {
            regexp* myexp = parse_string_regexp(thecom->main->string);
            if (myexp == NULL) {
                shell_error_notregexp(thecom);
                return -1;
            }
            switch (mode) {
            case MODE_PRINT:
                reg_print(myexp);
                reg_free(myexp);
                return -1;
                break;
            case MODE_DEFAULT:
            case MODE_COPY:
                return object_add_regexp(varname, myexp);
                break;
            default:
                break;
            }
            return -1;
        }

        // Second case: the command is a variable name.
        if (!check_varname(thecom->main->string)) {
            shell_error_syntax();
            return -1;
        }

        // We try to get the index of the object corresponding to the variable name.
        int i = object_get_from_name(thecom->main->string);
        if (i == -1) {
            shell_error_unknownvar(thecom->main->string);
            return -1;
        }

        // There are two subcases, depending on the type of variable.

        // Subcase 1: regular expression, automaton or morphism.
        if (objects[i]->type != RECDEF) {
            int j = com_get_object(i, thecom->main->next);
            if (j < 0) {
                shell_error_dispatch(j);
                return j;
            }
            switch (mode) {
            case MODE_DEFAULT:
                if (saved) {
                    *saved = false;
                }
                return j;
                break;
            case MODE_PRINT:
                shell_view_object(objects[j], true);
                return -1;
                break;
            case MODE_COPY:
                return shell_copy_generic(j, varname);
                break;
            default:
                break;
            }
        }
        else {
            // Otherwise, we are dealing with a recursive definition (recdef).

            // If the command consists of a single link, display the summary of the recdef.
            if (com_single(thecom)) {
                if (mode == MODE_PRINT) {
                    shell_view_object(objects[i], true);
                    return -1;
                }
                else {
                    shell_error_copyrec();
                    return -1;
                }
            }

            if (thecom->thetype != CMT_IND || !thecom->main->next || thecom->main->next->next) {
                shell_error_syntax();
                return -1;
            }

            uchar h = shell_rec_getnum(objects[i]->rec, thecom->main->next->string);

            if (h >= objects[i]->rec->num) {
                shell_error_unknownrel(thecom->main->string, thecom->main->next->string);
                return -1;
            }

            if (!objects[i]->rec->full) {
                shell_error_recnotok(thecom->main->string);
                return -1;
            }

            int j = object_add_regexp(varname, shell_rec_getexp(i, thecom->main->next->string, thecom->ind));
            if (j == -1) {
                return -1;
            }

            switch (mode) {

            case MODE_DEFAULT:
                *saved = true;
                return j;
                break;
            case MODE_PRINT:
                shell_view_object(objects[j], true);
                object_free(j);
                return -1;
                break;
            case MODE_COPY:
                return j;
                break;
            default:
                break;
            }
            return j;
        }

        shell_error_syntax();
        return -1;
    }

    // If the command starts with a keyword, we check if it is the only one.
    if (thecom->main->next || key == KY_NULL) {
        shell_error_syntax();
        return -1;
    }

    // Commands that do not calculate objects (specific to print mode).
    if (mode == MODE_PRINT) {

        // We check if the command is a known class name.
        classes cl = command_to_class(thecom);
        if (cl != CL_END) {
            fprintf(stdout, "#### ");
            print_command(thecom, stdout);
            fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
            print_class_info(cl, stdout);
            return -1;
        }

        // Other commands that do not calculate objects.
        switch (key) {
        case KY_LATEX:
            return shell_latex_gen(thecom->params, thecom->main->string);
            break;
        case KY_SAVE:
            return shell_save_to_file(thecom->params, thecom->main->string);
            break;
        case KY_SAVESESSION:
            return shell_save_session(thecom->params, thecom->main->string);
            break;
        case KY_LOADSESSION:
            return shell_load_session(thecom->params, thecom->main->string);
            break;
        case KY_DELETE:
            return shell_delete(thecom->params, thecom->main->string);
            break;
        case KY_SEPAR:
            return shell_separation(thecom->params, thecom->main->string);
            break;
        case KY_RESET:
            return shell_reset(thecom->params, thecom->main->string);
            break;
        case KY_RCAY:
            return shell_view_rcayley(thecom->params, thecom->main->string);
            break;
        case KY_LCAY:
            return shell_view_lcayley(thecom->params, thecom->main->string);
            break;
        case KY_MULT:
            return shell_view_mormult(thecom->params, thecom->main->string);
            break;
        case KY_IDEMS:
            return shell_view_idems(thecom->params, thecom->main->string);
            break;
        case KY_ORDER:
            return shell_view_morder(thecom->params, thecom->main->string);
            break;
        case KY_RUN:
            return shell_view_nfa_run(thecom->params, thecom->main->string);
            break;
        case KY_IMAGE:
            return shell_view_mor_image(thecom->params, thecom->main->string);
            break;
        case KY_GKER:
            return shell_view_gkernel(thecom->params, thecom->main->string);
            break;
        case KY_MKER:
            return shell_view_mkernel(thecom->params, thecom->main->string);
            break;
        case KY_AKER:
            return shell_view_akernel(thecom->params, thecom->main->string);
            break;
        case KY_ORB:
            return shell_view_orbits(thecom->params, thecom->main->string);
            break;
        case KY_MEMB:
            return shell_membership(thecom->params, thecom->main->string);
            break;
        case KY_CHIERA:
            return shell_print_chiera(thecom->params, thecom->main->string);
            break;
        case KY_NHIERA:
            return shell_print_neghiera(thecom->params, thecom->main->string);
            break;
        case KY_FPHIERA:
            return shell_print_fphiera(thecom->params, thecom->main->string);
            break;
        case KY_COMMUT:
            return shell_commutative(thecom->params, thecom->main->string);
            break;
        case KY_COUNTER:
            return shell_counterfree(thecom->params, thecom->main->string);
            break;
        case KY_AUTODA:
            return shell_autoda(thecom->params, thecom->main->string);
            break;
        case KY_CYCLETRIV:
            return shell_cycletrivial(thecom->params, thecom->main->string);
            break;
        case KY_PERMUT:
            return shell_permutation(thecom->params, thecom->main->string);
            break;
        case KY_LETTERIND:
            return shell_letteruniform(thecom->params, thecom->main->string);
            break;
            // case KY_EXSEARCH:
            //     return shell_exsearch(thecom->params);
            //     break;
        case KY_EXINIT:
            return shell_initfile_exall(thecom->params, thecom->main->string);
            break;
        case KY_EXINITFP:
            return shell_initfile_exfp(thecom->params, thecom->main->string);
            break;
        case KY_EXINITNEG:
            return shell_initfile_exdet(thecom->params, thecom->main->string);
            break;
        case KY_EXCONTINUE:
            return shell_continuefile(thecom->params, thecom->main->string);
            break;
        case KY_EXRETRIEVE:
            return shell_retrievefile(thecom->params, thecom->main->string);
            break;
        case KY_EXALL:
            return shell_browse_dfas(thecom->params, thecom->main->string);
            break;
        case KY_NEXALL:
            return shell_browse_dfas_neg(thecom->params, thecom->main->string);
            break;
        case KY_FPEXALL:
            return shell_browse_dfas_fp(thecom->params, thecom->main->string);
            break;
        case KY_SORT:
            return shell_sort(thecom->params);
            break;
        case KY_LIST:
            return shell_filter_objects(thecom->params, DUMMY);
            break;
        case KY_CLEAR:
            return shell_delete_all(thecom->params, thecom->main->string);
            break;
        case KY_TOGGLE:
            return shell_toggle_optimization(thecom->params, thecom->main->string);
            break;
        case KY_AUTOMATA:
            return shell_filter_objects(thecom->params, AUTOMATON);
            break;
        case KY_MORPHISMS:
            return shell_filter_objects(thecom->params, MORPHISM);
            break;
        case KY_REGEXPS:
            return shell_filter_objects(thecom->params, REGEXP);
            break;
        case KY_RECDEFS:
            return shell_filter_objects(thecom->params, RECDEF);
            break;
        default:
            break;
        }
    }

    // Commands that calculate objects.
    int k = -1;
    switch (key) {
    case KY_BMCK:
        k = shell_mccluskey_reg(varname, thecom->params, thecom->main->string);
        break;
    case KY_THOMPSON:
        k = shell_thompson_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_INVTRANS:
        k = shell_invtrans(varname, thecom->params, thecom->main->string);
        break;
    case KY_DYCKTRANS:
        k = shell_dycktrans_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_GLUSHKOV:
        k = shell_glushkov_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_HOPCROFT:
        k = shell_hopcroft_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_MIRROR:
        k = shell_mirror_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_TRIMNFA:
        k = shell_trim_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_KLEENE:
        k = shell_kleene_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_ELIMEPS:
        k = shell_elimeps_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_INTERSEC:
        k = shell_intersect_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_UNION:
        k = shell_union_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_DETERMINIZE:
        k = shell_determinize_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_COMPLEMENT:
        k = shell_complement_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_BRZOZO:
        k = shell_brzozowski_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_OPEN:
        k = shell_open_object(varname, thecom->params, thecom->main->string);
        break;
    case KY_RNFA:
        k = shell_random_nfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_RDFA:
        k = shell_random_dfa(varname, thecom->params, thecom->main->string);
        break;
    case KY_RECINIT:
        k = shell_recursive_init(varname, thecom->params, thecom->main->string);
        break;

    case KY_SAVE:
    case KY_SAVESESSION:
    case KY_LOADSESSION:
    case KY_DELETE:
    case KY_SEPAR:
    case KY_RESET:
    case KY_SORT:
    case KY_RCAY:
    case KY_LCAY:
    case KY_MULT:
    case KY_IDEMS:
    case KY_ORDER:
    case KY_RUN:
    case KY_IMAGE:
    case KY_GKER:
    case KY_MKER:
    case KY_ORB:
    case KY_MEMB:
    case KY_CHIERA:
    case KY_NHIERA:
    case KY_FPHIERA:
        shell_error_noreturn(thecom);
        return -1;
        break;
    default:
        shell_error_syntax();
        return -1;
        break;
    }

    if (k >= 0 && mode == MODE_PRINT) {
        shell_view_object(objects[k], true);
        object_free(k);
        return -1;
    }
    return k;
}

/******************************/
/*+ Parameter interpretation +*/
/******************************/

bool param_getclass(com_parameters* pars, int p, const char* str, classes* class) {
    classes cl = command_to_class(com_getparam(pars, p));
    if (cl == CL_END) {
        fprintf(stderr, "#### Error  : Wrong parameter %d in the command \"%s\".", p + 1, str);
        fprintf(stderr, "              The class");
        print_command(com_getparam(pars, p), stdout);
        fprintf(stdout, " is either unknown or unsupported.\n");
        return false;
    }
    if (class) {
        *class = cl;

    }
    return true;
}

bool param_getrawtext(com_parameters* pars, int p, const char* str, char** rawtext) {
    com_command* arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not raw text.\n", p + 1, str);
        return false;
    }
    if (rawtext) {
        *rawtext = arg->main->string;
    }
    return true;
}

bool param_getinteger(com_parameters* pars, int p, const char* str, int* res) {
    com_command* arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an integer.\n", p + 1, str);
        return false;
    }
    char* end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an integer.\n", p + 1, str);
        return false;
    }
    return true;
}

bool param_getlong(com_parameters* pars, int p, const char* str, long* res) {
    com_command* arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an integer.\n", p + 1, str);
        return false;
    }
    char* end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an integer.\n", p + 1, str);
        return false;
    }
    return true;
}

bool param_checkobjtype(int i, par_type type, int p, const char* str) {

    switch (type) {
    case PAR_AUTOMATONV:
        if (i < 0 || objects[i]->type != AUTOMATON) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an automaton variable.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_AUTOMATON:
        if (i < 0 || objects[i]->type != AUTOMATON) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an automaton.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_REGEXP:
        if (i < 0 || objects[i]->type != REGEXP) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a regular expression.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_MORPHISM:
        if (i < 0 || objects[i]->type != MORPHISM) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a morphism.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_RECDEF:
        if (i < 0 || objects[i]->type != RECDEF) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a recursive definition.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_AUTOMOR:
        if (i < 0 || (objects[i]->type != AUTOMATON && objects[i]->type != MORPHISM)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an automaton or a morphism.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_AUTOREG:
        if (i < 0 || (objects[i]->type != AUTOMATON && objects[i]->type != REGEXP)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an automaton or a regular expression.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_REGMOR:
        if (i < 0 || (objects[i]->type != REGEXP && objects[i]->type != MORPHISM)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a regular expression or a morphism.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_OBJECTV:
        if (i < 0 || (objects[i]->type != AUTOMATON && objects[i]->type != REGEXP && objects[i]->type != MORPHISM && objects[i]->type != RECDEF)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an object variable.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_OBJECT:
        if (i < 0 || (objects[i]->type != AUTOMATON && objects[i]->type != REGEXP && objects[i]->type != MORPHISM && objects[i]->type != RECDEF)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not an object.\n", p + 1, str);
            return false;
        }
        break;
    case PAR_REGAUTOMOR:
        if (i < 0 || (objects[i]->type != REGEXP && objects[i]->type != AUTOMATON && objects[i]->type != MORPHISM)) {
            fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a regular expression or an automaton or a morphism.\n", p + 1, str);
            return false;
        }
        break;
    default:
        return false;
        break;
    }
    return true;
}

int param_retrieve(com_parameters* pars, int n, int f, par_type* types, classes* theclasses, char** rawtext, int* integers, int* objs, bool* saved, const char* str) {
    int k = com_nbparams(pars);
    if (n == 0 && k > 0) {
        fprintf(stderr, "#### Error  : the command \"%s\" takes no parameters.\n", str);
        return -2;
    }
    if (k > n || k < n - f) {
        fprintf(stderr, "#### Error  : wrong number of parameters in the command \"%s\".\n", str);
        return -2;
    }

    int jc = 0, jr = 0, ji = 0, jo = 0;
    bool failed = false;
    for (int i = 0; i < k; i++) {
        com_command* arg = com_getparam(pars, i);
        switch (types[i]) {
        case PAR_CLASS:
            if (!param_getclass(pars, i, str, theclasses + jc)) {

                failed = true;
            }
            jc++;
            break;
        case PAR_RAWTEXT:
            if (!param_getrawtext(pars, i, str, rawtext + jr)) {
                failed = true;
            }
            jr++;
            break;
        case PAR_INTEGER:
            if (!param_getinteger(pars, i, str, integers + ji)) {
                failed = true;
            }
            ji++;
            break;
        case PAR_OBJECTV:
        case PAR_AUTOMATONV:
            if (!com_single(arg) || !check_varname(arg->main->string)) {
                fprintf(stderr, "#### Error  : parameter %d in the command \"%s\" is not a variable.\n", i + 1, str);
                failed = true;
            }
            // fall through
        case PAR_AUTOMATON:
        case PAR_REGEXP:
        case PAR_MORPHISM:
        case PAR_RECDEF:
        case PAR_AUTOMOR:
        case PAR_AUTOREG:
        case PAR_REGMOR:
        case PAR_OBJECT:
        case PAR_REGAUTOMOR:
            objs[jo] = com_apply_command(arg, NULL, MODE_DEFAULT, saved + jo);
            if (objs[jo] == -2) {
                failed = true;
            }
            if (!param_checkobjtype(objs[jo], types[i], i, str)) {
                failed = true;
            }
            jo++;
            break;
        default:
            failed = true;
            break;
        }

        if (failed) {
            for (int h = 0; h < jo; h++) {
                if (saved[h] && objs[h] != -1) {
                    object_free(objs[h]);
                }
            }
            break;
        }
    }

    if (failed) {
        return -2;
    }

    return -1;
}

void usage_generic(par_type* types, int n, par_type typeret, const char* str) {
    fprintf(stderr, "#### Usage  : %s", str);
    if (n > 0) {
        if (n == 1) {
            fprintf(stderr, "(<par>");
        }
        else {
            fprintf(stderr, "(<par1>");
        }
        for (int i = 1; i < n; i++) {
            fprintf(stderr, ", <par%d>", i + 1);
        }
        fprintf(stderr, ")\n");
        for (int i = 0; i < n; i++) {
            if (n == 1) {
                fprintf(stderr, "              <par> is ");
            }
            else {
                fprintf(stderr, "              <par%d> is ", i + 1);
            }
            switch (types[i]) {
            case PAR_AUTOMATON:
                fprintf(stderr, "an automaton.\n");
                break;
            case PAR_REGEXP:
                fprintf(stderr, "a regular expression.\n");
                break;
            case PAR_MORPHISM:
                fprintf(stderr, "a morphism.\n");
                break;
            case PAR_RECDEF:
                fprintf(stderr, "a recursive definition.\n");
                break;
            case PAR_AUTOMATONV:
                fprintf(stderr, "an automaton variable.\n");
                break;
            case PAR_AUTOMOR:
                fprintf(stderr, "an automaton or a morphism.\n");
                break;
            case PAR_AUTOREG:
                fprintf(stderr, "an automaton or a regular expression.\n");
                break;
            case PAR_REGMOR:
                fprintf(stderr, "a regular expression or a morphism.\n");
                break;
            case PAR_OBJECTV:
                fprintf(stderr, "an object variable.\n");
                break;
            case PAR_OBJECT:
                fprintf(stderr, "an object.\n");
                break;
            case PAR_REGAUTOMOR:
                fprintf(stderr, "a regular expression or an automaton or a morphism.\n");
                break;
            case PAR_CLASS:
                fprintf(stderr, "a class name.\n");
                break;
            case PAR_RAWTEXT:
                fprintf(stderr, "raw text.\n");
                break;
            case PAR_INTEGER:
                fprintf(stderr, "an integer.\n");
                break;
            default:
                fprintf(stderr, "ERROR.\n");
                break;
            }
        }
    }
    else {
        fprintf(stderr, "\n");
    }

    switch (typeret) {
    case PAR_AUTOMATON:
        fprintf(stderr, "#### Return : an automaton.\n");
        break;
    case PAR_REGEXP:
        fprintf(stderr, "#### Return : a regular expression.\n");
        break;
    case PAR_MORPHISM:
        fprintf(stderr, "#### Return : a morphism.\n");
        break;
    case PAR_RECDEF:
        fprintf(stderr, "#### Return : a recursive definition.\n");
        break;
    case PAR_NONE:
        fprintf(stderr, "#### Return : nothing.\n");
        break;
    default:
        fprintf(stderr, "#### Return : ERROR.\n");
        break;
    }
}

int com_integer_par(com_parameters* pars, int p, const char* str, int* res) {
    com_command* arg = com_getparam(pars, p);
    if (!com_single(arg)) {
        error_parinteger(str, p);
        return -2;
    }
    char* end;
    *res = strtol(arg->main->string, &end, 10);
    if (*end != '\0') {
        error_parinteger(str, p);
        return -2;
    }
    return -1;
}



/********************************/
/*+ Computation of new objects +*/
/********************************/

int shell_mccluskey_reg(char* varname, com_parameters* pars, const char* str) {

    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_REGEXP, str);
        return -2;
    }
    regexp* exp;
    if (objects[i]->aut->dfa) {
        // Computes the regular expression from the DFA.
        exp = dfa_mccluskey(objects[i]->aut->obj_dfa);
    }
    else {
        // Computes the regular expression from the NFA.
        exp = nfa_mccluskey(objects[i]->aut->obj_nfa);
    }

    if (saved) {
        object_free(i);
    }
    return object_add_regexp(varname, exp);
}

int shell_thompson_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_REGEXP };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    // Computes the automaton from the regular expression.
    nfa* automaton = reg_thompson(objects[i]->exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_glushkov_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_REGEXP };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    // Computes the automaton from the regular expression.
    nfa* automaton = reg_glushkov(objects[i]->exp);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_mirror_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_mirror(objects[i]->aut->obj_dfa);
    }
    else {
        automaton = nfa_mirror(objects[i]->aut->obj_nfa);
    }
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_union_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[] = { false, false };
    par_type types[] = { PAR_AUTOMATON, PAR_AUTOMATON };
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }

    void* obj1 = objects[inds[0]]->aut->dfa ? (void*)objects[inds[0]]->aut->obj_dfa : (void*)objects[inds[0]]->aut->obj_nfa;
    void* obj2 = objects[inds[1]]->aut->dfa ? (void*)objects[inds[1]]->aut->obj_dfa : (void*)objects[inds[1]]->aut->obj_nfa;
    nfa* automaton = nfa_union(obj1, objects[inds[0]]->aut->dfa, obj2, objects[inds[1]]->aut->dfa);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_intersect_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[] = { false, false };
    par_type types[] = { PAR_AUTOMATON, PAR_AUTOMATON };
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }
    if (objects[inds[0]]->aut->dfa && objects[inds[1]]->aut->dfa) {
        dfa* automaton = dfa_intersect(objects[inds[0]]->aut->obj_dfa, objects[inds[1]]->aut->obj_dfa, true);
        if (saved[0]) {
            object_free(inds[0]);
        }
        if (saved[1]) {
            object_free(inds[1]);
        }
        return object_add_automaton_dfa(varname, automaton);
    }


    void* obj1 = objects[inds[0]]->aut->dfa ? (void*)objects[inds[0]]->aut->obj_dfa : (void*)objects[inds[0]]->aut->obj_nfa;
    void* obj2 = objects[inds[1]]->aut->dfa ? (void*)objects[inds[1]]->aut->obj_dfa : (void*)objects[inds[1]]->aut->obj_nfa;

    void* automaton = nfa_intersect_mixed(obj1, objects[inds[0]]->aut->dfa, obj2, objects[inds[1]]->aut->dfa, true);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }

    if (objects[inds[0]]->aut->dfa && objects[inds[1]]->aut->dfa) {
        return object_add_automaton_dfa(varname, (dfa*)automaton);
    }
    else {
        return object_add_automaton_nfa(varname, (nfa*)automaton);

    }


}

int shell_concat_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int inds[2];
    bool saved[] = { false, false };
    par_type types[] = { PAR_AUTOMATON, PAR_AUTOMATON };
    if (param_retrieve(pars, 2, 0, types, NULL, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 2, PAR_AUTOMATON, str);
        return -2;
    }

    void* obj1 = objects[inds[0]]->aut->dfa ? (void*)objects[inds[0]]->aut->obj_dfa : (void*)objects[inds[0]]->aut->obj_nfa;
    void* obj2 = objects[inds[1]]->aut->dfa ? (void*)objects[inds[1]]->aut->obj_dfa : (void*)objects[inds[1]]->aut->obj_nfa;
    nfa* automaton = nfa_concat(obj1, objects[inds[0]]->aut->dfa, obj2, objects[inds[1]]->aut->dfa);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_kleene_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_star(objects[i]->aut->obj_dfa);
    }
    else {
        automaton = nfa_star(objects[i]->aut->obj_nfa);
    }

    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_elimeps_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    if (objects[i]->aut->dfa) {
        dfa* automaton = dfa_copy(objects[i]->aut->obj_dfa);
        if (saved) {
            object_free(i);
        }
        return object_add_automaton_dfa(varname, automaton);
    }
    else {
        nfa* automaton = nfa_elimeps(objects[i]->aut->obj_nfa);
        if (saved) {
            object_free(i);
        }
        return object_add_automaton_nfa(varname, automaton);
    }


}

int shell_trim_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }


    if (objects[i]->aut->dfa) {
        dfa* automaton = dfa_trim(objects[i]->aut->obj_dfa);
        if (saved) {
            object_free(i);
        }
        return object_add_automaton_dfa(varname, automaton);
    }
    else {
        nfa* automaton = nfa_trim(objects[i]->aut->obj_nfa);
        if (saved) {
            object_free(i);
        }
        return object_add_automaton_nfa(varname, automaton);
    }
}

int shell_determinize_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    if (objects[i]->aut->dfa) {
        uint j = shell_copy_generic(i, varname);
        if (saved) {
            object_free(i);
        }
        return j;
    }

    dfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_copy(objects[i]->aut->obj_dfa);
    }
    else {
        // We need to determinize the NFA.
        automaton = nfa_determinize(objects[i]->aut->obj_nfa, true);
    }
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_dfa(varname, automaton);
}

int shell_complement_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    dfa* automaton;

    if (objects[i]->aut->dfa) {
        automaton = dfa_complement(objects[i]->aut->obj_dfa);

    }
    else {
        dfa* determinized = nfa_determinize(objects[i]->aut->obj_nfa, true);
        automaton = dfa_complement(determinized);
        dfa_delete(determinized);
    }

    if (saved) {
        object_free(i);
    }
    return object_add_automaton_dfa(varname, automaton);
}

int shell_hopcroft_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    dfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_hopcroft(objects[i]->aut->obj_dfa);
    }
    else {
        dfa* determinized = nfa_determinize(objects[i]->aut->obj_nfa, true);
        automaton = dfa_hopcroft(determinized);
        dfa_delete(determinized);
    }
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_dfa(varname, automaton);
}

int shell_brzozowski_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }
    dfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_brzozowski(objects[i]->aut->obj_dfa);
    }
    else {
        automaton = nfa_brzozowski(objects[i]->aut->obj_nfa);
    }
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_dfa(varname, automaton);
}

int shell_invtrans(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_to_nfa(objects[i]->aut->obj_dfa);
    }
    else {
        automaton = nfa_copy(objects[i]->aut->obj_nfa);
    }
    parti* par = nfa_inv_ext(automaton);
    delete_parti(par);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton);
}

int shell_dycktrans_nfa(char* varname, com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_AUTOMATON, str);
        return -2;
    }

    nfa* automaton;
    if (objects[i]->aut->dfa) {
        automaton = dfa_to_nfa(objects[i]->aut->obj_dfa);
    }
    else {
        automaton = nfa_copy(objects[i]->aut->obj_nfa);
    }

    parti* SCCS = nfa_inv_ext(automaton);
    parti* FOLD = nfa_stal_fold(automaton, SCCS);
    delete_parti(SCCS);
    nfa* automaton2 = nfa_dyck_ext(automaton, FOLD);
    nfa_delete(automaton);
    delete_parti(FOLD);
    if (saved) {
        object_free(i);
    }
    return object_add_automaton_nfa(varname, automaton2);
}

static void usage_random_nfa(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<nb_alph>, <nb_min>, <nb_max>)\n", str);
    fprintf(stderr, "####          <nb_alph> is the number of letters in the alphabet.\n");
    fprintf(stderr, "####          <nb_min> is the minimum number of states.\n");
    fprintf(stderr, "####          <nb_max> is the maximum number of states.\n");
    fprintf(stderr, "####          <nb_min> must be smaller than <nb_max>.\n");
    fprintf(stderr, "#### Return : a random NFA with <nb_alph> letters and between <nb_min> and <nb_max> states.\n");
}
int shell_random_nfa(char* varname, com_parameters* pars, const char* str) {
    int nb[3];
    par_type types[] = { PAR_INTEGER, PAR_INTEGER, PAR_INTEGER };
    if (param_retrieve(pars, 3, 0, types, NULL, NULL, nb, NULL, NULL, str) == -2) {
        usage_random_nfa(str);
        return -2;
    }
    return object_add_automaton_nfa(varname, nfa_random(nb[0], nb[1], nb[2]));
}

static void usage_random_dfa(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<nb_alph>, <nb_min>, <nb_max>)\n", str);
    fprintf(stderr, "####          <nb_alph> is the number of letters in the alphabet.\n");
    fprintf(stderr, "####          <nb_min> is the minimum number of states.\n");
    fprintf(stderr, "####          <nb_max> is the maximum number of states.\n");
    fprintf(stderr, "####          <nb_min> must be smaller than <nb_max>.\n");
    fprintf(stderr, "#### Return : a random DFA with <nb_alph> letters and between <nb_min> and <nb_max> states.\n");
}
int shell_random_dfa(char* varname, com_parameters* pars, const char* str) {
    int nb[3];
    par_type types[] = { PAR_INTEGER, PAR_INTEGER, PAR_INTEGER };
    if (param_retrieve(pars, 3, 0, types, NULL, NULL, nb, NULL, NULL, str) == -2) {
        usage_random_dfa(str);
        return -2;
    }
    return object_add_automaton_dfa(varname, dfa_random(nb[0], nb[1], nb[2]));
}

/*******************/
/*+ File handling +*/
/*******************/

static void usage_open(const char* str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to a json file that stores an object.\n");
    fprintf(stderr, "#### Return : the object read in the file.\n");
}
int shell_open_object(char* varname, com_parameters* pars, const char* str) {
    char* filename;
    par_type types[] = { PAR_RAWTEXT };
    if (param_retrieve(pars, 1, 0, types, NULL, &filename, NULL, NULL, NULL, str) == -2) {
        usage_open(str);
        return -2;
    }
    return files_read_object(filename, varname);
}

static void usage_save(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<par>,\"<path>\")\n", str);
    fprintf(stderr, "####          <par> is a regular expression or an automaton.\n");
    fprintf(stderr, "####          <path> is the path to the json file were the object has to be saved.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_save_to_file(com_parameters* pars, const char* str) {
    char* filename;
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOREG, PAR_RAWTEXT };
    if (param_retrieve(pars, 2, 0, types, NULL, &filename, NULL, &i, &saved, str) == -2) {
        usage_save(str);
        return -2;
    }
    fprintf(stdout, "#### saving in the file: \"%s\".\n", filename);
    files_save_object(objects[i], filename);
    if (saved) {
        object_free(i);
    }
    return -1;
}

static void usage_open_session(const char* str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to a json file that stores a session.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_load_session(com_parameters* pars, const char* str) {
    char* filename;
    par_type types[] = { PAR_RAWTEXT };
    if (param_retrieve(pars, 1, 0, types, NULL, &filename, NULL, NULL, NULL, str) == -2) {
        usage_open_session(str);
        return -2;
    }
    printf("loading the session saved in the file: \"%s\".\n", filename);
    files_load_session(filename);
    return -1;
}

static void usage_save_session(const char* str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to the json file were the session has to be saved.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_save_session(com_parameters* pars, const char* str) {
    char* filename;
    par_type types[] = { PAR_RAWTEXT };
    if (param_retrieve(pars, 1, 0, types, NULL, &filename, NULL, NULL, NULL, str) == -2) {
        usage_save_session(str);
        return -2;
    }
    printf("saving the session in the file: \"%s\".\n", filename);
    files_save_session(filename);
    return -1;
}

/*************/
/*+ Display +*/
/*************/

int shell_latex_gen(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMOR };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    if (objects[i]->type == AUTOMATON) {
        if (objects[i]->aut->dfa) {
            latex_print_dfa(objects[i]->aut->obj_dfa, stdout);
        }
        else {
            latex_print_nfa(objects[i]->aut->obj_nfa, stdout);
        }
    }
    else if (objects[i]->type == MORPHISM) {
        latex_print_cayley(objects[i]->mor->obj, stdout);
    }

    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_rcayley(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    print_title_box(100, true, stdout, 1, "Right Cayley graph of the morphism");
    view_cayley(objects[i]->mor->obj);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_lcayley(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    print_title_box(100, true, stdout, 1, "Left Cayley graph of the morphism");
    view_left_cayley(objects[i]->mor->obj);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_mormult(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    mor_compute_mult(objects[i]->mor->obj);
    print_title_box(100, true, stdout, 1, "Multiplication table of the morphism");
    mor_mult_print(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_morder(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    mor_compute_order(objects[i]->mor->obj);
    print_title_box(100, true, stdout, 1, "Ordering of the morphism.");
    mor_print_order(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_idems(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    mor_print_idems(objects[i]->mor->obj, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_toggle_optimization(com_parameters* pars, const char* str) {
    if (param_retrieve(pars, 0, 0, NULL, NULL, NULL, NULL, NULL, NULL, str) == -2) {
        usage_generic(NULL, 0, PAR_NONE, str);
        return -2;
    }
    if (optimization_level == LV_FULL) {
        optimization_level = LV_REG;
        fprintf(stdout, "#### Optimization mode for kernels and orbits enabled.\n");
    }
    else {
        optimization_level = LV_FULL;
        fprintf(stdout, "#### Optimization mode for kernels and orbits disabled.\n");
    }
    return -1;
}

int shell_view_mkernel(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    shell_view_object(objects[i], true);
    print_title_box(100, true, stdout, 1, "MOD-kernel of the morphism.");
    print_full_subsemi(shell_compute_ker(i, KER_MOD, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_akernel(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    shell_view_object(objects[i], true);
    print_title_box(100, true, stdout, 1, "AMT-kernel of the morphism.");
    print_full_subsemi(shell_compute_ker(i, KER_AMT, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_view_gkernel(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    shell_view_object(objects[i], true);
    print_title_box(100, true, stdout, 1, "GR-kernel of the morphism.");
    print_full_subsemi(shell_compute_ker(i, KER_GR, optimization_level), stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

static subsemi* shell_compute_orbit_aux(int i, uint e, orbits_type thetype) {
    switch (thetype) {
    case ORB_DD:
        return compute_one_ddorb(objects[i]->mor->obj, e);
        break;
    case ORB_MODP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_MOD, optimization_level), e);
        break;
    case ORB_AMTP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_AMT, optimization_level), e);
        break;
    case ORB_GRP:
        return compute_one_gplusorb(shell_compute_ker(i, KER_GR, optimization_level), e);
        break;
    case ORB_PT:
        return compute_one_ptorb(objects[i]->mor->obj, e, optimization_level);
        break;
    case ORB_BPMOD:
        return compute_one_bpgorb(objects[i]->mor->obj, e, optimization_level, BPG_MOD);
        break;
    case ORB_BPAMT:
        return compute_one_bpgorb(objects[i]->mor->obj, e, optimization_level, BPG_AMT);
        break;
    case ORB_BPGR:
        return compute_one_bpgorb(objects[i]->mor->obj, e, optimization_level, BPG_GR);
        break;
    case ORB_BPDD:
        return compute_one_bpgplusorb(objects[i]->mor->obj, e, optimization_level, BPG_ST);
        break;
    case ORB_BPMODP:
        return compute_one_bpgplusorb(objects[i]->mor->obj, e, optimization_level, BPG_MOD);
        break;
    case ORB_BPAMTP:
        return compute_one_bpgplusorb(objects[i]->mor->obj, e, optimization_level, BPG_AMT);
        break;
    case ORB_BPGRP:
        return compute_one_bpgplusorb(objects[i]->mor->obj, e, optimization_level, BPG_GR);
        break;
    default:
        break;
    }
    return NULL;
}

static void usage_orbits(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<class>, <par>) or %s(<class>, <par>, <idem>)\n", str, str);
    fprintf(stderr, "####          <class> is a class.\n");
    fprintf(stderr, "####          <par> is a morphism.\n");
    fprintf(stderr, "####          <idem> is an idempotent element.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_view_orbits(com_parameters* pars, const char* str) {
    classes cl;
    char* rawe;
    int i;
    bool saved = false;
    par_type types[] = { PAR_CLASS, PAR_MORPHISM, PAR_RAWTEXT };
    if (param_retrieve(pars, 3, 1, types, &cl, &rawe, NULL, &i, &saved, str) == -2) {
        usage_orbits(str);
        return -2;
    }

    char cl_name[32];
    orbits_type thetype;
    switch (cl) {
    case CL_DD:
        sprintf(cl_name, "DD");
        thetype = ORB_DD;
        break;
    case CL_MODP:
        sprintf(cl_name, "MOD⁺");
        thetype = ORB_MODP;
        break;
    case CL_AMTP:
        sprintf(cl_name, "AMT⁺");
        thetype = ORB_AMTP;
        break;
    case CL_GRP:
        sprintf(cl_name, "GR⁺");
        thetype = ORB_GRP;
        break;
    case CL_AT:
        sprintf(cl_name, "AT");
        thetype = ORB_PT;
        break;
    case CL_ATT:
        sprintf(cl_name, "ATT");
        thetype = ORB_PT;
        break;
    case CL_UL:
        sprintf(cl_name, "UL");
        thetype = ORB_PT;
        break;
    case CL_FLST:
        sprintf(cl_name, "FL(ST)");
        thetype = ORB_PT;
        break;
    case CL_PLST:
        sprintf(cl_name, "PL(ST)");
        thetype = ORB_PT;
        break;
    case CL_PT:
        sprintf(cl_name, "PT");
        thetype = ORB_PT;
        break;
    case CL_LT:
        sprintf(cl_name, "LT");
        thetype = ORB_BPDD;
        break;
    case CL_LTT:
        sprintf(cl_name, "LTT");
        thetype = ORB_BPDD;
        break;
    case CL_TLDD:
        sprintf(cl_name, "TL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_FLDD:
        sprintf(cl_name, "FL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_PLDD:
        sprintf(cl_name, "PL(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_BPOLDD:
        sprintf(cl_name, "BPol(DD)");
        thetype = ORB_BPDD;
        break;
    case CL_TLMOD:
        sprintf(cl_name, "TL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_FLMOD:
        sprintf(cl_name, "FL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_PLMOD:
        sprintf(cl_name, "PL(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_BPOLMOD:
        sprintf(cl_name, "BPol(MOD)");
        thetype = ORB_BPMOD;
        break;
    case CL_TLAMT:
        sprintf(cl_name, "TL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_FLAMT:
        sprintf(cl_name, "FL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_PLAMT:
        sprintf(cl_name, "PL(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_BPOLAMT:
        sprintf(cl_name, "BPol(AMT)");
        thetype = ORB_BPAMT;
        break;
    case CL_TLMODP:
        sprintf(cl_name, "TL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_FLMODP:
        sprintf(cl_name, "FL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_PLMODP:
        sprintf(cl_name, "PL(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_BPOLMODP:
        sprintf(cl_name, "BPol(MOD⁺)");
        thetype = ORB_BPMODP;
        break;
    case CL_TLAMTP:
        sprintf(cl_name, "TL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_FLAMTP:
        sprintf(cl_name, "FL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_PLAMTP:
        sprintf(cl_name, "PL(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_BPOLAMTP:
        sprintf(cl_name, "BPol(AMT⁺)");
        thetype = ORB_BPAMTP;
        break;
    case CL_TLGR:
        sprintf(cl_name, "TL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_FLGR:
        sprintf(cl_name, "FL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_PLGR:
        sprintf(cl_name, "PL(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_BPOLGR:
        sprintf(cl_name, "BPol(GR)");
        thetype = ORB_BPGR;
        break;
    case CL_TLGRP:
        sprintf(cl_name, "TL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_FLGRP:
        sprintf(cl_name, "FL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_PLGRP:
        sprintf(cl_name, "PL(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    case CL_BPOLGRP:
        sprintf(cl_name, "BPol(GR⁺)");
        thetype = ORB_BPGRP;
        break;
    default:
        return -1;
        break;
    }

    char title[64];
    sprintf(title, "%s-orbits of the morphism", cl_name);
    print_title_box(100, true, stdout, 1, title);

    if (com_nbparams(pars) == 2) {
        print_all_orbs(shell_compute_orbits(i, thetype, optimization_level), cl_name, stdout);
    }
    else {
        regexp* myexp = parse_string_regexp(rawe);
        if (myexp->op != WORD) {
            fprintf(stderr, "Error: Parameter 3 of the command \"%s\" has to be an idempotent element.\n", str);
            reg_free(myexp);
            if (saved) {
                object_free(i);
            }
            return -2;
        }

        uint e = mor_compute_image(objects[i]->mor->obj, myexp->word);
        reg_free(myexp);
        if (e >= objects[i]->mor->obj->r_cayley->size_graph || !objects[i]->mor->obj->idem_array[e]) {
            fprintf(stderr, "Error: Parameter 3 of the command \"%s\" has to be an idempotent element.\n", str);
            if (saved) {
                object_free(i);
            }
            return -2;
        }
        subsemi* theorbit = shell_compute_orbit_aux(i, e, thetype);

        fprintf(stdout, "#### %s-orbit of the idempotent ", cl_name);
        mor_fprint_name_utf8(objects[i]->mor->obj, e, stdout);
        printf(".\n");
        print_full_subsemi(theorbit, stdout);

        delete_subsemi(theorbit);
    }

    if (saved) {
        object_free(i);
    }
    return -1;
}

static void usage_nfa_runs(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<com>, <word>)\n", str);
    fprintf(stderr, "####          <com> is a command evaluating to an automaton.\n");
    fprintf(stderr, "####          <word> is a word.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_view_nfa_run(com_parameters* pars, const char* str) {
    char* raw;
    int k;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON, PAR_RAWTEXT };
    if (param_retrieve(pars, 2, 0, types, NULL, &raw, NULL, &k, &saved, str) == -2) {
        usage_nfa_runs(str);
        return -2;
    }

    regexp* myexp = parse_string_regexp(raw);
    if (myexp->op != WORD) {
        fprintf(stderr, "Error: Parameter 2 of the command \"%s\" has to be a word.\n", str);
        usage_nfa_runs(str);
        reg_free(myexp);
        return -2;
    }

    shell_view_object(objects[k], true);
    fprintf(stdout, "Running the word %s in this automaton.\n", pars->next->param->main->string);

    if (objects[k]->aut->dfa) {
        dfa* A = objects[k]->aut->obj_dfa;
        uint s = dfa_compute_run(A, myexp->word);
        if (s >= A->trans->size_graph) {
            fprintf(stderr, "This word contains letters outside the alphabet of this automaton.\n");
        }
        else {
            fprintf(stdout, "State reached: ");
            dfa_print_state(A, s, stdout);
            printf(".\n");
            if (bsearch(&s, A->finals, A->nb_finals, sizeof(uint), compare_uint)) {
                printf("The word is accepted.\n");
            }
            else {
                printf("The word is rejected.\n");
            }
        }
    }
    else {
        nfa* A = objects[k]->aut->obj_nfa;
        dequeue* states = nfa_compute_runs(A, myexp->word);
        if (states == NULL) {
            fprintf(stderr, "This word contains letters outside the alphabet of this automaton.\n");
        }
        else {
            fprintf(stdout, "Set of states reached: ");
            if (isempty_dequeue(states)) {
                printf("∅.\n");
            }
            else {
                printf("{");
                for (uint i = 0; i < size_dequeue(states) - 1; i++) {
                    nfa_print_state(A, lefread_dequeue(states, i), stdout);
                    printf(",");
                }
                nfa_print_state(A, lefread_dequeue(states, size_dequeue(states) - 1), stdout);
                printf("}.\n");
            }

            uint i = 0;
            uint j = 0;
            while (i < size_dequeue(states) && j < size_dequeue(A->finals)) {
                if (lefread_dequeue(states, i) == lefread_dequeue(A->finals, j)) {
                    printf("The word is accepted.\n");
                    break;
                }
                else if (lefread_dequeue(states, i) < lefread_dequeue(A->finals, j)) {
                    i++;
                }
                else {
                    j++;
                }
            }
            if (i == size_dequeue(states) || j == size_dequeue(A->finals)) {
                printf("The word is rejected.\n");
            }
        }
    }


    reg_free(myexp);
    if (saved) {
        object_free(k);
    }
    return -1;
}

static void usage_mor_image(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<com>, <word>)\n", str);
    fprintf(stderr, "####          <com> is a command evaluating to a morphism.\n");
    fprintf(stderr, "####          <word> is a word.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_view_mor_image(com_parameters* pars, const char* str) {

    char* raw;
    int k;
    bool saved = false;
    par_type types[] = { PAR_MORPHISM, PAR_RAWTEXT };
    if (param_retrieve(pars, 2, 0, types, NULL, &raw, NULL, &k, &saved, str) == -2) {
        usage_mor_image(str);
        return -2;
    }

    regexp* myexp = parse_string_regexp(raw);
    if (myexp->op != WORD) {
        fprintf(stderr, "Error: Parameter 2 of the command \"%s\" has to be a word.\n", str);
        usage_mor_image(str);
        reg_free(myexp);
        return -2;
    }

    shell_view_object(objects[k], true);
    morphism* M = objects[k]->mor->obj;

    fprintf(stdout, "Computing the image of the word %s by this morphism.\n", pars->next->param->main->string);

    uint s = mor_compute_image(M, myexp->word);

    reg_free(myexp);
    if (s >= M->r_cayley->size_graph) {
        fprintf(stderr, "Error: This is not a valid word for this morphism.\n");
        return -1;
    }
    fprintf(stdout, "Image: ");
    mor_fprint_name_utf8(M, s, stdout);

    // On teste si ce mot est dans le langage.
    if (M->accept_array[s]) {
        fprintf(stdout, "\nThe word belongs to the recognized language.\n");
    }
    else {
        fprintf(stdout, "\nThe word does not belong to the recognized language.\n");
    }
    if (saved) {
        object_free(k);
    }
    return -1;
}

int shell_jep(void) {
    fprintf(stdout, "Please enter the number of states: ");
    getchar();
    return -1;
}

/*******************/
/*+ Property test +*/
/*******************/

int shell_counterfree(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;
    shell_autoprop_cfree(i, "DFA", &error, stdout);
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing counter-freeness\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_autoda(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;

    shell_autoprop_dapat(i, "DFA", &error, stdout);
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing the DA pattern equation.\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_permutation(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;
    shell_autoprop_permut(i, "DFA", &error, stdout);
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing if a DFA is a permutation automaton.\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_cycletrivial(com_parameters* pars, const char* str) {
    // Retrieval of the parameters (errors are handled in the function).
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMATON };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;
    shell_autoprop_cyclet(i, "DFA", &error, stdout);
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing if a DFA is cycle trivial.\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_commutative(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMOR };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;
    switch (objects[i]->type) {
    case AUTOMATON:
        shell_autoprop_commut(i, "DFA", &error, stdout);
        break;
    case MORPHISM:
        shell_morprop_monocom(i, "monoid", stdout);
        break;
    default:
        break;
    }
    if (error < 0) {
        fprintf(stdout, "#### Error received while testing commutativity.\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_letteruniform(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_AUTOMOR };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }

    int error = 0;
    switch (objects[i]->type) {
    case AUTOMATON:
        shell_autoprop_letterind(i, "DFA", &error, stdout);
        break;
    case MORPHISM:
        shell_morprop_letterind(i, "morphism", stdout);
        break;
    default:
        break;
    }
    if (error < 0) {
        fprintf(stdout, "#### Error received while checking letter uniformity.\n");
    }
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_membership(com_parameters* pars, const char* str) {
    classes cl;
    int i;
    bool saved = false;
    par_type types[] = { PAR_CLASS, PAR_REGAUTOMOR };
    if (param_retrieve(pars, 2, 0, types, &cl, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 2, PAR_NONE, str);
        return -2;
    }

    print_title_box(0, true, stdout, 1, "Membership");


    if (class_membership[cl] == NULL) {
        fprintf(stdout, "#### Membership is unsupported for the class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, ".\n\n");
        if (saved) {
            object_free(i);
        }
        return -2;
    }

    fprintf(stdout, "#### ");
    print_command(com_getparam(pars, 0), stdout);
    fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
    print_class_info(cl, stdout);

    print_info_input(i, stdout);

    // Computes the syntactic morphism.
    int j = shell_compute_syntac(i, shell_membership_needs_order(cl));
    if (j == -1) {
        return -2;
    }

    print_title_box(10, true, stdout, 1, "The syntactic morphism.");
    shell_view_object(objects[j], false);
    print_conclusion_comp(stdout, class_membership[cl](j, stdout), class_names[cl]);
    if (saved) {
        object_free(i);
    }
    return -1;
}


static void shell_display_inputs(nfa* A1, nfa* A2, FILE* out) {
    fprintf(out, "#### Displaying NFAs recognizing the two inputs.\n");
    fprintf(out, "#### First input language.\n");
    view_nfa(A1);
    fprintf(out, "#### Second input language.\n");
    view_nfa(A2);
}

int shell_separation(com_parameters* pars, const char* str) {
    classes cl;
    int inds[2];
    bool saved[2] = { false ,false };
    par_type types[] = { PAR_CLASS, PAR_REGAUTOMOR ,PAR_REGAUTOMOR };
    if (param_retrieve(pars, 3, 0, types, &cl, NULL, NULL, inds, saved, str) == -2) {
        usage_generic(types, 3, PAR_NONE, str);
        return -2;
    }

    print_title_box(0, true, stdout, 1, "Separation");
    if (class_separation[cl] == NULL) {
        fprintf(stdout, "#### Separation is unsupported for the class ");
        print_command(com_getparam(pars, 0), stdout);
        fprintf(stdout, ".\n\n");
        if (saved[0]) {
            object_free(inds[0]);
        }
        if (saved[1]) {
            object_free(inds[1]);
        }
        return -2;
    }

    fprintf(stdout, "#### ");
    print_command(com_getparam(pars, 0), stdout);
    fprintf(stdout, " is equal to the class %s.\n", class_names[cl]);
    print_class_info(cl, stdout);


    // We first compute NFAs recognizing the two inputs.
    nfa* A[2];
    for (uchar i = 0; i < 2; i++) {
        switch (objects[inds[i]]->type)
        {
        case AUTOMATON:
            if (objects[inds[0]]->aut->dfa) {
                A[i] = dfa_to_nfa(objects[inds[i]]->aut->obj_dfa);
            }
            else {
                A[i] = nfa_elimeps(objects[inds[i]]->aut->obj_nfa);
                nfa_reset_state_names(A[i]);
            }
            break;
        case REGEXP:
            A[i] = reg_thompson(objects[inds[i]]->exp);
            nfa_elimeps_mod(A[i]);
            nfa_trim_mod(A[i]);
            nfa_reset_state_names(A[i]);
            break;
        case MORPHISM:
            A[i] = dfa_to_nfa(objects[shell_compute_minimal(inds[i])]->aut->obj_dfa);
            break;
        default:
            if (saved[0]) {
                object_free(inds[0]);
            }
            if (saved[1]) {
                object_free(inds[1]);
            }
            return -2;
            break;
        }
    }
    shell_display_inputs(A[0], A[1], stdout);


    print_conclusion_separ(stdout, class_separation[cl](A[0], A[1], false, stdout), class_names[cl]);
    if (saved[0]) {
        object_free(inds[0]);
    }
    if (saved[1]) {
        object_free(inds[1]);
    }
    return -1;
}



int shell_print_chiera(com_parameters* pars, const char* str) {
    int i;
    bool saved = false;
    par_type types[] = { PAR_REGAUTOMOR };
    if (param_retrieve(pars, 1, 0, types, NULL, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 1, PAR_NONE, str);
        return -2;
    }
    shell_chiera_summary(i, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_print_neghiera(com_parameters* pars, const char* str) {
    classes cl;
    int i;
    bool saved = false;
    par_type types[] = { PAR_CLASS, PAR_REGAUTOMOR };
    if (param_retrieve(pars, 2, 0, types, &cl, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 2, PAR_NONE, str);
        return -2;
    }
    shell_neghiera(cl, i, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

int shell_print_fphiera(com_parameters* pars, const char* str) {
    classes cl;
    int i;
    bool saved = false;
    par_type types[] = { PAR_CLASS, PAR_REGAUTOMOR };
    if (param_retrieve(pars, 2, 0, types, &cl, NULL, NULL, &i, &saved, str) == -2) {
        usage_generic(types, 2, PAR_NONE, str);
        return -2;
    }
    shell_fphiera(cl, i, stdout);
    if (saved) {
        object_free(i);
    }
    return -1;
}

/*************************/
/*+ Examples generators +*/
/*************************/



static void usage_initfile_exall(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<classes1>, <classes2>, <nb_states>, <nb_letters>, \"<path>\")\n", str);
    fprintf(stderr, "####          <classes1> is either a single class or of the form out(<cl1>, <cl2>, ...) where the <cli> are classes.\n");
    fprintf(stderr, "####          <classes2> is either a single class or of the form in(<cl1>, <cl2>, ...) where the <cli> are classes.\n");
    fprintf(stderr, "####          <nb_states> is the number of states.\n");
    fprintf(stderr, "####          <nb_letters> is the size of the alphabet.\n");
    fprintf(stderr, "####          <path> is the path to the json file that will be created (or overwritten if it already exists).\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_initfile_exall(com_parameters* pars, const char* str) {
    int n = com_nbparams(pars);
    if (n != 5) {
        fprintf(stderr, "#### Error  : wrong number of parameters in the command \"%s\".\n", str);
        usage_initfile_exall(str);
        return -2;
    }

    exall_profile theprofile;
    theprofile.mode = EXAGEN_ALL;

    // Classes to exclude
    com_keyword lkey = key_from_string_chain_single(com_getparam(pars, 0)->main);
    if (lkey == KY_OUTSIDE) {
        theprofile.nblow = min(com_nbparams(com_getparam(pars, 0)->params), 32);
    }
    else {
        theprofile.nblow = 1;
    }

    if (theprofile.nblow == 0) {
        fprintf(stderr, "#### Error  : At least one forbidden class of languages is required.\n");
        usage_initfile_exall(str);
        return -2;
    }

    if (lkey != KY_OUTSIDE) {
        theprofile.low[0] = command_to_class(com_getparam(pars, 0));
    }
    else {
        com_parameters* lpars = com_getparam(pars, 0)->params;
        for (uint i = 0; i < theprofile.nblow; i++) {
            theprofile.low[i] = command_to_class(com_getparam(lpars, i));
        }
    }

    for (uint i = 0; i < theprofile.nblow; i++) {
        if (theprofile.low[i] == CL_END) {
            fprintf(stdout, "#### Error : Unknow or unsupported class\n");
            usage_initfile_exall(str);
            return -2;
        }
        if (class_membership[theprofile.low[i]] == NULL) {
            fprintf(stdout, "#### Error : Membership is unsupported for the class %s.\n", class_names[theprofile.low[i]]);
            usage_initfile_exall(str);
            return -2;
        }
    }

    // Classes to include
    com_keyword hkey = key_from_string_chain_single(com_getparam(pars, 1)->main);
    if (hkey == KY_INSIDE) {
        theprofile.nbhigh = min(32, com_nbparams(com_getparam(pars, 1)->params));
    }
    else {
        theprofile.nbhigh = 1;
    }

    if (theprofile.nbhigh == 0) {
        fprintf(stderr, "#### Error : At least one constraining class of languages is required.\n");
        usage_initfile_exall(str);
        return -2;
    }


    if (hkey != KY_INSIDE) {
        theprofile.high[0] = command_to_class(com_getparam(pars, 1));
    }
    else {
        com_parameters* hpars = com_getparam(pars, 1)->params;
        for (uint i = 0; i < theprofile.nbhigh; i++) {
            theprofile.high[i] = command_to_class(com_getparam(hpars, i));
        }
    }

    for (uint i = 0; i < theprofile.nbhigh; i++) {
        if (theprofile.high[i] == CL_END) {
            fprintf(stdout, "#### Error : Unknow or unsupported class\n");
            usage_initfile_exall(str);
            return -2;
        }
        if (class_membership[theprofile.high[i]] == NULL) {
            fprintf(stdout, "#### Error : Membership is unsupported for the class %s.\n", class_names[theprofile.high[i]]);
            usage_initfile_exall(str);
            return -2;
        }
    }


    if (!param_getinteger(pars, 2, str, &theprofile.states)) {
        usage_initfile_exall(str);
        return -2;
    }

    if (!param_getinteger(pars, 3, str, &theprofile.alpha)) {
        usage_initfile_exall(str);
        return -2;
    }

    theprofile.done = 0;
    theprofile.found = NULL;
    theprofile.nb_found = 0;
    theprofile.finished = false;

    char* path;
    if (!param_getrawtext(pars, 4, str, &path)) {
        usage_initfile_exall(str);
        return -2;
    }

    if (files_save_exall(path, &theprofile) != -1) {
        fprintf(stderr, "#### Error  : Could not save the file %s.\n", path);
        return -2;
    }

    fprintf(stdout, "#### The file %s has been created.\n", path);
    return -1;
}




static void usage_initfile_negfp(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<class>, <level>, <nb_states>, <nb_letters>, \"<path>\")\n", str);
    fprintf(stderr, "####          <class> is the base class of the hierarchy.\n");
    fprintf(stderr, "####          <level> is the desired level in the hierarchy.\n");
    fprintf(stderr, "####          <nb_states> is the number of states.\n");
    fprintf(stderr, "####          <nb_letters> is the size of the alphabet.\n");
    fprintf(stderr, "####          <path> is the path to the json file that will be created (or overwritten if it already exists).\n");
    fprintf(stderr, "#### Return : nothing (directly stores multiple DFAs in memory).\n");
}
static int shell_initfile_negfp(com_parameters* pars, const char* str, bool fp) {
    classes cl;
    int nb[5];
    char* path;
    par_type types[] = { PAR_CLASS, PAR_INTEGER, PAR_INTEGER, PAR_INTEGER,PAR_RAWTEXT };
    if (param_retrieve(pars, 5, 0, types, &cl, &path, nb, NULL, NULL, str) == -2) {
        usage_initfile_negfp(str);
        return -2;
    }

    if (!class_is_basis(cl)) {
        fprintf(stderr, "#### Error: The class %s is not a valid basis.\n", class_names[cl]);
        return -2;
    }

    exall_profile theprofile;
    if (fp) {
        theprofile.mode = EXAGEN_FPHIERA;
    }
    else {
        theprofile.mode = EXAGEN_DETHIERA;
    }

    theprofile.nblow = nb[0];
    theprofile.low[0] = cl;
    theprofile.states = nb[1];
    theprofile.alpha = nb[2];

    theprofile.done = 0;
    theprofile.found = NULL;
    theprofile.nb_found = 0;
    theprofile.finished = false;


    if (files_save_exall(path, &theprofile) != -1) {
        fprintf(stderr, "#### Error  : Could not save the file %s.\n", path);
        return -2;
    }

    fprintf(stdout, "#### The file %s has been created.\n", path);
    return -1;
}

int shell_initfile_exfp(com_parameters* pars, const char* str) {
    return shell_initfile_negfp(pars, str, true);
}

int shell_initfile_exdet(com_parameters* pars, const char* str) {
    return shell_initfile_negfp(pars, str, false);
}





static void usage_continuefile(const char* str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\")\n", str);
    fprintf(stderr, "####          <path> is the path to the json file containing the search information.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_continuefile(com_parameters* pars, const char* str) {
    par_type types[] = { PAR_RAWTEXT };
    char* path;
    if (param_retrieve(pars, 1, 0, types, NULL, &path, NULL, NULL, NULL, str) == -2) {
        usage_continuefile(str);
        return -2;
    }
    shell_memb_file(path);

    return -1;

}


static void usage_retrievefile(const char* str) {
    fprintf(stderr, "#### Usage  : %s(\"<path>\",\"<prefix>\")\n", str);
    fprintf(stderr, "####          <path> is the path to the json file containing the search information.\n");
    fprintf(stderr, "####          <prefix> is the desired prefix for the generated automata variables.\n");
    fprintf(stderr, "#### Return : nothing.\n");
}
int shell_retrievefile(com_parameters* pars, const char* str) {
    par_type types[] = { PAR_RAWTEXT,PAR_RAWTEXT };
    char* text[2];
    if (param_retrieve(pars, 2, 0, types, NULL, text, NULL, NULL, NULL, str) == -2) {
        usage_retrievefile(str);
        return -2;
    }

    if (!check_varname(text[1])) {
        fprintf(stderr, "#### Error  : The prefix \"%s\" is not a valid variable name.\n", text[1]);
        usage_retrievefile(str);
        return -2;
    }


    shell_file_retrieve(text[0], text[1]);
    return -1;
}


static void usage_browse_dfas(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<classes1>, <classes2>, <nb_states>, <nb_letters>[, <start>, <end>])\n", str);
    fprintf(stderr, "####          <classes1> is either a single class or of the form out(<cl1>, <cl2>, ...) where the <cli> are classes.\n");
    fprintf(stderr, "####          <classes2> is either a single class or of the form in(<cl1>, <cl2>, ...) where the <cli> are classes.\n");
    fprintf(stderr, "####          <nb_states> is the number of states.\n");
    fprintf(stderr, "####          <nb_letters> is the size of the alphabet.\n");
    fprintf(stderr, "####          <start> is the enumeration index of the first DFA to test (optional).\n");
    fprintf(stderr, "####          <end> is the enumeration index of the last DFA to test (optional).\n");
    fprintf(stderr, "#### Return : nothing (directly stores multiple DFAs in memory).\n");
}
int shell_browse_dfas(com_parameters* pars, const char* str) {
    int n = com_nbparams(pars);
    if (n < 4 || n > 6) {
        fprintf(stderr, "#### Error  : wrong number of parameters in the command \"%s\".\n", str);
        usage_browse_dfas(str);
        return -2;
    }

    // Classes to exclude
    uchar nblow;
    com_keyword lkey = key_from_string_chain_single(com_getparam(pars, 0)->main);
    if (lkey == KY_OUTSIDE) {
        nblow = com_nbparams(com_getparam(pars, 0)->params);
    }
    else {
        nblow = 1;
    }

    if (nblow == 0) {
        fprintf(stderr, "#### Error  : At least one forbidden class of languages is required.\n");
        usage_browse_dfas(str);
        return -2;
    }

    classes low[nblow];

    if (lkey != KY_OUTSIDE) {
        low[0] = command_to_class(com_getparam(pars, 0));
    }
    else {
        com_parameters* lpars = com_getparam(pars, 0)->params;
        for (int i = 0; i < nblow; i++) {
            low[i] = command_to_class(com_getparam(lpars, i));
        }
    }

    for (int i = 0; i < nblow; i++) {
        if (low[i] == CL_END) {
            fprintf(stdout, "#### Error : Unknow or unsupported class\n");
            usage_browse_dfas(str);
            return -2;
        }
        if (class_membership[low[i]] == NULL) {
            fprintf(stdout, "#### Error : Membership is unsupported for the class %s.\n", class_names[low[i]]);
            usage_browse_dfas(str);
            return -2;
        }
    }

    // Classes to include
    uchar nbhigh;
    com_keyword hkey = key_from_string_chain_single(com_getparam(pars, 1)->main);
    if (hkey == KY_INSIDE) {
        nbhigh = com_nbparams(com_getparam(pars, 1)->params);
    }
    else {
        nbhigh = 1;
    }

    if (nbhigh == 0) {
        fprintf(stderr, "#### Error : At least one constraining class of languages is required.\n");
        usage_browse_dfas(str);
        return -2;
    }

    classes high[nbhigh];

    if (hkey != KY_INSIDE) {
        high[0] = command_to_class(com_getparam(pars, 1));
    }
    else {
        com_parameters* hpars = com_getparam(pars, 1)->params;
        for (int i = 0; i < nbhigh; i++) {
            high[i] = command_to_class(com_getparam(hpars, i));
        }
    }

    for (int i = 0; i < nbhigh; i++) {
        if (high[i] == CL_END) {
            fprintf(stdout, "#### Error : Unknow or unsupported class\n");
            usage_browse_dfas(str);
            return -2;
        }
        if (class_membership[high[i]] == NULL) {
            fprintf(stdout, "#### Error : Membership is unsupported for the class %s.\n", class_names[high[i]]);
            usage_browse_dfas(str);
            return -2;
        }

        // printf("high: %s\n", class_names[high[i]]);
    }

    int states;
    if (!param_getinteger(pars, 2, str, &states)) {
        usage_browse_dfas(str);
        return -2;
    }

    int alpha;
    if (!param_getinteger(pars, 3, str, &alpha)) {
        usage_browse_dfas(str);
        return -2;
    }

    long start = 0;
    long end = LONG_MAX;

    if (n > 4) {
        if (!param_getlong(pars, 4, str, &start)) {
            usage_browse_dfas(str);
            return -2;
        }
    }

    if (n == 6) {
        if (!param_getlong(pars, 5, str, &end)) {
            usage_browse_dfas(str);
            return -2;
        }
    }


    shell_exall(low, nblow, high, nbhigh, states, start, end, alpha);

    return -1;
}

static void usage_browse_dfas_negfp(const char* str) {
    fprintf(stderr, "#### Usage  : %s(<class>, <level>, <nb_states>, <nb_letters>[, <start>, <end>])\n", str);
    fprintf(stderr, "####          <class> is the base class of the hierarchy.\n");
    fprintf(stderr, "####          <level> is the desired level in the hierarchy.\n");
    fprintf(stderr, "####          <nb_states> is the number of states.\n");
    fprintf(stderr, "####          <nb_letters> is the size of the alphabet.\n");
    fprintf(stderr, "####          <start> is the enumeration index of the first DFA to test (optional).\n");
    fprintf(stderr, "####          <end> is the enumeration index of the last DFA to test (optional).\n");
    fprintf(stderr, "#### Return : nothing (directly stores multiple DFAs in memory).\n");
}
int shell_browse_dfas_neg(com_parameters* pars, const char* str) {
    classes cl;
    int nb[5];
    par_type types[] = { PAR_CLASS, PAR_INTEGER, PAR_INTEGER, PAR_INTEGER,PAR_INTEGER, PAR_INTEGER };
    if (param_retrieve(pars, 6, 2, types, &cl, NULL, nb, NULL, NULL, str) == -2) {
        usage_browse_dfas_negfp(str);
        return -2;
    }

    int n = com_nbparams(pars);
    long start = 0;
    long end = LONG_MAX;
    if (n > 4) {
        if (!param_getlong(pars, 4, str, &start)) {
            usage_browse_dfas_negfp(str);
            return -2;
        }
    }

    if (n == 6) {
        if (!param_getlong(pars, 5, str, &end)) {
            usage_browse_dfas_negfp(str);
            return -2;
        }
    }

    shell_exall_dethiera(cl, nb[0], nb[1], nb[2], start, end, true);

    return -1;
}

int shell_browse_dfas_fp(com_parameters* pars, const char* str) {
    classes cl;
    int nb[5];
    par_type types[] = { PAR_CLASS, PAR_INTEGER, PAR_INTEGER, PAR_INTEGER,PAR_INTEGER, PAR_INTEGER };
    if (param_retrieve(pars, 6, 2, types, &cl, NULL, nb, NULL, NULL, str) == -2) {
        usage_browse_dfas_negfp(str);
        return -2;
    }

    int n = com_nbparams(pars);
    long start = 0;
    long end = LONG_MAX;
    if (n > 4) {
        if (!param_getlong(pars, 4, str, &start)) {
            usage_browse_dfas_negfp(str);
            return -2;
        }
    }

    if (n == 6) {
        if (!param_getlong(pars, 5, str, &end)) {
            usage_browse_dfas_negfp(str);
            return -2;
        }
    }

    shell_exall_dethiera(cl, nb[0], nb[1], nb[2], start, end, false);

    return -1;

    return -1;
}
