#include "shell_sclass.h"
#include "monoid_display.h"
#define MESSAGESIZE 200


void (*class_infos[CL_END]) (FILE*) = { NULL };
const char* class_names[CL_END] = { NULL };
/*********************/
/* General functions */
/*********************/

void init_class_info(void) {

    class_infos[CL_HTGEN] = info_htgen;
    class_names[CL_HTGEN] = "HTGEN";

    // Star-free
    class_infos[CL_SF] = info_sf;
    class_names[CL_SF] = "SF";
    class_infos[CL_SFMOD] = info_sf_mod;
    class_names[CL_SFMOD] = "SF(MOD)";
    class_infos[CL_SFAMT] = info_sf_amt;
    class_names[CL_SFAMT] = "SF(AMT)";
    class_infos[CL_SFGR] = info_sf_gr;
    class_names[CL_SFGR] = "SF(GR)";





    // Polynomial closure
    class_infos[CL_PPT] = info_ppt;
    class_infos[CL_POLMOD] = info_pol_mod;
    class_infos[CL_POLAMT] = info_pol_amt;
    class_infos[CL_POLGR] = info_pol_gr;
    class_infos[CL_POLDD] = info_pol_dd;
    class_infos[CL_POLMODP] = info_pol_modp;
    class_infos[CL_POLAMTP] = info_pol_amtp;
    class_infos[CL_POLGRP] = info_pol_grp;
    class_infos[CL_POL2ST] = info_pol2_st;
    class_infos[CL_POL2MOD] = info_pol2_mod;
    class_infos[CL_POL2AMT] = info_pol2_amt;
    class_infos[CL_POL2GR] = info_pol2_gr;
    class_infos[CL_POL2DD] = info_pol2_dd;
    class_infos[CL_POL2MODP] = info_pol2_modp;
    class_infos[CL_POL2AMTP] = info_pol2_amtp;
    class_infos[CL_POL2GRP] = info_pol2_grp;

    class_names[CL_PPT] = "PPT";
    class_names[CL_POLMOD] = "Pol(MOD)";
    class_names[CL_POLAMT] = "Pol(AMT)";
    class_names[CL_POLGR] = "Pol(GR)";
    class_names[CL_POLDD] = "Pol(DD)";
    class_names[CL_POLMODP] = "Pol(MOD⁺)";
    class_names[CL_POLAMTP] = "Pol(AMT⁺)";
    class_names[CL_POLGRP] = "Pol(GR⁺)";
    class_names[CL_POL2ST] = "Pol₂(ST)";
    class_names[CL_POL2MOD] = "Pol₂(MOD)";
    class_names[CL_POL2AMT] = "Pol₂(AMT)";
    class_names[CL_POL2GR] = "Pol₂(GR)";
    class_names[CL_POL2DD] = "Pol₂(DD)";
    class_names[CL_POL2MODP] = "Pol₂(MOD⁺)";
    class_names[CL_POL2AMTP] = "Pol₂(AMT⁺)";
    class_names[CL_POL2GRP] = "Pol₂(GR⁺)";

    // Boolean Polynomial closure
    class_infos[CL_PT] = info_pt;
    class_infos[CL_BPOLMOD] = info_bpol_mod;
    class_infos[CL_BPOLAMT] = info_bpol_amt;
    class_infos[CL_BPOLGR] = info_bpol_gr;
    class_infos[CL_BPOLDD] = info_bpol_dd;
    class_infos[CL_BPOLMODP] = info_bpol_modp;
    class_infos[CL_BPOLAMTP] = info_bpol_amtp;
    class_infos[CL_BPOLGRP] = info_bpol_grp;
    class_infos[CL_BPOL2ST] = info_bpol2_st;
    class_infos[CL_BPOL2MOD] = info_bpol2_mod;
    class_infos[CL_BPOL2AMT] = info_bpol2_amt;
    class_infos[CL_BPOL2GR] = info_bpol2_gr;
    class_infos[CL_BPOL2DD] = info_bpol2_dd;
    class_infos[CL_BPOL2MODP] = info_bpol2_modp;
    class_infos[CL_BPOL2AMTP] = info_bpol2_amtp;
    class_infos[CL_BPOL2GRP] = info_bpol2_grp;

    class_infos[CL_JORBMOD] = info_jorb_mod;
    class_infos[CL_JORBAMT] = info_jorb_amt;
    class_infos[CL_JORBDD] = info_jorb_dd;
    class_infos[CL_JORBMODP] = info_jorb_modp;
    class_infos[CL_JORBAMTP] = info_jorb_amtp;
    class_infos[CL_JORBGRP] = info_jorb_grp;
    class_infos[CL_JORBAT] = info_jorb_at;

    class_infos[CL_KNASTAT] = info_knastat;

    class_names[CL_PT] = "PT";
    class_names[CL_BPOLMOD] = "BPol(MOD)";
    class_names[CL_BPOLAMT] = "BPol(AMT)";
    class_names[CL_BPOLGR] = "BPol(GR)";
    class_names[CL_BPOLDD] = "BPol(DD)";
    class_names[CL_BPOLMODP] = "BPol(MOD⁺)";
    class_names[CL_BPOLAMTP] = "BPol(AMT⁺)";
    class_names[CL_BPOLGRP] = "BPol(GR⁺)";
    class_names[CL_BPOL2ST] = "BPol₂(ST)";
    class_names[CL_BPOL2MOD] = "BPol₂(MOD)";
    class_names[CL_BPOL2AMT] = "BPol₂(AMT)";
    class_names[CL_BPOL2GR] = "BPol₂(GR)";
    class_names[CL_BPOL2DD] = "BPol₂(DD)";
    class_names[CL_BPOL2MODP] = "BPol₂(MOD⁺)";
    class_names[CL_BPOL2AMTP] = "BPol₂(AMT⁺)";
    class_names[CL_BPOL2GRP] = "BPol₂(GR⁺)";

    class_names[CL_JORBMOD] = "JOrb(MOD)";
    class_names[CL_JORBAMT] = "JOrb(AMT)";
    class_names[CL_JORBDD] = "JOrb(DD)";
    class_names[CL_JORBMODP] = "JOrb(MOD⁺)";
    class_names[CL_JORBAMTP] = "JOrb(AMT⁺)";
    class_names[CL_JORBGRP] = "JOrb(GR⁺)";
    class_names[CL_JORBAT] = "JOrb(AT)";

    class_names[CL_KNASTAT] = "KNASTAT";

    // Unambiguous Polynomial closure
    class_infos[CL_UPOLDD] = info_upol_dd;
    class_infos[CL_UPOLMODP] = info_upol_modp;
    class_infos[CL_UPOLAMTP] = info_upol_amtp;
    class_infos[CL_UPOLGRP] = info_upol_grp;

    class_names[CL_UPOLDD] = "UPol(DD)";
    class_names[CL_UPOLMODP] = "UPol(MOD⁺)";
    class_names[CL_UPOLAMTP] = "UPol(AMT⁺)";
    class_names[CL_UPOLGRP] = "UPol(GR⁺)";

    class_infos[CL_UBPOL2ST] = info_ubpol2_st;
    class_infos[CL_UBPOL2MOD] = info_ubpol2_mod;
    class_infos[CL_UBPOL2AMT] = info_ubpol2_amt;
    class_infos[CL_UBPOL2GR] = info_ubpol2_gr;
    class_infos[CL_UBPOL2DD] = info_ubpol2_dd;
    class_infos[CL_UBPOL2MODP] = info_ubpol2_modp;
    class_infos[CL_UBPOL2AMTP] = info_ubpol2_amtp;
    class_infos[CL_UBPOL2GRP] = info_ubpol2_grp;

    class_names[CL_UBPOL2ST] = "UPol(BPol₂(ST))";
    class_names[CL_UBPOL2MOD] = "UPol(BPol₂(MOD))";
    class_names[CL_UBPOL2AMT] = "UPol(BPol₂(AMT))";
    class_names[CL_UBPOL2GR] = "UPol(BPol₂(GR))";
    class_names[CL_UBPOL2DD] = "UPol(BPol₂(DD))";
    class_names[CL_UBPOL2MODP] = "UPol(BPol₂(MOD⁺))";
    class_names[CL_UBPOL2AMTP] = "UPol(BPol₂(AMT⁺))";
    class_names[CL_UBPOL2GRP] = "UPol(BPol₂(GR⁺))";

    // UTL closure
    class_infos[CL_UL] = info_ul;
    class_infos[CL_TLMOD] = info_utl_mod;
    class_infos[CL_TLAMT] = info_utl_amt;
    class_infos[CL_TLGR] = info_utl_gr;
    class_infos[CL_TLDD] = info_utl_dd;
    class_infos[CL_TLMODP] = info_utl_modp;
    class_infos[CL_TLAMTP] = info_utl_amtp;
    class_infos[CL_TLGRP] = info_utl_grp;
    class_infos[CL_TL2ST] = info_utl2_st;
    class_infos[CL_TL2MOD] = info_utl2_mod;
    class_infos[CL_TL2AMT] = info_utl2_amt;
    class_infos[CL_TL2GR] = info_utl2_gr;
    class_infos[CL_TL2DD] = info_utl2_dd;
    class_infos[CL_TL2MODP] = info_utl2_modp;
    class_infos[CL_TL2AMTP] = info_utl2_amtp;
    class_infos[CL_TL2GRP] = info_utl2_grp;

    class_names[CL_UL] = "UL";
    class_names[CL_TLMOD] = "TL(MOD)";
    class_names[CL_TLAMT] = "TL(AMT)";
    class_names[CL_TLGR] = "TL(GR)";
    class_names[CL_TLDD] = "TL(DD)";
    class_names[CL_TLMODP] = "TL(MOD⁺)";
    class_names[CL_TLAMTP] = "TL(AMT⁺)";
    class_names[CL_TLGRP] = "TL(GR⁺)";
    class_names[CL_TL2ST] = "TL₂(ST)";
    class_names[CL_TL2MOD] = "TL₂(MOD)";
    class_names[CL_TL2AMT] = "TL₂(AMT)";
    class_names[CL_TL2GR] = "TL₂(GR)";
    class_names[CL_TL2DD] = "TL₂(DD)";
    class_names[CL_TL2MODP] = "TL₂(MOD⁺)";
    class_names[CL_TL2AMTP] = "TL₂(AMT⁺)";
    class_names[CL_TL2GRP] = "TL₂(GR⁺)";

    // FTL closure
    class_infos[CL_FLST] = info_ftl_st;
    class_infos[CL_FLMOD] = info_ftl_mod;
    class_infos[CL_FLAMT] = info_ftl_amt;
    class_infos[CL_FLGR] = info_ftl_gr;
    class_infos[CL_FLDD] = info_ftl_dd;
    class_infos[CL_FLMODP] = info_ftl_modp;
    class_infos[CL_FLAMTP] = info_ftl_amtp;
    class_infos[CL_FLGRP] = info_ftl_grp;
    class_infos[CL_FL2ST] = info_ftl2_st;
    class_infos[CL_FL2MOD] = info_ftl2_mod;
    class_infos[CL_FL2AMT] = info_ftl2_amt;
    class_infos[CL_FL2GR] = info_ftl2_gr;
    class_infos[CL_FL2DD] = info_ftl2_dd;
    class_infos[CL_FL2MODP] = info_ftl2_modp;
    class_infos[CL_FL2AMTP] = info_ftl2_amtp;
    class_infos[CL_FL2GRP] = info_ftl2_grp;

    class_names[CL_FLST] = "FL(ST)";
    class_names[CL_FLMOD] = "FL(MOD)";
    class_names[CL_FLAMT] = "FL(AMT)";
    class_names[CL_FLGR] = "FL(GR)";
    class_names[CL_FLDD] = "FL(DD)";
    class_names[CL_FLMODP] = "FL(MOD⁺)";
    class_names[CL_FLAMTP] = "FL(AMT⁺)";
    class_names[CL_FLGRP] = "FL(GR⁺)";
    class_names[CL_FL2ST] = "FL₂(ST)";
    class_names[CL_FL2MOD] = "FL₂(MOD)";
    class_names[CL_FL2AMT] = "FL₂(AMT)";
    class_names[CL_FL2GR] = "FL₂(GR)";
    class_names[CL_FL2DD] = "FL₂(DD)";
    class_names[CL_FL2MODP] = "FL₂(MOD⁺)";
    class_names[CL_FL2AMTP] = "FL₂(AMT⁺)";
    class_names[CL_FL2GRP] = "FL₂(GR⁺)";

    // PTL closure
    class_infos[CL_PLST] = info_ptl_st;
    class_infos[CL_PLMOD] = info_ptl_mod;
    class_infos[CL_PLAMT] = info_ptl_amt;
    class_infos[CL_PLGR] = info_ptl_gr;
    class_infos[CL_PLDD] = info_ptl_dd;
    class_infos[CL_PLMODP] = info_ptl_modp;
    class_infos[CL_PLAMTP] = info_ptl_amtp;
    class_infos[CL_PLGRP] = info_ptl_grp;
    class_infos[CL_PL2ST] = info_ptl2_st;
    class_infos[CL_PL2MOD] = info_ptl2_mod;
    class_infos[CL_PL2AMT] = info_ptl2_amt;
    class_infos[CL_PL2GR] = info_ptl2_gr;
    class_infos[CL_PL2DD] = info_ptl2_dd;
    class_infos[CL_PL2MODP] = info_ptl2_modp;
    class_infos[CL_PL2AMTP] = info_ptl2_amtp;
    class_infos[CL_PL2GRP] = info_ptl2_grp;

    class_names[CL_PLST] = "PL(ST)";
    class_names[CL_PLMOD] = "PL(MOD)";
    class_names[CL_PLAMT] = "PL(AMT)";
    class_names[CL_PLGR] = "PL(GR)";
    class_names[CL_PLDD] = "PL(DD)";
    class_names[CL_PLMODP] = "PL(MOD⁺)";
    class_names[CL_PLAMTP] = "PL(AMT⁺)";
    class_names[CL_PLGRP] = "PL(GR⁺)";
    class_names[CL_PL2ST] = "PL₂(ST)";
    class_names[CL_PL2MOD] = "PL₂(MOD)";
    class_names[CL_PL2AMT] = "PL₂(AMT)";
    class_names[CL_PL2GR] = "PL₂(GR)";
    class_names[CL_PL2DD] = "PL₂(DD)";
    class_names[CL_PL2MODP] = "PL₂(MOD⁺)";
    class_names[CL_PL2AMTP] = "PL₂(AMT⁺)";
    class_names[CL_PL2GRP] = "PL₂(GR⁺)";

    // Single classes
    class_infos[CL_AT] = info_at;
    class_infos[CL_ATT] = info_att;
    class_infos[CL_LT] = info_lt;
    class_infos[CL_LTT] = info_ltt;
    class_infos[CL_ST] = info_st;
    class_infos[CL_DD] = info_dd;
    class_infos[CL_MOD] = info_mod;
    class_infos[CL_MODP] = info_modp;
    class_infos[CL_AMT] = info_amt;
    class_infos[CL_AMTP] = info_amtp;
    class_infos[CL_GR] = info_gr;
    class_infos[CL_GRP] = info_grp;
    class_infos[CL_REG] = info_reg;

    class_names[CL_AT] = "AT";
    class_names[CL_ATT] = "ATT";
    class_names[CL_LT] = "LT";
    class_names[CL_LTT] = "LTT";
    class_names[CL_ST] = "ST";
    class_names[CL_DD] = "DD";
    class_names[CL_MOD] = "MOD";
    class_names[CL_MODP] = "MOD⁺";
    class_names[CL_AMT] = "AMT";
    class_names[CL_AMTP] = "AMT⁺";
    class_names[CL_GR] = "GR";
    class_names[CL_GRP] = "GR⁺";
    class_names[CL_REG] = "REG";

}

