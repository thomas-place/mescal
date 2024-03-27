/***********************/
/* Gestion de fichiers */
/***********************/

#include "files.h"

unsigned short autocount = 0;

/***********************************************/
/* Manipulation de structures JSON - Automates */
/***********************************************/

json_object *files_auto_to_json(automata *theauto)
{
    // Création de l'objet json
    json_object *root = json_object_new_object();
    if (!root)
    {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }

    json_object_object_add(root, "TYPE", json_object_new_string("AUTO"));
    p_nfa A = theauto->nfa;

    // Taille de l'alphabet
    json_object_object_add(root, "N_ALPH", json_object_new_int(A->trans->size_alpha));

    // Taille de l'automate (nombre d'états)
    json_object_object_add(root, "N_SIZE", json_object_new_int(A->trans->size_graph));

    // Liste des transitions (on commence par compter le nombre de transitions)
    uint num_trans = 0;
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            num_trans = num_trans + size_vertices(A->trans->edges[q][a]);
        }
    }

    json_object *trans = json_object_new_array_ext(num_trans);
    for (uint q = 0; q < A->trans->size_graph; q++)
    {
        for (uint a = 0; a < A->trans->size_alpha; a++)
        {
            for (uint i = 0; i < size_vertices(A->trans->edges[q][a]); i++)
            {
                json_object *onetrans = json_object_new_array_ext(3);
                json_object_array_add(onetrans, json_object_new_int(q));
                json_object_array_add(onetrans, json_object_new_int(a));
                json_object_array_add(onetrans,
                                      json_object_new_int(lefread_vertices(A->trans->edges[q][a],
                                                                           i)));
                json_object_array_add(trans, onetrans);
            }
        }
    }
    json_object_object_add(root, "L_TRAN", trans);

    // Liste des états initiaux
    json_object *initials = json_object_new_array_ext(size_vertices(A->initials));
    for (uint i = 0; i < size_vertices(A->initials); i++)
    {
        json_object_array_add(initials, json_object_new_int(lefread_vertices(A->initials, i)));
    }

    json_object_object_add(root, "L_INIT", initials);

    // Liste des états finaux
    json_object *finals = json_object_new_array_ext(size_vertices(A->finals));
    for (uint i = 0; i < size_vertices(A->finals); i++)
    {
        json_object_array_add(finals, json_object_new_int(lefread_vertices(A->finals, i)));
    }
    json_object_object_add(root, "L_FINA", finals);

    // Liste des transitions epsilon (si l'automate en contient)
    if (A->etrans != NULL)
    {
        uint num_etrans = 0;
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            num_etrans = num_etrans + size_vertices(A->etrans->edges[q]);
        }

        json_object *etrans = json_object_new_array_ext(num_etrans);
        for (uint q = 0; q < A->trans->size_graph; q++)
        {
            for (uint i = 0; i < size_vertices(A->etrans->edges[q]); i++)
            {
                json_object *onetrans = json_object_new_array_ext(2);
                json_object_array_add(onetrans, json_object_new_int(q));
                json_object_array_add(onetrans,
                                      json_object_new_int(lefread_vertices(A->etrans->edges[q],
                                                                           i)));
                json_object_array_add(etrans, onetrans);
            }
        }
        json_object_object_add(root, "L_ETRA", etrans);
    }

    // Liste des transitions inverses (si l'automate en contient)
    if (A->itrans != NULL)
    {
        uint num_itrans = 0;
        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            for (uint a = 0; a < A->itrans->size_alpha; a++)
            {
                num_itrans = num_itrans + size_vertices(A->itrans->edges[q][a]);
            }
        }

        json_object *itrans = json_object_new_array_ext(num_itrans);

        for (uint q = 0; q < A->itrans->size_graph; q++)
        {
            for (uint a = 0; a < A->itrans->size_alpha; a++)
            {
                for (uint i = 0; i < size_vertices(A->itrans->edges[q][a]); i++)
                {
                    json_object *onetrans = json_object_new_array_ext(3);
                    json_object_array_add(onetrans, json_object_new_int(q));
                    json_object_array_add(onetrans, json_object_new_int(a));
                    json_object_array_add(onetrans,
                                          json_object_new_int(lefread_vertices(
                                                                  A->trans->edges[q][a], i)));
                    json_object_array_add(itrans, onetrans);
                }
            }
        }
        json_object_object_add(root, "L_ITRA", itrans);
    }
    return root;
}

