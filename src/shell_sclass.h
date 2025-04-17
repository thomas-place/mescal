/*****************************************/
/* Fonctions spécifiques sur les classes */
/*      (affichage principalement)       */
/*****************************************/

#ifndef SHELL_SCLASS_H
#define SHELL_SCLASS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "alloc.h"
#include "shell_keywords.h"

/**
 * @brief
 * Internal names for all known classes.
 */
typedef enum {
    // Ad-Hoc
    CL_HTGEN,


    // Star-free
    CL_SF,
    CL_SFMOD,
    CL_SFAMT,
    CL_SFGR,

    // Polynomial closure
    CL_PPT,
    CL_POLMOD,
    CL_POLAMT,
    CL_POLGR,
    CL_POLDD,
    CL_POLMODP,
    CL_POLAMTP,
    CL_POLGRP,
    CL_POL2ST,
    CL_POL2MOD,
    CL_POL2AMT,
    CL_POL2GR,
    CL_POL2DD,
    CL_POL2MODP,
    CL_POL2AMTP,
    CL_POL2GRP,

    // Boolean Polynomial closure
    CL_PT,
    CL_BPOLMOD,
    CL_BPOLAMT,
    CL_BPOLGR,
    CL_BPOLDD,
    CL_BPOLMODP,
    CL_BPOLAMTP,
    CL_BPOLGRP,
    CL_BPOL2ST,
    CL_BPOL2MOD,
    CL_BPOL2AMT,
    CL_BPOL2GR,
    CL_BPOL2DD,
    CL_BPOL2MODP,
    CL_BPOL2AMTP,
    CL_BPOL2GRP,

    CL_JORBMOD,
    CL_JORBAMT,
    CL_JORBDD,
    CL_JORBMODP,
    CL_JORBAMTP,
    CL_JORBGRP,
    CL_JORBAT,

    CL_KNASTAT,

    // Unambiguous Polynomial closure
    CL_UPOLDD,
    CL_UPOLMODP,
    CL_UPOLAMTP,
    CL_UPOLGRP,

    CL_UBPOL2ST,
    CL_UBPOL2MOD,
    CL_UBPOL2AMT,
    CL_UBPOL2GR,
    CL_UBPOL2DD,
    CL_UBPOL2MODP,
    CL_UBPOL2AMTP,
    CL_UBPOL2GRP,

    // UTL closure
    CL_UL,
    CL_TLMOD,
    CL_TLAMT,
    CL_TLGR,
    CL_TLDD,
    CL_TLMODP,
    CL_TLAMTP,
    CL_TLGRP,
    CL_TL2ST,
    CL_TL2MOD,
    CL_TL2AMT,
    CL_TL2GR,
    CL_TL2DD,
    CL_TL2MODP,
    CL_TL2AMTP,
    CL_TL2GRP,

    // FTL closure
    CL_FLST,
    CL_FLMOD,
    CL_FLAMT,
    CL_FLGR,
    CL_FLDD,
    CL_FLMODP,
    CL_FLAMTP,
    CL_FLGRP,
    CL_FL2ST,
    CL_FL2MOD,
    CL_FL2AMT,
    CL_FL2GR,
    CL_FL2DD,
    CL_FL2MODP,
    CL_FL2AMTP,
    CL_FL2GRP,

    // PTL closure
    CL_PLST,
    CL_PLMOD,
    CL_PLAMT,
    CL_PLGR,
    CL_PLDD,
    CL_PLMODP,
    CL_PLAMTP,
    CL_PLGRP,
    CL_PL2ST,
    CL_PL2MOD,
    CL_PL2AMT,
    CL_PL2GR,
    CL_PL2DD,
    CL_PL2MODP,
    CL_PL2AMTP,
    CL_PL2GRP,

    // Single classes
    CL_AT,
    CL_ATT,
    CL_LT,
    CL_LTT,
    CL_ST,
    CL_DD,
    CL_MOD,
    CL_MODP,
    CL_AMT,
    CL_AMTP,
    CL_GR,
    CL_GRP,
    CL_REG,

    // Number of classes
    CL_END,
} classes;

/**
 * @brief
 * Array containing the information functions for all classes.
 */
extern void (*class_infos[CL_END]) (FILE*);

/**
 * @brief
 * Array containing the classes names.
 */
extern const char* class_names[CL_END];

/*********************/
/* General functions */
/*********************/

/**
 * @brief
 * Initialization of the array of information functions.
 */
void init_class_info(void);


/**
 * @brief
 * Computes the class corresponding to a command
 *
 * @remark
 * Returns CL_END if the command does not correspod to a class.
 */
classes command_to_class(com_command*);

/**
 * @brief
 * Print information on a class
 */
void print_class_info(classes, FILE*);

/**********/
/* AD-HOC */
/**********/

void info_htgen(FILE*);

/*************/
/* Star-free */
/*************/

void info_sf(FILE*);
void info_sf_mod(FILE*);
void info_sf_amt(FILE*);
void info_sf_gr(FILE*);



/**********************/
/* Polynomial closure */
/**********************/