typedef struct keylist {
    com_keyword key;
    struct keylist* next;
}keylist;

static keylist* make_keylist_class(com_command* thecom) {
    if (!thecom || !thecom->main || thecom->thetype != CMT_KEY || com_nbparams(thecom->params) > 1) {
        return NULL;
    }

    com_keyword key = key_from_string_chain_single(thecom->main);
    keylist* ret;
    if (com_nbparams(thecom->params) == 0) {
        switch (key)
        {
        case KY_HTGEN:
        case KY_AT:
        case KY_ATT:
        case KY_SF:
        case KY_UL:
        case KY_TLC:
        case KY_TLX:
        case KY_PPT:
        case KY_PT:
        case KY_LT:
        case KY_LTT:
        case KY_ST:
        case KY_DD:
        case KY_MOD:
        case KY_MODP:
        case KY_AMT:
        case KY_AMTP:
        case KY_GR:
        case KY_GRP:
        case KY_REG:
        case KY_KNASTAT:
            MALLOC(ret, 1);
            ret->key = key;
            ret->next = NULL;
            return ret;
            break;
        default:
            return NULL;
            break;
        }
    }
    else {
        keylist* sub = make_keylist_class(thecom->params->param);
        if (!sub) {
            return NULL;
        }
        switch (key)
        {
        case KY_JORB:
        case KY_SF:
        case KY_POL:
        case KY_BPOL:
        case KY_UPOL:
        case KY_TLC:
        case KY_FLC:
        case KY_PLC:
            MALLOC(ret, 1);
            ret->key = key;
            ret->next = sub;
            return ret;
            break;
        case KY_POL2:
            MALLOC(ret, 1);
            ret->key = KY_POL;
            MALLOC(ret->next, 1);
            ret->next->key = KY_BPOL;
            ret->next->next = sub;
            return ret;
        case KY_BPOL2:
            MALLOC(ret, 1);
            ret->key = KY_BPOL;
            MALLOC(ret->next, 1);
            ret->next->key = KY_BPOL;
            ret->next->next = sub;
            return ret;
        case KY_UBPOL:
            MALLOC(ret, 1);
            ret->key = KY_UPOL;
            MALLOC(ret->next, 1);
            ret->next->key = KY_BPOL;
            ret->next->next = sub;
            return ret;
        case KY_UBPOL2:
            MALLOC(ret, 1);
            ret->key = KY_UPOL;
            MALLOC(ret->next, 1);
            ret->next->key = KY_BPOL;
            MALLOC(ret->next->next, 1);
            ret->next->next->key = KY_BPOL;
            ret->next->next->next = sub;
            return ret;
        case KY_TLC2:
            MALLOC(ret, 1);
            ret->key = KY_TLC;
            MALLOC(ret->next, 1);
            ret->next->key = KY_TLC;
            ret->next->next = sub;
            return ret;
        case KY_FLC2:
            MALLOC(ret, 1);
            ret->key = KY_FLC;
            MALLOC(ret->next, 1);
            ret->next->key = KY_FLC;
            ret->next->next = sub;
            return ret;
        case KY_PLC2:
            MALLOC(ret, 1);
            ret->key = KY_FLC;
            MALLOC(ret->next, 1);
            ret->next->key = KY_FLC;
            ret->next->next = sub;
            return ret;
        default:
            return NULL;
            break;
        }


    }
    return NULL;
}

