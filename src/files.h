/**
 * @file files.h
 * @brief
 * Management of savefiles.
 *
 * @details
 * Contains all the functions that can be used to save objects in files and then
 * load these files in the software.
 *
 * @remark
 * Savefiles are formatted in JSON. All functions depend on the json-c library.
 */

#ifndef FILES_H
#define FILES_H

 /*  __  __                         _               ____                        */
 /* |  \/  | __ _ _ __   __ _  __ _(_)_ __   __ _  / ___|  __ ___   _____  ___  */
 /* | |\/| |/ _` | '_ \ / _` |/ _` | | '_ \ / _` | \___ \ / _` \ \ / / _ \/ __| */
 /* | |  | | (_| | | | | (_| | (_| | | | | | (_| |  ___) | (_| |\ V /  __/\__ \ */
 /* |_|  |_|\__,_|_| |_|\__,_|\__, |_|_| |_|\__, | |____/ \__,_| \_/ \___||___/ */
 /*                           |___/         |___/                               */

#include "main.h"
#include "nfa.h"
#include "parser.h"
#include "regexp.h"
#include "shell_languages.h"
#include "type_basic.h"
#include <json-c/json.h>

#define OUTPUT_DIR "./outputs"

/***********************/
/* Regular expressions */
/***********************/

/**
 * @brief
 * Maximal size of a regular expression that can be saved.
 */
#define EXPSIZE 2048

 /**
  * @brief
  * Converts a regular expression into a string.
  *
  * @remark
  * The string is returned by writing into the second parameter which
  * has to be large enough.
  */
void reg_to_string(regexp*, //!< The regular expression.
    char* //!< The buffer used to return the string (it needs to be large enough).
);

/**
 * @brief
 * Converts a regular expression into a json object.
 *
 * @return
 * The json object
 */
json_object* files_regexp_to_json(regexp* //!< The regular expression.
);

/************/
/* Automata */
/************/

/**
 * @brief
 * Converts an automaton into a json object.
 *
 * @return
 * The json object
 */
json_object* files_auto_to_json(nfa* //!< The automaton.
);

/**
 * @brief
 * Reads a json object encoding an automaton and extracts this automaton.
 *
 * @return
 * The automaton (or NULL is the json object does not encode an automaton).
 */
nfa* files_json_to_nfa(json_object* //!< The json object.
);

/***********************/
/* Save/Load in a file */
/***********************/

/**
 * @brief
 * Saves a shell object into a file.
 */
void files_save_object(object*, //!< The object that needs to be saved.
    char* //!< The path to the file in which the save has to be made.
);

/**
 * @brief
 * Reads a shell object from a file and assigns it to a variable name.
 */
void files_read_object(char*, //!< The path to the file that needs to be read.
    char* //!< The variable name.
);

/**
 * @brief
 * Saves a complete session in a file.
 */
void files_save_session(char* //!< The path to the file in which the save has to be made.
);

/**
 * @brief
 * Loads a complete session from a file.
 */
void files_load_session(char* //!< The path to the file that needs to be read.
);

#endif
