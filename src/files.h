/***********************/
/* Gestion de fichiers */
/***********************/

#ifndef FILES_H
#define FILES_H

#include "main.h"
#include "types.h"
#include "nfa.h"
#include "parser.h"
#include "regexp.h"
#include "shell_languages.h"
#include <json-c/json.h>

#define EXPSIZE 2048


/***********************************************/
/* Manipulation de structures JSON - Automates */
/***********************************************/

json_object* files_auto_to_json(automata*);
json_object* files_lang_to_json(language*);

p_nfa files_json_to_nfa(json_object*);


/************************************/
/* Lecture/écriture dans un fichier */
/************************************/

void files_save_object(object*, char*);

void files_read_object(char* filename, char* varname);

void files_save_session(char* filename);

void files_load_session(char* filename);

#endif