static void del_keylist(keylist* list) {
    if (!list) {
        return;
    }
    del_keylist(list->next);
    free(list);
}

static classes command_to_class_aux(keylist* list) {
    if (!list) {
        return CL_END;
    }

    if (!list->next) {
        switch (list->key)
        {
        case KY_HTGEN:
            return CL_HTGEN;
            break;
        case KY_AT:
            return CL_AT;
            break;
        case KY_ATT:
            return CL_ATT;
            break;
        case KY_SF:
            return CL_SF;
            break;
        case KY_UL:
            return CL_UL;
            break;
        case KY_TLC:
            return CL_UL;
            break;
        case KY_TLX:
            return CL_TLDD;
            break;
        case KY_PPT:
            return CL_PPT;
            break;
        case KY_PT:
            return CL_PT;
            break;
        case KY_LT:
            return CL_LT;
            break;
        case KY_LTT:
            return CL_LTT;
            break;
        case KY_ST:
            return CL_ST;
            break;
        case KY_DD:
            return CL_DD;
            break;
        case KY_MOD:
            return CL_MOD;
            break;
        case KY_MODP:
            return CL_MODP;
            break;
        case KY_AMT:
            return CL_AMT;
            break;
        case KY_AMTP:
            return CL_AMTP;
            break;
        case KY_GR:
            return CL_GR;
            break;
        case KY_GRP:
            return CL_GRP;
            break;
        case KY_REG:
            return CL_REG;
            break;
        case KY_KNASTAT:
            return CL_KNASTAT;
            break;
        default:
            return CL_END;
            break;
        }
    }






    classes subcl = command_to_class_aux(list->next);
    if (subcl == CL_END) {
        return CL_END;
    }

    if (subcl == CL_REG) {
        return CL_REG;
    }

    if (list->key == KY_SF) {
        switch (subcl)
        {
        case CL_AT:
        case CL_ATT:
        case CL_LT:
        case CL_LTT:
        case CL_ST:
        case CL_DD:
        case CL_PPT:
        case CL_UPOLDD:
        case CL_POLDD:
        case CL_POL2ST:
        case CL_POL2DD:
        case CL_PT:
        case CL_BPOLDD:
        case CL_BPOL2ST:
        case CL_BPOL2DD:
        case CL_UBPOL2ST:
        case CL_UBPOL2DD:
        case CL_UL:
        case CL_TLDD:
        case CL_TL2ST:
        case CL_TL2DD:
        case CL_FLST:
        case CL_FLDD:
        case CL_FL2ST:
        case CL_FL2DD:
        case CL_PLST:
        case CL_PLDD:
        case CL_PL2ST:
        case CL_PL2DD:
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_MODP:
        case CL_POLMOD:
        case CL_UPOLMODP:
        case CL_POLMODP:
        case CL_POL2MOD:
        case CL_POL2MODP:
        case CL_BPOLMOD:
        case CL_BPOLMODP:
        case CL_BPOL2MOD:
        case CL_BPOL2MODP:
        case CL_UBPOL2MOD:
        case CL_UBPOL2MODP:
        case CL_TLMOD:
        case CL_TLMODP:
        case CL_TL2MOD:
        case CL_TL2MODP:
        case CL_FLMOD:
        case CL_FLMODP:
        case CL_FL2MOD:
        case CL_FL2MODP:
        case CL_PLMOD:
        case CL_PLMODP:
        case CL_PL2MOD:
        case CL_PL2MODP:
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMT:
        case CL_POLAMTP:
        case CL_POL2AMT:
        case CL_POL2AMTP:
        case CL_BPOLAMT:
        case CL_BPOLAMTP:
        case CL_BPOL2AMT:
        case CL_BPOL2AMTP:
        case CL_UBPOL2AMT:
        case CL_UBPOL2AMTP:
        case CL_TLAMT:
        case CL_TLAMTP:
        case CL_TL2AMT:
        case CL_TL2AMTP:
        case CL_FLAMT:
        case CL_FLAMTP:
        case CL_FL2AMT:
        case CL_FL2AMTP:
        case CL_PLAMT:
        case CL_PLAMTP:
        case CL_PL2AMT:
        case CL_PL2AMTP:
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_GRP:
        case CL_POLGR:
        case CL_UPOLGRP:
        case CL_POLGRP:
        case CL_POL2GR:
        case CL_POL2GRP:
        case CL_BPOLGR:
        case CL_BPOLGRP:
        case CL_BPOL2GR:
        case CL_BPOL2GRP:
        case CL_UBPOL2GR:
        case CL_UBPOL2GRP:
        case CL_TLGR:
        case CL_TLGRP:
        case CL_TL2GR:
        case CL_TL2GRP:
        case CL_FLGR:
        case CL_FLGRP:
        case CL_FL2GR:
        case CL_FL2GRP:
        case CL_PLGR:
        case CL_PLGRP:
        case CL_PL2GR:
        case CL_PL2GRP:
        case CL_SFGR:
            return CL_SFGR;
            break;
        default:
            return CL_END;
            break;
        }

    }

    if (list->key == KY_POL) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_PPT;
            break;
        case CL_DD:
        case CL_UPOLDD:
        case CL_POLDD:
            return CL_POLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_POL2ST:
        case CL_UL:
        case CL_FLST:
        case CL_PLST:
            return CL_POL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_POL2DD:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_POL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_POLMOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
        case CL_POLMODP:
            return CL_POLMODP;
            break;
        case CL_POL2MOD:
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_POL2MOD;
            break;
        case CL_POL2MODP:
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_POL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_POLAMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMTP:
            return CL_POLAMTP;
            break;
        case CL_POL2AMT:
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_POL2AMT;
            break;
        case CL_POL2AMTP:
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_POL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_POLGR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
        case CL_POLGRP:
            return CL_POLGRP;
            break;
        case CL_POL2GR:
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_POL2GR;
            break;
        case CL_POL2GRP:
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_POL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }



    if (list->key == KY_BPOL) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_PT;
            break;
        case CL_DD:
        case CL_UPOLDD:
        case CL_POLDD:
            return CL_BPOLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_UL:
        case CL_POL2ST:
        case CL_FLST:
        case CL_PLST:
            return CL_BPOL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_POL2DD:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_BPOL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_BPOLMOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
        case CL_POLMODP:
            return CL_BPOLMODP;
            break;
        case CL_POL2MOD:
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_BPOL2MOD;
            break;
        case CL_POL2MODP:
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_BPOL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_BPOLAMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMTP:
            return CL_BPOLAMTP;
            break;
        case CL_POL2AMT:
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_BPOL2AMT;
            break;
        case CL_POL2AMTP:
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_BPOL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_BPOLGR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
        case CL_POLGRP:
            return CL_BPOLGRP;
            break;
        case CL_POL2GR:
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_BPOL2GR;
            break;
        case CL_POL2GRP:
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_BPOL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }




    if (list->key == KY_UPOL) {
        switch (subcl)
        {
        case CL_ST:
            return CL_ST;
            break;
        case CL_DD:
        case CL_UPOLDD:
            return CL_UPOLDD;
            break;
        case CL_PPT:
            return CL_PPT;
            break;
        case CL_POLDD:
            return CL_POLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_UL:
        case CL_FLST:
        case CL_PLST:
            return CL_UL;
            break;
        case CL_POL2ST:
            return CL_POL2ST;
            break;
        case CL_BPOL2ST:
        case CL_UBPOL2ST:
            return CL_UBPOL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_TLDD;
            break;
        case CL_POL2DD:
            return CL_POL2DD;
        case CL_BPOL2DD:
        case CL_UBPOL2DD:
            return CL_UBPOL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
            return CL_MOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
            return CL_UPOLMODP;
            break;
        case CL_POLMOD:
            return CL_POLMOD;
            break;
        case CL_POLMODP:
            return CL_POLMODP;
            break;
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_TLMOD;
            break;
        case CL_POL2MOD:
            return CL_POL2MOD;
            break;
        case CL_BPOL2MOD:
        case CL_UBPOL2MOD:
            return CL_UBPOL2MOD;
            break;
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_TLMODP;
            break;
        case CL_POL2MODP:
            return CL_POL2MODP;
            break;
        case CL_BPOL2MODP:
        case CL_UBPOL2MODP:
            return CL_UBPOL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
            return CL_AMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
            return CL_UPOLAMTP;
            break;
        case CL_POLAMT:
            return CL_POLAMT;
            break;
        case CL_POLAMTP:
            return CL_POLAMTP;
            break;
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_TLAMT;
            break;
        case CL_POL2AMT:
            return CL_POL2AMT;
            break;
        case CL_BPOL2AMT:
        case CL_UBPOL2AMT:
            return CL_UBPOL2AMT;
            break;
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_TLAMTP;
            break;
        case CL_POL2AMTP:
            return CL_POL2AMTP;
            break;
        case CL_BPOL2AMTP:
        case CL_UBPOL2AMTP:
            return CL_UBPOL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
            return CL_GR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
            return CL_UPOLGRP;
            break;
        case CL_POLGR:
            return CL_POLGR;
            break;
        case CL_POLGRP:
            return CL_POLGRP;
            break;
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_TLGR;
            break;
        case CL_POL2GR:
            return CL_POL2GR;
            break;
        case CL_BPOL2GR:
        case CL_UBPOL2GR:
            return CL_UBPOL2GR;
            break;
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_TLGRP;
            break;

        case CL_POL2GRP:
            return CL_POL2GRP;
            break;
        case CL_BPOL2GRP:
        case CL_UBPOL2GRP:
            return CL_UBPOL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }



    if (list->key == KY_TLC) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_UL;
            break;
        case CL_DD:
        case CL_UPOLDD:
        case CL_POLDD:
            return CL_TLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_UL:
        case CL_POL2ST:
        case CL_FLST:
        case CL_PLST:
            return CL_TL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_POL2DD:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_TL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_TLMOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
        case CL_POLMODP:
            return CL_TLMODP;
            break;
        case CL_POL2MOD:
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_TL2MOD;
            break;
        case CL_POL2MODP:
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_TL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_TLAMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMTP:
            return CL_TLAMTP;
            break;
        case CL_POL2AMT:
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_TL2AMT;
            break;
        case CL_POL2AMTP:
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_TL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_TLGR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
        case CL_POLGRP:
            return CL_TLGRP;
            break;
        case CL_POL2GR:
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_TL2GR;
            break;
        case CL_POL2GRP:
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_TL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }





    if (list->key == KY_FLC) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_FLST;
            break;
        case CL_DD:
        case CL_UPOLDD:
        case CL_POLDD:
            return CL_FLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_UL:
        case CL_POL2ST:
        case CL_FLST:
        case CL_PLST:
            return CL_FL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_POL2DD:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_FL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_FLMOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
        case CL_POLMODP:
            return CL_FLMODP;
            break;
        case CL_POL2MOD:
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_FL2MOD;
            break;
        case CL_POL2MODP:
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_FL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_FLAMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMTP:
            return CL_FLAMTP;
            break;
        case CL_POL2AMT:
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_FL2AMT;
            break;
        case CL_POL2AMTP:
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_FL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_FLGR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
        case CL_POLGRP:
            return CL_FLGRP;
            break;
        case CL_POL2GR:
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_FL2GR;
            break;
        case CL_POL2GRP:
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_FL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }


    if (list->key == KY_PLC) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_PLST;
            break;
        case CL_DD:
        case CL_UPOLDD:
        case CL_POLDD:
            return CL_PLDD;
            break;
        case CL_AT:
        case CL_ATT:
        case CL_PT:
        case CL_UL:
        case CL_POL2ST:
        case CL_FLST:
        case CL_PLST:
            return CL_PL2ST;
            break;
        case CL_LT:
        case CL_LTT:
        case CL_POL2DD:
        case CL_TLDD:
        case CL_FLDD:
        case CL_PLDD:
            return CL_PL2DD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_PLMOD;
            break;
        case CL_MODP:
        case CL_UPOLMODP:
        case CL_POLMODP:
            return CL_PLMODP;
            break;
        case CL_POL2MOD:
        case CL_BPOLMOD:
        case CL_TLMOD:
        case CL_FLMOD:
        case CL_PLMOD:
            return CL_PL2MOD;
            break;
        case CL_POL2MODP:
        case CL_BPOLMODP:
        case CL_TLMODP:
        case CL_FLMODP:
        case CL_PLMODP:
            return CL_PL2MODP;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_PLAMT;
            break;
        case CL_AMTP:
        case CL_UPOLAMTP:
        case CL_POLAMTP:
            return CL_PLAMTP;
            break;
        case CL_POL2AMT:
        case CL_BPOLAMT:
        case CL_TLAMT:
        case CL_FLAMT:
        case CL_PLAMT:
            return CL_PL2AMT;
            break;
        case CL_POL2AMTP:
        case CL_BPOLAMTP:
        case CL_TLAMTP:
        case CL_FLAMTP:
        case CL_PLAMTP:
            return CL_PL2AMTP;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_PLGR;
            break;
        case CL_GRP:
        case CL_UPOLGRP:
        case CL_POLGRP:
            return CL_PLGRP;
            break;
        case CL_POL2GR:
        case CL_BPOLGR:
        case CL_TLGR:
        case CL_FLGR:
        case CL_PLGR:
            return CL_PL2GR;
            break;
        case CL_POL2GRP:
        case CL_BPOLGRP:
        case CL_TLGRP:
        case CL_FLGRP:
        case CL_PLGRP:
            return CL_PL2GRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;

        default:
            return CL_END;
            break;
        }

    }


    if (list->key == KY_JORB) {
        switch (subcl)
        {
        case CL_ST:
        case CL_PPT:
            return CL_PT;
            break;
        case CL_DD:
        case CL_POLDD:
            return CL_JORBDD;
            break;
        case CL_SF:
            return CL_SF;
            break;
        case CL_MOD:
        case CL_POLMOD:
            return CL_JORBMOD;
            break;
        case CL_SFMOD:
            return CL_SFMOD;
            break;
        case CL_AMT:
        case CL_POLAMT:
            return CL_JORBAMT;
            break;
        case CL_SFAMT:
            return CL_SFAMT;
            break;
        case CL_AMTP:
        case CL_POLAMTP:
            return CL_JORBAMTP;
            break;
        case CL_GR:
        case CL_POLGR:
            return CL_BPOLGR;
            break;
        case CL_GRP:
        case CL_POLGRP:
            return CL_JORBGRP;
            break;
        case CL_SFGR:
            return CL_SFGR;
            break;
        case CL_AT:
        case CL_PT:
            return CL_JORBAT;
            break;
        default:
            return CL_END;
            break;
        }

    }


    return CL_END;


}