json_object *files_lang_to_json(language *thelang)
{
    json_object *root = json_object_new_object();
    if (!root)
    {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return NULL;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("LANG"));
    if (thelang->type == SPE_REG)
    {
        json_object_object_add(root, "SPEC", json_object_new_string("REXP"));
        char *newexp;
        MALLOC(newexp, EXPSIZE);
        newexp[0] = 0;
        reg_to_string(thelang->spe.reg, newexp);
        json_object_object_add(root, "REXP", json_object_new_string(newexp));
        free(newexp);
    }
    if (thelang->type == SPE_NFA)
    {
        json_object_object_add(root, "SPEC", json_object_new_string("AUTO"));
        json_object_object_add(root, "AUTO",
                               files_auto_to_json(objects[thelang->spe.nfa]->theob.aut));
    }
    return root;
}

p_nfa files_json_to_nfa(json_object *root)
{
    // Récupération de la taille de l'automate
    json_object *jsize_graph;
    if (!json_object_object_get_ex(root, "N_SIZE",
                                   &jsize_graph) ||
        json_object_get_type(jsize_graph) != json_type_int)
    {
        fprintf(stderr, "Error: cannot read the size of the automaton.\n");
        return NULL;
    }
    uint size_graph = json_object_get_int(jsize_graph);

    // Récupération de la taille de l'alphabet
    json_object *jsize_alpha;
    if (!json_object_object_get_ex(root, "N_ALPH",
                                   &jsize_alpha) ||
        json_object_get_type(jsize_alpha) != json_type_int)
    {
        fprintf(stderr, "Error: cannot read the size of the alphabet.\n");
        return NULL;
    }
    uint size_alpha = json_object_get_int(jsize_alpha);

    // Création du NFA
    p_nfa A;
    MALLOC(A, 1);
    A->trans    = create_lgraph_noedges(size_graph, size_alpha);
    A->initials = create_vertices();
    A->finals   = create_vertices();
    A->names    = NULL;
    A->ntype    = NONAME;
    A->etrans   = NULL;
    A->itrans   = NULL;

    // Récupération du graphe des transitions
    json_object *jtrans;
    if (!json_object_object_get_ex(root, "L_TRAN",
                                   &jtrans) || json_object_get_type(jtrans) != json_type_array)
    {
        fprintf(stderr, "Error: cannot read the list of transitions.\n");
        delete_nfa(A);
        return NULL;
    }
    uint len = json_object_array_length(jtrans);

    for (uint i = 0; i < len; i++)
    {
        json_object *onetrans = json_object_array_get_idx(jtrans, i);
        if (json_object_get_type(onetrans) != json_type_array ||
            json_object_array_length(onetrans) != 3)
        {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            delete_nfa(A);
            return NULL;
        }
        json_object *jq = json_object_array_get_idx(onetrans, 0);
        json_object *ja = json_object_array_get_idx(onetrans, 1);
        json_object *jr = json_object_array_get_idx(onetrans, 2);
        if (json_object_get_type(jq) != json_type_int || json_object_get_type(jr) != json_type_int)
        {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            delete_nfa(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        uint r = json_object_get_int(jr);
        uint a;
        if (json_object_get_type(ja) == json_type_int)
        {
            a = json_object_get_int(ja);
        }
        else if (json_object_get_type(ja) == json_type_string &&
                 json_object_get_string_len(ja) == 1)
        {
            const char *sa = json_object_get_string(ja);
            a = sa[0];
            if ('a' <= a && a <= 'z')
            {
                a = a - 'a';
            }
            else
            {
                fprintf(stderr, "Error: incorrect letter used in the transitions.\n");
                delete_nfa(A);
                return NULL;
            }
        }
        else
        {
            fprintf(stderr, "Error: cannot read the list of transitions.\n");
            delete_nfa(A);
            return NULL;
        }

        if (q < size_graph && r < size_graph && a < size_alpha)
        {
            rigins_vertices(r, A->trans->edges[q][a]);
        }
        else
        {
            fprintf(stderr, "Error: incorrect letter or state used in the transitions.\n");
            delete_nfa(A);
            return NULL;
        }
    }

    for (uint q = 0; q < size_graph; q++)
    {
        for (uint a = 0; a < size_alpha; a++)
        {
            sort_vertices(A->trans->edges[q][a]);
        }
    }

    // Récupération de la liste des états initiaux
    A->initials = create_vertices();
    json_object *jini;
    if (!json_object_object_get_ex(root, "L_INIT",
                                   &jini) || json_object_get_type(jini) != json_type_array)
    {
        fprintf(stderr, "Error: cannot read the list of initial states.\n");
        delete_nfa(A);
        return NULL;
    }
    len = json_object_array_length(jini);
    for (uint i = 0; i < len; i++)
    {
        json_object *jq = json_object_array_get_idx(jini, i);
        if (json_object_get_type(jq) != json_type_int)
        {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            delete_nfa(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph)
        {
            rigins_vertices(q, A->initials);
        }
        else
        {
            fprintf(stderr, "Error: incorrect state in the list of initial states.\n");
            delete_nfa(A);
            return NULL;
        }
    }
    sort_vertices(A->initials);

    // Récupération de la liste des états finaux
    A->finals = create_vertices();
    json_object *jfin;
    if (!json_object_object_get_ex(root, "L_FINA",
                                   &jfin) || json_object_get_type(jfin) != json_type_array)
    {
        fprintf(stderr, "Error: cannot read the list of final states.\n");
        delete_nfa(A);
        return NULL;
    }
    len = json_object_array_length(jfin);
    for (uint i = 0; i < len; i++)
    {
        json_object *jq = json_object_array_get_idx(jfin, i);
        if (json_object_get_type(jq) != json_type_int)
        {
            fprintf(stderr, "Error: cannot read the list of final states.\n");
            delete_nfa(A);
            return NULL;
        }
        uint q = json_object_get_int(jq);
        if (q < size_graph)
        {
            rigins_vertices(q, A->finals);
        }
        else
        {
            fprintf(stderr, "Error: incorrect state in the list of final states.\n");
            delete_nfa(A);
            return NULL;
        }
    }
    sort_vertices(A->finals);

    // Récupération des transitions epsilon (si il y en a)
    json_object *jetrans;
    if (json_object_object_get_ex(root, "L_ETRA", &jetrans))
    {
        if (json_object_get_type(jetrans) != json_type_array)
        {
            fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
            delete_nfa(A);
            return NULL;
        }
        init_epstrans(A);
        len = json_object_array_length(jetrans);
        for (uint i = 0; i < len; i++)
        {
            json_object *onetrans = json_object_array_get_idx(jetrans, i);
            if (json_object_get_type(onetrans) != json_type_array ||
                json_object_array_length(onetrans) != 2)
            {
                fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
                delete_nfa(A);
                return NULL;
            }
            json_object *jq = json_object_array_get_idx(onetrans, 0);
            json_object *jr = json_object_array_get_idx(onetrans, 1);
            if (json_object_get_type(jq) != json_type_int ||
                json_object_get_type(jr) != json_type_int)
            {
                fprintf(stderr, "Error: cannot read the list of epsilon transitions.\n");
                delete_nfa(A);
                return NULL;
            }
            uint q = json_object_get_int(jq);
            uint r = json_object_get_int(jr);

            if (q < size_graph && r < size_graph)
            {
                rigins_vertices(r, A->etrans->edges[q]);
            }
            else
            {
                fprintf(stderr,
                        "Error: incorrect state used in the list of epsilon transitions.\n");
                delete_nfa(A);
                return NULL;
            }
        }
        for (uint q = 0; q < size_graph; q++)
        {
            sort_vertices(A->etrans->edges[q]);
        }
    }

    // Récupération des transitions inverses (si il y en a)
    json_object *jitrans;
    if (json_object_object_get_ex(root, "L_ITRA", &jitrans))
    {
        if (json_object_get_type(jitrans) != json_type_array)
        {
            fprintf(stderr, "Error: cannot read the list of inverse transitions.\n");
            delete_nfa(A);
            return NULL;
        }
        A->itrans = create_lgraph_noedges(size_graph, size_alpha);
        len       = json_object_array_length(jitrans);
        for (uint i = 0; i < len; i++)
        {
            json_object *onetrans = json_object_array_get_idx(jitrans, i);
            if (json_object_get_type(onetrans) != json_type_array ||
                json_object_array_length(onetrans) != 3)
            {
                fprintf(stderr, "Error: cannot read the list of inverse transitions.\n");
                delete_nfa(A);
                return NULL;
            }
            json_object *jq = json_object_array_get_idx(onetrans, 0);
            json_object *ja = json_object_array_get_idx(onetrans, 1);
            json_object *jr = json_object_array_get_idx(onetrans, 2);
            if (json_object_get_type(jq) != json_type_int ||
                json_object_get_type(jr) != json_type_int)
            {
                fprintf(stderr, "Error: cannot read the list of inverse transitions.\n");
                delete_nfa(A);
                return NULL;
            }
            uint q = json_object_get_int(jq);
            uint r = json_object_get_int(jr);
            uint a;
            if (json_object_get_type(ja) == json_type_int)
            {
                a = json_object_get_int(ja);
            }
            else if (json_object_get_type(ja) == json_type_string &&
                     json_object_get_string_len(ja) == 1)
            {
                const char *sa = json_object_get_string(ja);
                a = sa[0];
                if ('a' <= a && a <= 'z')
                {
                    a = a - 'a';
                }
                else
                {
                    fprintf(stderr,
                            "Error: incorrect letter used in the list of inverse transitions.\n");
                    delete_nfa(A);
                    return NULL;
                }
            }
            else
            {
                fprintf(stderr, "Error: cannot read the list of inverse transitions.\n");
                delete_nfa(A);
                return NULL;
            }

            if (q < size_graph && r < size_graph && a < size_alpha)
            {
                rigins_vertices(r, A->itrans->edges[q][a]);
            }
            else
            {
                fprintf(stderr,
                        "Error: incorrect letter or state used in the list of inverse transitions.\n");
                delete_nfa(A);
                return NULL;
            }
        }
        for (uint q = 0; q < size_graph; q++)
        {
            for (uint a = 0; a < size_alpha; a++)
            {
                sort_vertices(A->itrans->edges[q][a]);
            }
        }
    }
    return A;
}

/************************************/
/* Lecture/écriture dans un fichier */
/************************************/

void files_save_object(object *theobj, char *filename)
{
    json_object *root = NULL;
    switch (theobj->type)
    {
    case LANGUAGE:
        root = files_lang_to_json(theobj->theob.lan);
        break;
    case AUTOMATA:
        root = files_auto_to_json(theobj->theob.aut);
        break;
    default:
        return;
        break;
    }
    // Sauvegarde
    if (json_object_to_file(filename, root))
    {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    }
    else
    {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

static void files_read_object_aux(json_object *root, char *varname)
{
    json_object *obj_type;
    if (!json_object_object_get_ex(root, "TYPE",
                                   &obj_type) ||
        json_object_get_type(obj_type) != json_type_string)
    {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return;
    }
    const char *thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "AUTO") == 0)
    {
        fprintf(stdout, "Reading an automaton.\n");
        p_nfa A = files_json_to_nfa(root);
        json_object_put(root);
        object_add_automata(varname, A);
        return;
    }
    if (strcmp(thetype, "LANG") == 0)
    {
        fprintf(stdout, "Reading a language.\n");
        json_object *lang_spe;
        if (!json_object_object_get_ex(root, "SPEC",
                                       &lang_spe) ||
            json_object_get_type(lang_spe) != json_type_string)
        {
            fprintf(stderr, "Error: cannot read the specification type of the language.\n");
            return;
        }
        const char *thespec = json_object_get_string(lang_spe);
        if (strcmp(thespec, "REXP") == 0)
        {
            json_object *theregexp;
            if (!json_object_object_get_ex(root, "REXP",
                                           &theregexp) ||
                json_object_get_type(theregexp) != json_type_string)
            {
                fprintf(stderr,
                        "Error: cannot read the regular expression specifying the language.\n");
                return;
            }
            const char *exp = json_object_get_string(theregexp);
            regexp *myexp   = parse_string_regexp(exp);
            if (myexp == NULL)
            {
                return;
            }
            reg_print(reg_simplify(myexp));
            object_add_language_reg(varname, myexp);
        }
        if (strcmp(thespec, "AUTO") == 0)
        {
            json_object *theauto;
            if (!json_object_object_get_ex(root, "AUTO",
                                           &theauto) ||
                json_object_get_type(theauto) != json_type_object)
            {
                fprintf(stderr, "Error: cannot read the automaton specifying the language.\n");
                return;
            }
            p_nfa A = files_json_to_nfa(theauto);
            char *newname;
            MALLOC(newname, 20);
            sprintf(newname, "SYSAUTO%04d", autocount++);
            int j = object_add_automata(newname, A);
            object_add_language_nfa(varname, j);
            return;
        }

        return;
    }
}

void files_read_object(char *filename, char *varname)
{
    json_object *root = json_object_from_file(filename);
    if (!root)
    {
        fprintf(stderr, "Failed to read the file.\n");
        return;
    }
    files_read_object_aux(root, varname);
    json_object_put(root);

    return;
}

void files_save_session(char *filename)
{
    // Création de l'objet json
    json_object *root = json_object_new_object();
    if (!root)
    {
        fprintf(stderr, "Failed to create the JSON object.\n");
        return;
    }
    json_object_object_add(root, "TYPE", json_object_new_string("SESSION"));
    uint size = 0;
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->parent == -1)
        {
            size++;
        }
    }

    json_object *thetable = json_object_new_array_ext(size);
    for (int i = 0; i < nb_objects; i++)
    {
        if (objects[i]->parent == -1)
        {
            json_object *oneobj = json_object_new_array_ext(2);
            json_object_array_add(oneobj, json_object_new_string(objects[i]->name));
            if (objects[i]->type == LANGUAGE)
            {
                json_object_array_add(oneobj, files_lang_to_json(objects[i]->theob.lan));
            }
            if (objects[i]->type == AUTOMATA)
            {
                json_object_array_add(oneobj, files_auto_to_json(objects[i]->theob.aut));
            }

            json_object_array_add(thetable, oneobj);
        }
    }
    json_object_object_add(root, "OBJS", thetable);
    // Sauvegarde
    if (json_object_to_file(filename, root))
    {
        fprintf(stderr, "Error: failed to save %s.\n", filename);
    }
    else
    {
        printf("%s saved.\n", filename);
    }

    // Libération du JSON
    json_object_put(root);
}

void files_load_session(char *filename)
{
    json_object *root = json_object_from_file(filename);
    if (!root)
    {
        fprintf(stderr, "Failed to read the file.\n");
        return;
    }
    // Récupération du type
    json_object *obj_type;
    if (!json_object_object_get_ex(root, "TYPE",
                                   &obj_type) ||
        json_object_get_type(obj_type) != json_type_string)
    {
        fprintf(stderr, "Error: cannot read the type of the object.\n");
        return;
    }

    const char *thetype = json_object_get_string(obj_type);
    if (strcmp(thetype, "SESSION") != 0)
    {
        fprintf(stderr, "Error: not a session file.\n");
        return;
    }

    // Récupération des objets
    json_object *jobjs;
    if (!json_object_object_get_ex(root, "OBJS",
                                   &jobjs) || json_object_get_type(jobjs) != json_type_array)
    {
        fprintf(stderr, "Error: cannot read the list of objects.\n");
        return;
    }
    uint len = json_object_array_length(jobjs);

    for (uint i = 0; i < len; i++)
    {
        json_object *oneobj = json_object_array_get_idx(jobjs, i);
        if (json_object_get_type(oneobj) != json_type_array ||
            json_object_array_length(oneobj) != 2)
        {
            fprintf(stderr, "Error: cannot read the list of objects.\n");
            return;
        }
        json_object *varname = json_object_array_get_idx(oneobj, 0);
        json_object *theobje = json_object_array_get_idx(oneobj, 1);

        const char *thename  = json_object_get_string(varname);
        files_read_object_aux(theobje, strdup(thename));
    }

    json_object_put(root);
}
