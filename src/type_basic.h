/**
 * @file type_basic.h
 * @brief
 * Basic types.
 */

#ifndef TYPES_H
#define TYPES_H

 /*  ____            _        _____                       */
 /* | __ )  __ _ ___(_) ___  |_   _|   _ _ __   ___  ___  */
 /* |  _ \ / _` / __| |/ __|   | || | | | '_ \ / _ \/ __| */
 /* | |_) | (_| \__ \ | (__    | || |_| | |_) |  __/\__ \ */
 /* |____/ \__,_|___/_|\___|   |_| \__, | .__/ \___||___/ */
 /*                                |___/|_|               */

#include "string.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#endif