void info_ppt(FILE*);
void info_pol_mod(FILE*);
void info_pol_amt(FILE*);
void info_pol_gr(FILE*);
void info_pol_dd(FILE*);
void info_pol_modp(FILE*);
void info_pol_amtp(FILE*);
void info_pol_grp(FILE*);
void info_pol2_st(FILE*);
void info_pol2_mod(FILE*);
void info_pol2_amt(FILE*);
void info_pol2_gr(FILE*);
void info_pol2_dd(FILE*);
void info_pol2_modp(FILE*);
void info_pol2_amtp(FILE*);
void info_pol2_grp(FILE*);


/******************************/
/* Boolean Polynomial closure */
/******************************/

void info_pt(FILE*);
void info_bpol_mod(FILE*);
void info_bpol_amt(FILE*);
void info_bpol_gr(FILE*);
void info_bpol_dd(FILE*);
void info_bpol_modp(FILE*);
void info_bpol_amtp(FILE*);
void info_bpol_grp(FILE*);
void info_bpol2_st(FILE*);
void info_bpol2_mod(FILE*);
void info_bpol2_amt(FILE*);
void info_bpol2_gr(FILE*);
void info_bpol2_dd(FILE*);
void info_bpol2_modp(FILE*);
void info_bpol2_amtp(FILE*);
void info_bpol2_grp(FILE*);

void info_jorb_dd(FILE*);
void info_jorb_mod(FILE*);
void info_jorb_amt(FILE*);
void info_jorb_modp(FILE*);
void info_jorb_amtp(FILE*);
void info_jorb_grp(FILE*);
void info_jorb_at(FILE*);

void info_knastat(FILE*);

/**********************************/
/* Unambiguous Polynomial closure */
/**********************************/

void info_upol_dd(FILE*);
void info_upol_modp(FILE*);
void info_upol_amtp(FILE*);
void info_upol_grp(FILE*);

void info_ubpol2_st(FILE*);
void info_ubpol2_mod(FILE*);
void info_ubpol2_amt(FILE*);
void info_ubpol2_gr(FILE*);
void info_ubpol2_dd(FILE*);
void info_ubpol2_modp(FILE*);
void info_ubpol2_amtp(FILE*);
void info_ubpol2_grp(FILE*);


/***************/
/* UTL closure */
/***************/

void info_ul(FILE*);
void info_utl_mod(FILE*);
void info_utl_amt(FILE*);
void info_utl_gr(FILE*);
void info_utl_dd(FILE*);
void info_utl_modp(FILE*);
void info_utl_amtp(FILE*);
void info_utl_grp(FILE*);
void info_utl2_st(FILE*);
void info_utl2_mod(FILE*);
void info_utl2_amt(FILE*);
void info_utl2_gr(FILE*);
void info_utl2_dd(FILE*);
void info_utl2_modp(FILE*);
void info_utl2_amtp(FILE*);
void info_utl2_grp(FILE*);


/***************/
/* FTL closure */
/***************/

void info_ftl_st(FILE*);
void info_ftl_mod(FILE*);
void info_ftl_amt(FILE*);
void info_ftl_gr(FILE*);
void info_ftl_dd(FILE*);
void info_ftl_modp(FILE*);
void info_ftl_amtp(FILE*);
void info_ftl_grp(FILE*);
void info_ftl2_st(FILE*);
void info_ftl2_mod(FILE*);
void info_ftl2_amt(FILE*);
void info_ftl2_gr(FILE*);
void info_ftl2_dd(FILE*);
void info_ftl2_modp(FILE*);
void info_ftl2_amtp(FILE*);
void info_ftl2_grp(FILE*);


/***************/
/* PTL closure */
/***************/

void info_ptl_st(FILE*);
void info_ptl_mod(FILE*);
void info_ptl_amt(FILE*);
void info_ptl_gr(FILE*);
void info_ptl_dd(FILE*);
void info_ptl_modp(FILE*);
void info_ptl_amtp(FILE*);
void info_ptl_grp(FILE*);
void info_ptl2_st(FILE*);
void info_ptl2_mod(FILE*);
void info_ptl2_amt(FILE*);
void info_ptl2_gr(FILE*);
void info_ptl2_dd(FILE*);
void info_ptl2_modp(FILE*);
void info_ptl2_amtp(FILE*);
void info_ptl2_grp(FILE*);


/******************/
/* Single classes */
/******************/

void info_at(FILE*);
void info_att(FILE*);
void info_lt(FILE*);
void info_ltt(FILE*);
void info_st(FILE*);
void info_dd(FILE*);
void info_mod(FILE*);
void info_modp(FILE*);
void info_amt(FILE*);
void info_amtp(FILE*);
void info_gr(FILE*);
void info_grp(FILE*);
void info_reg(FILE*);


/*********************/
/* Affichage partagé */
/*********************/

void print_info_input(int i, FILE* out);

void print_start_comp(FILE* out, char* class);

void print_conclusion_comp(FILE* out, bool res, const char* namec);

void print_conclusion_separ(FILE* out, bool res, const char* namec);



/***********************************************/
/* Affichage des informations sur un opérateur */
/***********************************************/




// Negation hierarchies
void print_infooper_neghiera(com_keyword c, FILE* out);

// Future/past hierarchies
void print_infooper_fphiera(com_keyword c, FILE* out);

#endif
