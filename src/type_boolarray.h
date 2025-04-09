/**
 * @file type_boolarray.h
 * @brief
 * Implementation of Boolean arrays.
 */
#ifndef BOOLARRAY_H_
#define BOOLARRAY_H_

 /*  ____              _                                                    */
 /* | __ )  ___   ___ | | ___  __ _ _ __     __ _ _ __ _ __ __ _ _   _ ___  */
 /* |  _ \ / _ \ / _ \| |/ _ \/ _` | '_ \   / _` | '__| '__/ _` | | | / __| */
 /* | |_) | (_) | (_) | |  __/ (_| | | | | | (_| | |  | | | (_| | |_| \__ \ */
 /* |____/ \___/ \___/|_|\___|\__,_|_| |_|  \__,_|_|  |_|  \__,_|\__, |___/ */
 /*                                                              |___/      */

#include "tools.h"



/**
 * @brief
 * Type used to represent an array of Booleans.
 *
 * @details
 * An array of Booleans is represented by an array of usigned char. Each
 * usigned char stores 8 Boolean values.
 */

typedef struct {
    uchar* array;              //!< The array.
    uint size;                 //!< The number of stored Boolean values.
    uint size_array;           //!< The size of the actual array.
} barray;

/**
 * @brief
 * Creation of an array of Booleans.
 *
 * @remark
 * All cells are initialized to the false value.
 *
 * @return
 * The array.
 */
barray* create_barray(uint //!< The size
);

/**
 * @brief
 * Computation of the size of an array of Booleans.
 *
 * @return
 * The size of the array.
 */
uint getsize_barray(barray*   //!< The array of Booleans.
);

/**
 * @brief
 * Release of an array of Booleans.
 */
void delete_barray(barray*   //!< The array of Booleans.
);

/**
 * @brief
 * Sets a cell to true in an array of Booleans.
 */
void settrue_barray(barray*,  //!< The array of Booleans.
    uint                      //!< The cell.
);

/**
 * @brief
 * Sets a cell to false in an array of Booleans.
 */
void setfalse_barray(barray*, //!< The array of Booleans.
    uint                      //!< The cell.
);

/**
 * @brief
 * Returns the value of a cell in an array of Booleans.
 *
 * @return
 * The Boolean value.
 */
bool getval_barray(barray*,   //!< The array of Booleans.
    uint                      //!< The cell.
);

/**
 * @brief
 * Bitwise disjunction of two Boolean arrays of the same size.
 *
 * @attention
 * If the arrays have distinct size, a NULL pointer is returned.
 *
 * @return
 * A new array containing the bitwise disjunction.
 */
barray* or_barray(barray*,    //!< The first array of Booleans.
    barray*                   //!< The second array of Booleans.
);


/**
 * @brief
 * Bitwise disjunction of two Boolean arrays of the same size. Modifies the first array.
 *
 * @attention
 * If the arrays have distinct size, the first array is not modified.
 */
void or_barray_mod(barray*,    //!< The first array of Booleans.
    barray*                    //!< The second array of Booleans.
);

/**
 * @brief
 * Bitwise conjunction of two Boolean arrays of the same size.
 *
 * @attention
 * If the arrays have distinct size, a NULL pointer is returned.
 *
 * @return
 * A new array containing the bitwise conjunction.
 */
barray* and_barray(barray*,   //!< The first array of Booleans.
    barray*                   //!< The second array of Booleans.
);


/**
 * @brief
 * Bitwise conjunction of two Boolean arrays of the same size. Modifies the first array.
 *
 * @attention
 * If the arrays have distinct size, the first array is not modified.
 */
void and_barray_mod(barray*,   //!< The first array of Booleans.
    barray*                    //!< The second array of Booleans.
);

/**
 * @brief
 * Copy of an array of Booleans.
 *
 * @return
 * The copy.
 */
barray* copy_barray(barray*   //!< The array of Booleans.
);



#endif
