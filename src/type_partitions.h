/**
 * @file type_partitions.h
 *
 * @brief
 * Types used to represent partitions. Includes the union-find algorithm.
 */

#ifndef PARTITIONS_H
#define PARTITIONS_H

/*  ____            _   _ _   _                  */
/* |  _ \ __ _ _ __| |_(_) |_(_) ___  _ __  ___  */
/* | |_) / _` | '__| __| | __| |/ _ \| '_ \/ __| */
/* |  __/ (_| | |  | |_| | |_| | (_) | | | \__ \ */
/* |_|   \__,_|_|   \__|_|\__|_|\___/|_| |_|___/ */

#include "alloc.h"
#include "stdbool.h"
#include "stdlib.h"
#include "type_basic.h"
#include "type_dequeue.h"

/**************/
/* First Type */
/**************/

/**
 * @brief
 * First type used to represent a partition. This is not the one used by
 * Union-Find.
 *
 * @remark
 * This type is designed for an easy manipulation of the partition once it has
 * been computed.
 */
typedef struct {
    uint size_set; //!< Size of the partitioned set (the set itself is
                   //!< {0,...,size_set-1}).
    uint size_par; //!< Size of the partition.
    uint *numcl; //!< Array indexed by the partitioned set. Each element in the
                 //!< set is mapped to its class.
    dequeue *
        *cl; //!< Array indexed by the classes. Each class is mapped to the list
             //!< sorted in increasing order of all elements that it contains.
} parti;

/**
 * @brief
 * Release of a partition.
 */
void delete_parti(parti * //!< The partition that needs to be freed.
);

/**
 * @brief
 * Checks whether a partition is trivial (all classes are singletons).
 *
 * @return
 * A Boolean indicating whether the partition is trivial.
 */
bool istrivial_parti(parti * //!< The partition.
);

/**
 * @brief
 * Restriction of a partition to a subset of the partitioned set.
 *
 * @remark
 * Ordering of the classes is preserved.
 *
 * @return
 * The restricted partition.
 */
parti *restrict_parti(
    parti *, //!< The partition.
    uint,    //!< The size of the subset.
    bool *, //!< The array of Booleans indicating which elements are to be kept.
    uint *  //!< The array mapping each kept element to its index in the subset.
);

/**
 * @brief
 * Restriction of a partition to a subset of the partitioned set. Input
 * partition is already a partition of a subset.
 *
 * @remark
 * Ordering of the classes is preserved.
 *
 * @return
 * The restricted partition.
 */
parti *restrict_parti_subset(
    parti *, //!< The partition.
    uint,    //!< The size of the subset.
    bool *, //!< The array of Booleans indicating which elements are to be kept.
    uint *, //!< The array mapping each kept element to its index in the subset.
    uint *  // Mapping from the original subset to the full set.
);

/**************/
/* Union-Find */
/**************/

/**
 * @brief
 * Second type used to represent a partition. This is the type used by
 * Union-Find.
 */
typedef struct {
    uint size_tab; //!< Size of all arrays.
    uint size_set; //!< Size of the partitionned set (the set itself is
    //!< {0,...,size_set-1}).
    uint size_par; //!< Size of the partition.
    uint *parent;  //!< Array encoding the parent relation.
    uint *rank;    //!< Array of ranks (meaningful only for roots).
    uint *sizec;   //!< Array of classes sizes (meaningful only for roots).
} ufind;

/**
 * @brief
 * Creation of a trivial union-find partition (all classes are singletons).
 *
 * @return
 * The union-find partition.
 */
ufind *create_ufind(uint //!< The size of the partitioned set.
);

/**
 * @brief
 * Release of a union-find partition.
 */
void delete_ufind(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Computes the size of the partitioned set in a union-find partition.
 *
 * @return
 * The size of the partitioned set.
 */
uint sizeset_ufind(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Computes the size of the partition in a union-find partition.
 *
 * @return
 * The size of the partition.
 */
uint sizepar_ufind(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Increments the size of the partionned set in a union-find partition.
 *
 * @remark
 * The class of the new element is a singleton.
 */
void makeset_ufind(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Computes the class number of an element in a union-find partition.
 *
 * @return
 * The class number of the element.
 */
uint find_ufind(uint,   //!< The element.
                ufind * //!< The union-find partition.
);

/**
 * @brief
 * Computes the size of the class of an element in a union-find partition.
 *
 * @return
 * The size of the class of the element.
 */
uint sizeclass_ufind(uint,   //!< The element.
                     ufind * //!< The union-find partition.
);

/**
 * @brief
 * Merging of the classes of two elements in a union-find partition.
 */
void union_ufind(uint,   //!< The first element.
                 uint,   //!< The second element.
                 ufind * //!< The union-find partition.
);

/**
 * @brief
 * Displays a union-find partition.
 */
void print_ufind(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Conversion from the partition type to the union-find type.
 *
 * @return
 * A representation of the input partition in the union-find type.
 */
ufind *parti_to_ufind(parti * //!< The partition.
);

/**
 * @brief
 * Conversion from the union-find type to the partition type.
 *
 * @return
 * A representation of the input partition in the partition type.
 */
parti *ufind_to_parti(ufind * //!< The union-find partition.
);

/**
 * @brief
 * Conversion from the union-find type to the partition type. In this case,
 * a partition that is coarser that the union-find partition is also given as
 * input.
 *
 * @return
 * A representation of the input partition in the partition type.
 */
parti *ufind_to_parti_refined(
    ufind *, //!< The union-find partition.
    parti *  //!< A partition coarser than the union-find partition.
);

#endif
