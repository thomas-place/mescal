#ifndef _MONOID_DISPLAY_H_
#define _MONOID_DISPLAY_H_

#include <math.h>
#include "monoid.h"
#include "monoid_sub.h"
#include "monoid_orbits.h"

/*  __  __                  _     _                         _ _           _              */
/* |  \/  | ___  _ __ _ __ | |__ (_)___ _ __ ___  ___    __| (_)___ _ __ | | __ _ _   _  */
/* | |\/| |/ _ \| '__| '_ \| '_ \| / __| '_ ` _ \/ __|  / _` | / __| '_ \| |/ _` | | | | */
/* | |  | | (_) | |  | |_) | | | | \__ \ | | | | \__ \ | (_| | \__ \ |_) | | (_| | |_| | */
/* |_|  |_|\___/|_|  | .__/|_| |_|_|___/_| |_| |_|___/  \__,_|_|___/ .__/|_|\__,_|\__, | */
/*                   |_|                                           |_|            |___/  */


/************/
/* Elements */
/************/

/**
 * @brief
 * Displays an element on a given stream: UTF8 version for the indices.
 *
 * @return
 * The length of the displayed element.
 */
uint mor_fprint_name_utf8(morphism*, //!< The morphism.
    uint,       //!< The element.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Prints an element in a given char array: UTF8 version for the indices.
 *
 * @return
 * The length of the displayed element (in bytes).
 */
int mor_sprint_name_utf8(morphism*, //!< The morphism.
    uint,       //!< The element.
    char*      //!< The string (must be large enough).
);


/**
 * @brief
 * Displays an element on a given stream: graphviz version for the indices.
 */
void mor_print_name_gviz(morphism*, //!< The morphism.
    uint,       //!< The element.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays an element on a given stream: UTF8 version for the indices. Aligned mode.
 */
void mor_fprint_name_utf8_aligned(morphism*, //!< The morphism.
    uint,       //!< The element.
    uint,       //!< The minimal number of charcaters that should be used (has to be large enough).
    FILE*      //!< The stream.
);

/**
 * @brief
 * Compute the maximal length of a name within a subset of elements.
 *
 * @remark
 * If the subset is NULL, all elements of the monoid are considered.
 *
 * @return
 * The maximal length.
 */
uint mor_max_name_size(morphism*, //!< The morphism.
    dequeue*   //!< The subset of elements (all elements are considered when NULL).
);

/**
 * @brief
 * Displays a subset of elements on a given stream.
 */
void mor_print_sub(morphism*, //!< The morphism.
    dequeue*,  //!< The subset.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays a subset of elements on a given stream inside a partial box.
 *
 * @remark
 * Line breaks are made automatically.
 */
void mor_print_sub_aligned(morphism*, //!< The morphism.
    dequeue*,  //!< The subset.
    uint,       //!< The width of the box.
    uint,       //!< Padding before the display on the first line.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays a subset of elements on a given stream inside a complete titled box.
 *
 * @remark
 * Line breaks are made automatically.
 */
void mor_print_sub_titled(morphism*, //!< The morphism.
    dequeue*,  //!< The subset.
    uint,       //!< The width of the box.
    char*,     //!< The title.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays the images of all letters in a morphism.
 */
void mor_print_mapping(morphism*, //!< The morphism.
    FILE*      //!< The stream.
);


/**
 * @brief
 * Displays the idempotents elements in a morphism on a given stream.
 */
void mor_print_idems(morphism*, //!< The morphism.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays the ordering of a morphism on a given stream.
 */
void mor_print_order(morphism*, //!< The morphism.
    FILE*      //!< The stream.
);

/**
 * @brief
 * Displays the multiplication table of a morphism on a given stream.
 */
void mor_mult_print(morphism*, //!< The morphism.
    FILE*      //!< The stream.
);


/**
 * @brief
 * Displays the idempotents in a subsemigroup.
 */
void submono_print_idems(subsemi*, //!< The subsemigroup.
    FILE* //!< The stream.
);

/**
 * @brief
 * Displays the ordering on the elements of a subsemigroup.
 */
void submono_print_order(subsemi*, //!< The subsemigroup.
    FILE* //!< The stream.
);



/***********/
/* Display */
/***********/

/**
 * @brief
 * Displays information concerning Green relations on a given stream.
 */
void print_infos_green(morphism*, //!< The morphism.
    FILE* //!< The stream.
);

/**
 * @brief
 * Displays information concerning Green relations on a given stream.
 * Special version used for subsemigroups.
 */
void print_infos_green_sub(subsemi*, //!< The subsemigroup.
    FILE* //!< The stream.
);



/**
 * @brief
 * Displays the eggbox diagram of a particular J-class on a given stream.
 */
void print_jclass_green(morphism*, //!< The morphism.
    uint, //!< The J-class.
    uint, //!< Horizontal padding for the display.
    FILE* //!< The stream.
);

/**
 * @brief
 * Displays the eggbox diagram of a particular J-class inside a subsemigroup on a given stream.
 */
void print_jclass_subsemi(subsemi*, //!< The subsemigroup.
    uint, //!< The J-class.
    uint, //!< Horizontal padding for the display.
    FILE* //!< The stream.
);

/**
 * @brief
 * Displays the eggbox diagrams of all J-classes on a given stream.
 */
void print_full_green(morphism*, //!< The morphism.
    bool, //!< Boolean indicating if a title has to be displayed.
    FILE* //!< The stream.
);

/**
 * @brief
 * Displays the eggbox diagrams of all J-classes inside a subsemigroup on a given stream.
 */
void print_full_subsemi(subsemi*, //!< The subsemigroup.
    FILE* //!< The stream.
);


/**
 * @brief
 * Displays all informations concerning a morphism in a given stream.
 */
void view_morphism(morphism*, //!< The morphism.
    FILE* //!< The stream.
);


/**
 * @brief
 * Displays a single orbit on a given stream.
 */
void print_one_orb(orbits*, //!< The orbits
    uint, //!< Index of the orbit
    char*, //!< Name of the class C such that this is a C-orbit.
    FILE* //!< The stream.
);



/**
 * @brief
 * Displays an orbits set on a given stream.
 */
void print_all_orbs(orbits*, //!< The orbits set.
    char*, //!< Name of the class C such that this is a C-orbit.
    FILE* //!< The stream.
);



#endif // _MONOID_DISPLAY_H_
