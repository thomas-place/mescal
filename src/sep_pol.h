/**
 * @file sep_pol.h
 * @brief
 * Separation by classes built with polynomial closure.
 */

#ifndef SEP_POL_H
#define SEP_POL_H

#include <stdbool.h>
#include <stdlib.h>
#include "nfa.h"
#include "sep_group.h"
#include "printing.h"
#include "graphs_tarjan.h"
#include "nfa_intersec.h"

 /*  ____                             _   _                 ____       _                             _       _  */
 /* / ___|  ___ _ __   __ _ _ __ __ _| |_(_) ___  _ __  _  |  _ \ ___ | |_   _ _ __   ___  _ __ ___ (_) __ _| | */
 /* \___ \ / _ \ '_ \ / _` | '__/ _` | __| |/ _ \| '_ \(_) | |_) / _ \| | | | | '_ \ / _ \| '_ ` _ \| |/ _` | | */
 /*  ___) |  __/ |_) | (_| | | | (_| | |_| | (_) | | | |_  |  __/ (_) | | |_| | | | | (_) | | | | | | | (_| | | */
 /* |____/_\___| .__/ \__,_|_|  \__,_|\__|_|\___/|_| |_(_) |_|   \___/|_|\__, |_| |_|\___/|_| |_| |_|_|\__,_|_| */
 /*   ___| | __|_|___ _   _ _ __ ___                                     |___/                                  */
 /*  / __| |/ _ \/ __| | | | '__/ _ \                                                                           */
 /* | (__| | (_) \__ \ |_| | | |  __/                                                                           */
 /*  \___|_|\___/|___/\__,_|_|  \___|                                                                           */



/**
 * @brief
 * Pol(ST)-separation.
 *
 * @remark
 * A stream is given as input. Results of the computation are displayed on
 *  A NULL pointer should be given if no display is wanted. It
 * is possible to ask for details on the computation using the Boolean.
 *
 * @return
 * A Boolean indicating whether the two input languages are Pol(ST)-separable.
 */
bool decid_polst_sep(nfa*, //!< First NFA.
    nfa*, //!< Second NFA.
    bool,//!< Should further details be displayed?
    FILE*               //!< The stream.
);

/**
 * @brief
 * Pol(MOD)-separation.
 *
 * @remark
 * A stream is given as input. Results of the computation are displayed on
 *  A NULL pointer should be given if no display is wanted. It
 * is possible to ask for details on the computation using the Boolean.
 *
 * @return
 * A Boolean indicating whether the two input languages are Pol(MOD)-separable.
 */
bool decid_polmod_sep(nfa*, //!< First NFA.
    nfa*,  //!< Second NFA.
    bool,  //!< Should further details be displayed?
    FILE*                //!< The stream.
);

/**
 * @brief
 * Pol(GR)-separation.
 *
 * @remark
 * A stream is given as input. Results of the computation are displayed on
 *  A NULL pointer should be given if no display is wanted. It
 * is possible to ask for details on the computation using the Boolean.
 *
 * @return
 * A Boolean indicating whether the two input languages are Pol(GR)-separable.
 */
bool decid_polgr_sep(nfa*, //!< First NFA.
    nfa*, //!< Second NFA.
    bool, //!< Should further details be displayed?
    FILE*               //!< The stream.
);

#endif