classes command_to_class(com_command* thecom) {

    keylist* list = make_keylist_class(thecom);
    classes ret = command_to_class_aux(list);
    del_keylist(list);
    return ret;
}


void print_class_info(classes class, FILE* out) {
    if (class_infos[class]) {
        class_infos[class](out);
        print_dbot_line(100, out);
    }
}


void info_htgen(FILE* out) {

    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism where 1 and the generators have trivial H-classes : HTGEN.");

}

/*************/
/* Star-free */
/*************/



void info_sf(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Star-free languages : SF.");
    print_dline_box(0, out, " Definition: Languages defined by a star-free expression.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with the linear order (FO(<)).");
    print_dline_box(0, out, "  - Linear temporal logic (LTL).");
    print_dline_box(0, out, "  - Languages with an aperiodic syntactic monoid.");
    print_dline_box(0, out, "  - Languages with a counter-free minimal automaton.");

}

void info_sf_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Star-free closure of MOD : SF(MOD).");
    print_dline_box(0, out, " Definition: Least class containing MOD closed under concatenation and Boolean operations.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with the linear order and modular predicates (FO(<,MOD)).");
    print_dline_box(0, out, "  - Linear temporal logic with modular modalities (LTL(MOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel is aperiodic.");
    print_dline_box(0, out, "  - Languages with a minimal automaton whose MOD-kernel is counter-free.");

}

void info_sf_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Star-free closure of AMT : SF(AMT).");
    print_dline_box(0, out, " Definition: Least class containing AMT closed under concatenation and Boolean operations.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with the linear order and alphabetic modular predicates (FO(<,AMOD)).");
    print_dline_box(0, out, "  - Linear temporal logic with alphabetic modular modalities (LTL(AMOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT-kernel is aperiodic.");
    print_dline_box(0, out, "  - Languages with a minimal automaton whose AMT-kernel is counter-free.");

}

void info_sf_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Star-free closure of GR : SF(GR).");
    print_dline_box(0, out, " Definition: Least class containing GR closed under concatenation and Boolean operations.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with the linear order and group predicates (FO(<,GR)).");
    print_dline_box(0, out, "  - Linear temporal logic with group modalities (LTL(GR)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR-kernel is aperiodic.");
    print_dline_box(0, out, "  - Languages with a minimal automaton whose GR-kernel is counter-free.");

}



/**********************/
/* Polynomial closure */
/**********************/

void info_ppt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Positive piecewise testable languages : PPT.");
    print_dline_box(0, out, " Definition : Contains all finite unions of ST-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the Straubing-Thérien hierarchy (Pol(ST)).");
    print_dline_box(0, out, "  - Level Σ₁(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Languages with an syntactic ordered monoid M satisfying 1 ≤ s for all s ∊ M.");

}
void info_pol_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of MOD : Pol(MOD).");
    print_dline_box(0, out, " Definition : Contains all finite unions of MOD-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the modulo hierarchy (Pol(MOD)).");
    print_dline_box(0, out, "  - Level Σ₁(<,MOD) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel satisfies 1 ≤ s for all s.");

}
void info_pol_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of AMT : Pol(AMT).");
    print_dline_box(0, out, " Definition : Contains all finite unions of AMT-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the alphabetic modulo hierarchy (Pol(AMT)).");
    print_dline_box(0, out, "  - Level Σ₁(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT-kernel satisfies 1 ≤ s for all s.");

}
void info_pol_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of GR : Pol(GR).");
    print_dline_box(0, out, " Definition : Contains all finite unions of GR-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the group hierarchy (Pol(GR)).");
    print_dline_box(0, out, "  - Level Σ₁(<,GR) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic monoid M which satisfies 1 ≤ e for all e ∊ E(M).");

}
void info_pol_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of DD : Pol(DD).");
    print_dline_box(0, out, " Definition : Contains all finite unions of DD-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the dot-depth hierarchy (Pol(DD)).");
    print_dline_box(0, out, "  - Level Σ₁(<,+1) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    successor.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits satisfy 1 ≤ s for all s.");

}
void info_pol_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of MOD⁺ : Pol(MOD⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of MOD⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the concatenation hierarchy of basis MOD⁺ (Pol(MOD⁺)).");
    print_dline_box(0, out, "  - Level Σ₁(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD⁺-orbits satisfy 1 ≤ s for all s.");

}
void info_pol_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of AMT⁺ : Pol(AMT⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of AMT⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the concatenation hierarchy of basis AMT⁺ (Pol(AMT⁺)).");
    print_dline_box(0, out, "  - Level Σ₁(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT⁺-orbits satisfy 1 ≤ s for all s.");

}
void info_pol_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of GR⁺ : Pol(GR⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of GR⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 1/2 in the concatenation hierarchy of basis GR⁺ (Pol(GR⁺)).");
    print_dline_box(0, out, "  - Level Σ₁(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR⁺-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(ST) : Pol₂(ST).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(ST)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the Straubing-Thérien hierarchy (Pol₂(ST)).");
    print_dline_box(0, out, "  - Level 1/2 in the concatenation hierarchy of basis AT (Pol(AT)).");
    print_dline_box(0, out, "  - Level Σ₂(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Level Σ₁(AT) in the alternation hierarchy of first-order logic with alphabetic predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(ST)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(MOD) : Pol₂(MOD).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(MOD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the modulo hierarchy (Pol₂(MOD)).");
    print_dline_box(0, out, "  - Level Σ₂(<,MOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(AMT) : Pol₂(AMT).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(AMT)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the alphabetic modulo hierarchy (Pol₂(AMT)).");
    print_dline_box(0, out, "  - Level Σ₂(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(GR) : Pol₂(GR).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(GR)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the group hierarchy (Pol₂(GR)).");
    print_dline_box(0, out, "  - Level Σ₂(<,GR) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(DD) : Pol₂(DD).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(DD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the dot-depth hierarchy (Pol₂(DD)).");
    print_dline_box(0, out, "  - Level Σ₂(<,+1) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the successor.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(DD)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(MOD⁺) : Pol₂(MOD⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(MOD⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the concatenation hierarchy of basis MOD⁺ (Pol₂(MOD⁺)).");
    print_dline_box(0, out, "  - Level Σ₂(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(AMT⁺) : Pol₂(AMT⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(AMT⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the concatenation hierarchy of basis AMT⁺ (Pol₂(AMT⁺)).");
    print_dline_box(0, out, "  - Level Σ₂(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT⁺)-orbits satisfy 1 ≤ s for all s.");

}
void info_pol2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Polynomial closure of BPol(GR⁺) : Pol₂(GR⁺).");
    print_dline_box(0, out, " Definition : Contains all finite unions of BPol(GR⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level 3/2 in the concatenation hierarchy of basis GR⁺ (Pol₂(GR⁺)).");
    print_dline_box(0, out, "  - Level Σ₂(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR⁺)-orbits satisfy 1 ≤ s for all s.");

}


/******************************/
/* Boolean Polynomial closure */
/******************************/


void info_pt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Piecewise testable languages : PT.");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of ST-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the Straubing-Thérien hierarchy (BPol(ST)).");
    print_dline_box(0, out, "  - Level BΣ₁(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Languages with a J-trivial syntactic monoid.");

}
void info_bpol_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of MOD : BPol(MOD).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of MOD-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the modulo hierarchy (BPol(MOD)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,MOD) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ for all q,r,s,t");
    print_dline_box(0, out, "    such that (q,s) is a MOD-pair.");

}

void info_bpol_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of AMT : BPol(AMT).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of AMT-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the alphabetic modulo hierarchy (BPol(AMT)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies (qr)ʷst(st)ʷ = (qr)ʷqt(st)ʷ for all q,r,s,t");
    print_dline_box(0, out, "    such that (q,s) is an AMT-pair.");

}
void info_bpol_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of GR : BPol(GR).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of GR-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the group hierarchy (BPol(GR)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,GR) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic monoid which is a block group.");

}
void info_bpol_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of DD : BPol(DD).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of DD-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the dot-depth hierarchy (BPol(DD)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,+1) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    successor.");
    print_dline_box(0, out, "  - Languages with a syntactic semigroup S which satisfies Knast's equation: for every q,r,s,t ∊ S");
    print_dline_box(0, out, "    and every e,f ∊ E(S), we have (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ.");

}
void info_bpol_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of MOD⁺ : BPol(MOD⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of MOD⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the concatenation hierarchy of basis MOD⁺ (BPol(MOD⁺)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose strict MOD-kernel S satisfies Knast's equation:");
    print_dline_box(0, out, "    for all q,r,s,t ∊ S and all e,f ∊ E(S), we have (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ.");

}
void info_bpol_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of AMT⁺ : BPol(AMT⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of AMT⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the concatenation hierarchy of basis AMT⁺ (BPol(AMT⁺)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies (eqfre)ʷsft(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ");
    print_dline_box(0, out, "    for all q,r,s,t and all strict idempotents e,f such (q,s) is an AMT-pair.");

}
void info_bpol_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of GR⁺ : BPol(GR⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of GR⁺-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the concatenation hierarchy of basis GR⁺ (BPol(GR⁺)).");
    print_dline_box(0, out, "  - Level BΣ₁(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order, the");
    print_dline_box(0, out, "    successor and the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies (eqfre)ʷsft(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ");
    print_dline_box(0, out, "    for all q,r,s,t and all strict idempotents e,f such (q,s) is a GR-pair.");

}
void info_bpol2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(ST) : BPol₂(ST).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(ST)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the Straubing-Thérien hierarchy (BPol₂(ST)).");
    print_dline_box(0, out, "  - Level one in the concatenation hierarchy of basis AT (BPol(AT)).");
    print_dline_box(0, out, "  - Level BΣ₂(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Level BΣ₁(AT) in the alternation hierarchy of first-order logic with alphabetic predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(ST)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(ST)-pair.");

}
void info_bpol2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(MOD) : BPol₂(MOD).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(MOD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the modulo hierarchy (BPol₂(MOD)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,MOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(MOD)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(MOD)-pair.");

}
void info_bpol2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(AMT) : BPol₂(AMT).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(AMT)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the alphabetic modulo hierarchy (BPol₂(AMT)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(AMT)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(AMT)-pair.");

}
void info_bpol2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(GR) : BPol₂(GR).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(GR)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the group hierarchy (BPol₂(GR)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,GR) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(GR)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(GR)-pair.");

}
void info_bpol2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(DD) : BPol₂(DD).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(DD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the dot-depth hierarchy (BPol₂(DD)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,+1) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the successor.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(DD)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(DD)-pair.");

}
void info_bpol2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(MOD⁺) : BPol₂(MOD⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(MOD⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the concatenation hierarchy of basis MOD⁺ (BPol₂(MOD⁺)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(MOD⁺)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(MOD⁺)-pair.");

}
void info_bpol2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(AMT⁺) : BPol₂(AMT⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(AMT⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the concatenation hierarchy of basis AMT⁺ (BPol₂(AMT⁺)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(AMT⁺)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(AMT⁺)-pair.");

}
void info_bpol2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Boolean polynomial closure of BPol(GR⁺) : BPol₂(GR⁺).");
    print_dline_box(0, out, " Definition : Contains all finite Boolean combinations of BPol(GR⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level two in the concatenation hierarchy of basis GR⁺ (BPol₂(GR⁺)).");
    print_dline_box(0, out, "  - Level BΣ₂(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism which satisfies the two following equations:");
    print_dline_box(0, out, "     • (eqfre)ʷ(esfte)ʷ = (eqfre)ʷqft(esfte)ʷ for all q,r,s,t ∊ M and e,f ∊ E(M) such that {q,e,f},");
    print_dline_box(0, out, "       {r,e,f}, {s,e,f} and {t,e,f} are BPol(GR⁺)-sets.");
    print_dline_box(0, out, "     • (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M and e ∊ E(M) where (e,s) is a BPol(GR⁺)-pair.");

}

void info_jorb_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose DD-orbits are J-trivial : JORB(DD).");
}

void info_jorb_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose MOD-kernel is J-trivial : JORB(MOD).");
}

void info_jorb_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose AMT-kernel is J-trivial : JORB(AMT).");
}

void info_jorb_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose MOD⁺-orbits are J-trivial : JORB(MOD⁺).");
}

void info_jorb_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose AMT⁺-orbits are J-trivial : JORB(AMT⁺).");
}

void info_jorb_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose GR⁺-orbits are J-trivial : JORB(GR).");
}

void info_jorb_at(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism whose AT-orbits are J-trivial : JORB(AT).");
}

void info_knastat(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Languages with a syntactic morphism satisfying that AT-variant of Knast's equation : KNASTAT.");
}


/**********************************/
/* Unambiguous Polynomial closure */
/**********************************/

void info_upol_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of DD : UPol(DD).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous DD-monomials.");
    print_dline_box(0, out, " Characterizations : ");
    print_dline_box(0, out, "  - Least Boolean agebra containing the languages wA* and A*w where w is a word.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits are trivial.");
}

void info_upol_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of MOD⁺ : UPol(MOD⁺).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous MOD⁺-monomials.");
    print_dline_box(0, out, "  - Least Boolean agebra containing MOD and the languages wA* and A*w where w is a word.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD⁺-orbits are trivial.");
}

void info_upol_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of AMT⁺ : UPol(AMT⁺).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous AMT⁺-monomials.");
    print_dline_box(0, out, "  - Least Boolean agebra containing AMT and the languages wA* and A*w where w is a word.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT⁺-orbits are trivial.");
}

void info_upol_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of GR⁺ : UPol(GR⁺).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous GR⁺-monomials.");
    print_dline_box(0, out, "  - Least Boolean agebra containing GR and the languages wA* and A*w where w is a word.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR⁺-orbits are trivial.");
}

void info_ubpol2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(ST) : UPol(BPol₂(ST)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(ST)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Level Δ₂(AT) in the alternation hierarchy of first-order logic with alphabetic predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(ST)-pair.");

}
void info_ubpol2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(MOD) : UPol(BPol₂(MOD)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(MOD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,MOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(MOD)-pair.");

}
void info_ubpol2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(AMT) : UPol(BPol₂(AMT)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(AMT)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(AMT)-pair.");

}
void info_ubpol2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(GR) : UPol(BPol₂(GR)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(GR)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,GR) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(GR)-pair.");

}
void info_ubpol2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(DD) : UPol(BPol₂(DD)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(DD)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,+1) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the successor.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(DD)-pair.");

}
void info_ubpol2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(MOD⁺) : UPol(BPol₂(MOD⁺)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(MOD⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(MOD⁺)-pair.");

}
void info_ubpol2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(AMT⁺) : UPol(BPol₂(AMT⁺)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(AMT⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(AMT⁺)-pair.");

}
void info_ubpol2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous polynomial closure of BPol₂(GR⁺) : UPol(BPol₂(GR⁺)).");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous BPol₂(GR⁺)-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level Δ₃(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism satisfying (esete)ʷ⁺¹ = (esete)ʷt(esete)ʷ for all s,t ∊ M");
    print_dline_box(0, out, "    and e ∊ E(M) where (e,s) is a BPol(GR⁺)-pair.");

}


/***************/
/* UTL closure */
/***************/


void info_ul(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unambiguous languages : UL.");
    print_dline_box(0, out, " Definition : Contains all finite disjoint unions of unambiguous AT-monomials.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order (FO²(<)).");
    print_dline_box(0, out, "  - Level Δ₂(<) in the alternation hierarchy of first-order logic with the linear order.");
    print_dline_box(0, out, "  - Unary temporal logic with finally and previously (TL = F + P).");
    print_dline_box(0, out, "  - Languages with a syntactic monoid in DA.");

}


void info_utl_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of MOD : TL(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(MOD) (UPol(BPol(MOD))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order and the modular predicates (FO²(<,MOD)).");
    print_dline_box(0, out, "  - Level Δ₂(<,MOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel is in DA.");

}

void info_utl_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of AMT : TL(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(AMT) (UPol(BPol(AMT))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order and the alphabetic modular predicates");
    print_dline_box(0, out, "     (FO²(<,AMOD)).");
    print_dline_box(0, out, "  - Level Δ₂(<,AMOD) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel is in DA.");

}
void info_utl_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of GR : TL(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(GR) (UPol(BPol(GR))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order and the group predicates (FO²(<,GR)).");
    print_dline_box(0, out, "  - Level Δ₂(<,GR) in the alternation hierarchy of first-order logic with the linear order and");
    print_dline_box(0, out, "    the group predicates.");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR-kernel is in DA.");

}
void info_utl_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of DD : TL(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure BPol(DD) (UPol(BPol(DD))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order and the successor (FO²(<,+1)).");
    print_dline_box(0, out, "  - Level Δ₂(<,+1) in the alternation hierarchy of first-order logic with the linear order and the");
    print_dline_box(0, out, "    successor.");
    print_dline_box(0, out, "  - Unary temporal logic with finally, next, previously and yesterday (TLX = F + X + P + Y).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits are in DA.");

}
void info_utl_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of MOD⁺ : TL(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(MOD⁺) (UPol(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order, the successor and the modular predicates");
    print_dline_box(0, out, "    (FO²(<,+1,MOD)).");
    print_dline_box(0, out, "  - Level Δ₂(<,+1,MOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the modular predicates.");
    print_dline_box(0, out, "  - Extended unary temporal logic closure of MOD (TLX(MOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD⁺-orbits are in DA.");

}
void info_utl_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of AMT⁺ : TL(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(AMT⁺) (UPol(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order, the successor and the alphabetic");
    print_dline_box(0, out, "    modular predicates (FO²(<,+1,AMOD)).");
    print_dline_box(0, out, "  - Level Δ₂(<,+1,AMOD) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the alphabetic modular predicates.");
    print_dline_box(0, out, "  - Extended unary temporal logic closure of AMT (TLX(AMT)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT⁺-orbits are in DA.");

}
void info_utl_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of GR⁺ : TL(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Unambiguous polynomial closure of BPol(GR⁺) (UPol(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Two-variable first-order logic with the linear order, the successor and the group predicates");
    print_dline_box(0, out, "    (FO²(<,+1,GR)).");
    print_dline_box(0, out, "  - Level Δ₂(<,+1,GR) in the alternation hierarchy of first-order logic with the linear order,");
    print_dline_box(0, out, "    the successor and the group predicates.");
    print_dline_box(0, out, "  - Extended unary temporal logic closure of GR (TLX(GR)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR⁺-orbits are in DA.");

}






void info_utl2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(ST) : TL₂(ST).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Two-variable first-order logic with alphabetic predicates (FO²(AT)).");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis AT (TL(AT)).");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(ST) (TL(BPol(ST))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis ST (TL₂(ST)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(ST)-orbits are in DA.");

}
void info_utl2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(MOD) : TL₂(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(MOD) (TL(BPol(MOD))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis MOD (TL₂(MOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD)-orbits are in DA.");

}
void info_utl2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(AMT) : TL₂(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(AMT) (TL(BPol(AMT))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis AMT (TL₂(AMT)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT)-orbits are in DA.");

}
void info_utl2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(GR) : TL₂(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(GR) (TL(BPol(GR))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis GR (TL₂(GR)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR)-orbits are in DA.");

}
void info_utl2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(DD) : TL₂(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis LT (TL(LT)).");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(DD) (TL(BPol(DD))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis DD (TL₂(DD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(DD)-orbits are in DA.");

}
void info_utl2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(MOD⁺) : TL₂(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(MOD⁺) (TL(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis MOD⁺ (TL₂(MOD⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits are in DA.");

}
void info_utl2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(AMT⁺) : TL₂(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(AMT⁺) (TL(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis AMT⁺ (TL₂(AMT⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT⁺)-orbits are in DA.");

}
void info_utl2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Unary temporal logic closure of TL(GR⁺) : TL₂(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the nested temporal hierarchy of basis BPol(GR⁺) (TL(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Level two in the nested temporal hierarchy of basis GR⁺ (TL₂(GR⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR⁺)-orbits are in DA.");

}





/***************/
/* FTL closure */
/***************/




void info_ftl_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of ST : FL(ST).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(ST) (RPol(BPol(ST))).");
    print_dline_box(0, out, "  - Unary temporal logic with finally (FL = F).");
    print_dline_box(0, out, "  - Languages with an L-trivial syntactic monoid.");

}


void info_ftl_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of MOD : FL(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(MOD) (RPol(BPol(MOD))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel is L-trivial.");

}

void info_ftl_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of AMT : FL(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(AMT) (RPol(BPol(AMT))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT-kernel is L-trivial.");

}
void info_ftl_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of GR : FL(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(GR) (RPol(BPol(GR))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR-kernel is L-trivial.");

}
void info_ftl_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of DD : FL(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(DD) (RPol(BPol(DD))).");
    print_dline_box(0, out, "  - Unary temporal logic with finally and next (FLX = F + X).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits are L-trivial.");

}
void info_ftl_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of MOD⁺ : FL(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(MOD⁺) (RPol(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD⁺-orbits are L-trivial.");

}
void info_ftl_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of AMT⁺ : FL(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(AMT⁺) (RPol(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT⁺-orbits are L-trivial.");

}
void info_ftl_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of GR⁺ : FL(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Right polynomial closure of BPol(GR⁺) (RPol(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR⁺-orbits are L-trivial.");

}




void info_ftl2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(ST) : FL₂(ST).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis AT (FL(AT)).");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(ST) (FL(BPol(ST))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis ST (FL₂(ST)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(ST)-orbits are L-trivial.");

}
void info_ftl2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(MOD) : FL₂(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(MOD) (FL(BPol(MOD))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis MOD (FL₂(MOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD)-orbits are L-trivial.");

}
void info_ftl2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(AMT) : FL₂(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(AMT) (FL(BPol(AMT))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis AMT (FL₂(AMT)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT)-orbits are L-trivial.");

}
void info_ftl2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(GR) : FL₂(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(GR) (FL(BPol(GR))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis GR (FL₂(GR)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR)-orbits are L-trivial.");

}
void info_ftl2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(DD) : FL₂(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis LT (FL(LT)).");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(DD) (FL(BPol(DD))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis DD (FL₂(DD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(DD)-orbits are L-trivial.");

}
void info_ftl2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(MOD⁺) : FL₂(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(MOD⁺) (FL(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis MOD⁺ (FL₂(MOD⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits are L-trivial.");

}
void info_ftl2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(AMT⁺) : FL₂(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(AMT⁺) (FL(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis AMT⁺ (FL₂(AMT⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT⁺)-orbits are L-trivial.");

}
void info_ftl2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Future temporal logic closure of FL(GR⁺) : FL₂(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(GR⁺) (FL(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis GR⁺ (FL₂(GR⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR⁺)-orbits are L-trivial.");

}









/***************/
/* PTL closure */
/***************/




void info_ptl_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of ST : PL(ST).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(ST) (LPol(BPol(ST))).");
    print_dline_box(0, out, "  - Unary temporal logic with previously (PL = P).");
    print_dline_box(0, out, "  - Languages with an R-trivial syntactic monoid.");

}


void info_ptl_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of MOD : PL(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(MOD) (LPol(BPol(MOD))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD-kernel is R-trivial.");

}

void info_ptl_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of AMT : PL(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(AMT) (LPol(BPol(AMT))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT-kernel is R-trivial.");

}
void info_ptl_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of GR : PL(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(GR) (LPol(BPol(GR))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR-kernel is R-trivial.");

}
void info_ptl_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of DD : PL(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(DD) (LPol(BPol(DD))).");
    print_dline_box(0, out, "  - Unary temporal logic with previously and yesterday (PLX = P + Y).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits are R-trivial.");

}
void info_ptl_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of MOD⁺ : PL(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(MOD⁺) (LPol(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose MOD⁺-orbits are R-trivial.");

}
void info_ptl_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of AMT⁺ : PL(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(AMT⁺) (LPol(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose AMT⁺-orbits are R-trivial.");

}
void info_ptl_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of GR⁺ : PL(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Left polynomial closure of BPol(GR⁺) (LPol(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose GR⁺-orbits are R-trivial.");

}




void info_ptl2_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(ST) : PL₂(ST).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis AT (PL(AT)).");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(ST) (PL(BPol(ST))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis ST (PL₂(ST)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(ST)-orbits are R-trivial.");

}
void info_ptl2_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(MOD) : PL₂(MOD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(MOD) (PL(BPol(MOD))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis MOD (PL₂(MOD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD)-orbits are R-trivial.");

}
void info_ptl2_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(AMT) : PL₂(AMT).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(AMT) (PL(BPol(AMT))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis AMT (PL₂(AMT)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT)-orbits are R-trivial.");

}
void info_ptl2_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(GR) : PL₂(GR).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(GR) (PL(BPol(GR))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis GR (PL₂(GR)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR)-orbits are R-trivial.");

}
void info_ptl2_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(DD) : PL₂(DD).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis LT (PL(LT)).");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(DD) (PL(BPol(DD))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis DD (PL₂(DD)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(DD)-orbits are R-trivial.");

}
void info_ptl2_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(MOD⁺) : PL₂(MOD⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(MOD⁺) (PL(BPol(MOD⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis MOD⁺ (PL₂(MOD⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(MOD⁺)-orbits are R-trivial.");

}
void info_ptl2_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(AMT⁺) : PL₂(AMT⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(AMT⁺) (PL(BPol(AMT⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis AMT⁺ (PL₂(AMT⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(AMT⁺)-orbits are R-trivial.");

}
void info_ptl2_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Past temporal logic closure of PL(GR⁺) : PL₂(GR⁺).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Level one in the future nested temporal hierarchy of basis BPol(GR⁺) (PL(BPol(GR⁺))).");
    print_dline_box(0, out, "  - Level two in the future nested temporal hierarchy of basis GR⁺ (PL₂(GR⁺)).");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose BPol(GR⁺)-orbits are R-trivial.");

}


/******************/
/* Single classes */
/******************/

void info_at(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Alphabet testable languages : AT.");
    print_dline_box(0, out, " Definition : Boolean combinations of languages A*aA* where a ∊ A is a letter.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - One variable first-order logic (FO¹(∅)).");
    print_dline_box(0, out, "  - Languages with an idempotent and commutative syntactic monoid.");

}
void info_att(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Alphabet threshold testable languages : ATT.");
    print_dline_box(0, out, " Definition : Boolean combinations of languages of the form (A*aA*)ᵏ where a ∊ A is a letter");
    print_dline_box(0, out, "    and k ≥ 1 is a number.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with equality only (FO(∅)).");
    print_dline_box(0, out, "  - Languages with an aperiodic and commutative syntactic monoid.");

}

void info_lt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Locally testable languages : LT.");
    print_dline_box(0, out, " Definition : Boolean combinations of languages of the form wA*, A*w and A*wA* where w ∊ A*");
    print_dline_box(0, out, "    is an arbitrary word.");
    print_dline_box(0, out, " Characterization :");
    print_dline_box(0, out, "  - Languages with a syntactic morphism whose DD-orbits are idempotent and commutative.");

}
void info_ltt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Locally threshold testable languages : LTT.");
    print_dline_box(0, out, " Definition : Boolean combinations of languages of the form wA*, A*w and F(w,k) where w ∊ A*");
    print_dline_box(0, out, "    is an arbitrary word and k ≥ 1 (F(w,k) consists of all words with k occurrences of the infix w).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - First-order logic with successor (FO(+1)).");
    print_dline_box(0, out, "  - Languages with an aperiodic syntactic semigroup S satisfying the equation erfsetf = etfserf");
    print_dline_box(0, out, "    for all elements r,s,t ∊ S and all idempotents e,f ∊ E(S).");

}
void info_st(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Basis of the Straubing-Thérien hierarchy : ST.");
    print_dline_box(0, out, " Definition : trivial class consisting of the languages ∅ and A*.");
    print_dline_box(0, out, " Characterization : Languages with a trivial syntactic monoid.");

}
void info_dd(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Basis of the dot-depth hierarchy : DD.");
    print_dline_box(0, out, " Definition : class consisting of the languages ∅, {ε}, A⁺ and A*.");
    print_dline_box(0, out, " Characterization : Languages with a trivial syntactic semigroup.");

}
void info_mod(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Modulo languages : MOD.");
    print_dline_box(0, out, " Definition : Finite unions of languages (Aⁿ)*Aᵏ where n > k ≥ 1 (membership of a word");
    print_dline_box(0, out, "    in the language depends only on its length modulo a fixed integer).");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Languages with a syntactic morphism into a group mapping all a ∊ A to the same element.");
    print_dline_box(0, out, "  - Languages with a minimal automaton which a permutation automaton in which every a ∊ A");
    print_dline_box(0, out, "    has the same action on the states.");

}

void info_modp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Extended modulo languages : MOD⁺.");
    print_dline_box(0, out, " Definition : Least Boolean algebra containing MOD and {ε}.");
    print_dline_box(0, out, " Characterizations : Languages with a syntactic semigroup which is a group and whose");
    print_dline_box(0, out, "    syntactic morphism maps every letter a to the same element.");

}
void info_amt(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Alphabet modulo testable languages AMT.");
    print_dline_box(0, out, " Definition : Boolean combinations of languages which count the occurrences of some letter");
    print_dline_box(0, out, "    a ∊ A modulo an integer.");
    print_dline_box(0, out, " Characterization : Languages with a syntactic monoid which is a commutative group.");


}
void info_amtp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Extended alphabetic modulo testable languages : AMT⁺.");
    print_dline_box(0, out, " Definition : Least Boolean algebra containing AMT and {ε}.");
    print_dline_box(0, out, " Characterization : Languages with a syntactic semigroup which is a commutative group.");

}
void info_gr(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Group languages GR.");
    print_dline_box(0, out, " Characterizations :");
    print_dline_box(0, out, "  - Languages with a syntactic monoid which is a group.");
    print_dline_box(0, out, "  - Languages with a minimal automaton which is a permutation automaton.");


}
void info_grp(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Extended group languages : GR⁺.");
    print_dline_box(0, out, " Definition : Least Boolean algebra containing GR and {ε}.");
    print_dline_box(0, out, " Characterization : Languages with a syntactic semigroup which is a group.");


}

void info_reg(FILE* out) {
    print_dtitle_box(10, false, out, 1, "Regular languages : REG.");
    print_dline_box(0, out, " Definition : Languages degfined by a regular expression.");
}









/*********************/
    /* Affichage partagé */
    /*********************/

void print_info_input(int i, FILE* out)
{
    switch (objects[i]->type)
    {
    case REGEXP:
        print_title_box(10, true, out, 1, "Input: a regular expression.");
        fprintf(out, "#### Regular expression  : ");
        reg_print(objects[i]->exp);

        /* morphism* morp = objects[lang->syntmor]->mor;
    fprintf(out, "#### Syntactic monoid    :\n");
    print_full_green(morp->morphism, morp->green, false, out);
    fprintf(out, "#### Syntactic morphism  :\n");
    morphism_print_morphism(morp->morphism, out); */

        break;
    case MORPHISM:
        print_title_box(10, true, out, 1, "Input: a morphism into a finite monoid.");
        fprintf(out, "#### The monoid          :\n");
        print_full_green(objects[i]->mor->obj, false, out);
        fprintf(out, "#### The morphism        :\n");
        mor_print_mapping(objects[i]->mor->obj, out);
        break;
    case AUTOMATON:
        print_title_box(10, true, out, 1, "Input: an automaton.");
        fprintf(out, "#### The automaton  :\n");
        view_nfa(objects[i]->nfa);
        break;
    default:
        break;
    }
}

void print_start_comp(FILE* out, char* class)
{
    char mess[MESSAGESIZE];
    sprintf(mess, "Deciding membership of the input language in %s.", class);
    print_dtitle_box(10, true, out, 1, mess);
}

void print_conclusion_comp(FILE* out, bool res, const char* namec)
{
    char mess[MESSAGESIZE];
    if (res)
    {
        sprintf(mess, "The recognized language belongs to %s.", namec);
    }
    else
    {
        sprintf(mess, "The input recognized language does NOT belong to %s.", namec);
    }

    print_dtitle_box(10, true, out, 1, mess);
}


void print_conclusion_separ(FILE* out, bool res, const char* namec)
{
    char mess[MESSAGESIZE];
    if (res)
    {
        sprintf(mess, "The input languages are %s-separable.", namec);
    }
    else
    {
        sprintf(mess, "The input languages are NOT %s-separable.", namec);
    }

    print_dtitle_box(10, true, out, 1, mess);
}



/***********************************************/
/* Affichage des informations sur un opérateur */
/***********************************************/



static void print_input_name(com_keyword c, FILE* out)
{
    switch (c)
    {
    case KY_ST:
        print_dline_box(0, out, " Applied to the input class ST : trivial class consisting of the languages ∅ and A*.");
        break;
    case KY_DD:
        print_dline_box(0, out, " Applied to the input class DD : class consisting of the languages ∅, {ε}, A⁺ and A*.");
        break;
    case KY_AT:
        print_dline_box(0, out, " Applied to the input class AT : alphabet testable languages, membership of a word in the language");
        print_dline_box(0, out, "    depends only on the set of letters occuring inside this word.");
        break;
    case KY_MOD:
        print_dline_box(0, out, " Applied to the input class MOD : modulo languages, membership of a word in the language depends");
        print_dline_box(0, out, "    only on its length modulo a fixed number.");
        break;
    case KY_MODP:
        print_dline_box(0, out, " Applied to the input class MOD⁺ : well-suited extension of the modulo languages, membership of a");
        print_dline_box(0, out, "    word in the language depends only on its length modulo a fixed number and on whether this word");
        print_dline_box(0, out, "    is empty or not. In this case, SF(MOD⁺) = SF(MOD).");
        break;
    case KY_GR:
        print_dline_box(0, out, " Applied to the input class GR : group languages (recognized by a finite group).");
        break;
    default:
        fprintf(out, "#### This input class is not supported.\n\n");
        break;
    }
}



// Negation hierarchies
void print_infooper_neghiera(com_keyword c, FILE* out)
{
    switch (c)
    {
    case KY_ST:
        print_dtitle_box(10, false, out, 1, "Negation hierarchy of unary temporal logic (TL = TL(ST)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " For each natural number n ≥ 1, the level n corresponds to the following classes:");
        print_dline_box(0, out, "  - Level TLₙ in the negation hierarchy of TL.");
        print_dline_box(0, out, "  - Level BΣ²ₙ(<) in the alternation hierarchy of two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order.");
        print_dline_box(0, out, "  - Level MPolₖ(BPol(ST)) for k = n - 1 in the deterministic hierarchy of basis BPol(ST).");
        print_dbot_line(100, out);
        break;
    case KY_DD:
        print_dtitle_box(10, false, out, 1, "Negation hierarchy of the unary temporal logic closure of DD (TL(DD)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " For each natural number n ≥ 1, the level n corresponds to the following classes:");
        print_dline_box(0, out, "  - Level TLₙ(DD) in the negation hierarchy of TL(DD).");
        print_dline_box(0, out, "  - Level BΣ²ₙ(<,+1) in the alternation hierarchy of two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order and the successor.");
        print_dline_box(0, out, "  - Level MPolₖ(BPol(DD)) for k = n - 1 in the deterministic hierarchy of basis BPol(DD).");
        print_dbot_line(100, out);
        break;
    case KY_MOD:
        print_dtitle_box(10, false, out, 1, "Negation hierarchy of the unary temporal logic closure of MOD (TL(MOD)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " For each natural number n ≥ 1, the level n corresponds to the following classes:");
        print_dline_box(0, out, "  - Level TLₙ(MOD) in the negation hierarchy of TL(MOD).");
        print_dline_box(0, out, "  - Level BΣ²ₙ(<,MOD) in the alternation hierarchy of two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order and the modular predicates.");
        print_dline_box(0, out, "  - Level MPolₖ(BPol(MOD)) for k = n - 1 in the deterministic hierarchy of basis BPol(MOD).");
        print_dbot_line(100, out);
        break;
    case KY_MODP:
        print_dtitle_box(10, false, out, 1, "Negation hierarchy of the unary temporal logic closure of MOD (TL(MOD⁺)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " For each natural number n ≥ 1, the level n corresponds to the following classes:");
        print_dline_box(0, out, "  - Level TLₙ(MOD⁺) in the negation hierarchy of TL(MOD⁺).");
        print_dline_box(0, out, "  - Level BΣ²ₙ(<,+1,MOD) in the alternation hierarchy of two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order, the successor and the modular predicates.");
        print_dline_box(0, out, "  - Level MPolₖ(BPol(MOD⁺)) for k = n - 1 in the deterministic hierarchy of basis BPol(MOD⁺).");
        print_dbot_line(100, out);
        break;
    case KY_GR:
        print_dtitle_box(10, false, out, 1, "Negation hierarchy of the unary temporal logic closure of GR (TL(GR)).");
        print_input_name(c, out);
        print_dmid_line(100, out);
        print_dline_box(0, out, " For each natural number n ≥ 1, the level n corresponds to the following classes:");
        print_dline_box(0, out, "  - Level TLₙ(GR) in the negation hierarchy of TL(MOD).");
        print_dline_box(0, out, "  - Level BΣ²ₙ(<,GR) in the alternation hierarchy of two-variable first-order logic with");
        print_dline_box(0, out, "    the linear order and the group predicates.");
        print_dline_box(0, out, "  - Level MPolₖ(BPol(GR)) for k = n - 1 in the deterministic hierarchy of basis BPol(GR).");
        print_dbot_line(100, out);
        break;
    default:
        break;
    }
}

// Future/past hierarchies
void print_infooper_fphiera(com_keyword c, FILE* out)
{
    switch (c)
    {
    case KY_ST:
        print_dtitle_box(10, false, stdout, 1, "Future/Past hierarchy of unary temporal logic (TL = TL(ST)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout, " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level RPolₙ(BPol(ST)) in the deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout, "  - If n is even, level LPolₙ(BPol(ST)) in the deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout, " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level LPolₙ(BPol(ST)) in the deterministic hierarchy of basis BPol(ST).");
        print_dline_box(0, stdout, "  - If n is even, level RPolₙ(BPol(ST)) in the deterministic hierarchy of basis BPol(ST).");
        print_dbot_line(100, stdout);
        break;
    case KY_DD:
        print_dtitle_box(10, false, stdout, 1, "Future/Past hierarchy of the unary temporal logic closure of DD (TL(DD)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout, " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level RPolₙ(BPol(DD)) in the deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout, "  - If n is even, level LPolₙ(BPol(DD)) in the deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout, " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level LPolₙ(BPol(DD)) in the deterministic hierarchy of basis BPol(DD).");
        print_dline_box(0, stdout, "  - If n is even, level RPolₙ(BPol(DD)) in the deterministic hierarchy of basis BPol(DD).");
        print_dbot_line(100, stdout);
        break;
    case KY_MOD:
        print_dtitle_box(10, false, stdout, 1,
            "Future/Past hierarchy of the unary temporal logic "
            "closure of MOD (TL(MOD)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout,
            " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level RPolₙ(BPol(MOD)) in the deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout, "  - If n is even, level LPolₙ(BPol(MOD)) in the deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout, " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level LPolₙ(BPol(MOD)) in the deterministic hierarchy of basis BPol(MOD).");
        print_dline_box(0, stdout, "  - If n is even, level RPolₙ(BPol(MOD)) in the deterministic hierarchy of basis BPol(MOD).");
        print_dbot_line(100, stdout);
        break;
    case KY_MODP:
        print_dtitle_box(10, false, stdout, 1, "Future/Past hierarchy of the unary temporal logic closure of MOD⁺ (TL(MOD⁺)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout, " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level RPolₙ(BPol(MOD⁺)) in the deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout, "  - If n is even, level LPolₙ(BPol(MOD⁺)) in the deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout, " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level LPolₙ(BPol(MOD⁺)) in the deterministic hierarchy of basis BPol(MOD⁺).");
        print_dline_box(0, stdout, "  - If n is even, level RPolₙ(BPol(MOD⁺)) in the deterministic hierarchy of basis BPol(MOD⁺).");
        print_dbot_line(100, stdout);
        break;
    case KY_GR:
        print_dtitle_box(10, false, stdout, 1, "Future/Past hierarchy of the unary temporal logic closure of GR (TL(GR)).");
        print_input_name(c, out);
        print_dmid_line(100, stdout);
        print_dline_box(0, stdout, " Characterization of the level FLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level RPolₙ(BPol(GR)) in the deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout, "  - If n is even, level LPolₙ(BPol(GR)) in the deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout, " Characterization of the level PLⁿ (for n ≥ 1):");
        print_dline_box(0, stdout, "  - If n is odd, level LPolₙ(BPol(GR)) in the deterministic hierarchy of basis BPol(GR).");
        print_dline_box(0, stdout, "  - If n is even, level RPolₙ(BPol(GR)) in the deterministic hierarchy of basis BPol(GR).");
        print_dbot_line(100, stdout);
        break;
    default:
        break;
    }
}
