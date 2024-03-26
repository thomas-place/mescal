/******************************************/
/* Procèdures de séparation et membership */
/******************************************/

#include "shell_sepmem.h"

/******************************************/
/* Commandes pour les tests de Membership */
/******************************************/

// Message d'erreur pour les automates
static void shell_membauto_error(void) {
  fprintf(stderr, "Error: Memberhip is not supported for input automata.\n");
}

// Message d'erreur pour les tests non-disponibles
static void shell_membunav_error(void) {
  fprintf(stderr, "Error: Memberhip is not supported for this class.\n");
}

// Récupère l'indice du morphisme correspondant à un objet de type morphisme ou langage
static morphism* shell_get_tiedmor(int i) {
  switch (objects[i]->type)
  {
  case LANGUAGE:
    return objects[shell_compute_syntac(i)]->theob.mor;
    break;
  case MORPHISM:
    return objects[i]->theob.mor;
    break;
  case AUTOMATA:
    return NULL;
    break;
  default:
    return NULL;
    break;
  }
}

bool shell_membership_test(com_parameters* pars) {
  int n = com_nbparams(pars);

  // Il doit y avoir entre deux et trois arguments:
  // - un ou deux pour la classe (classe unique ou opérateur + input)
  // - la variable qui donne le langage ou le morphisme input
  if (n < 1 || n > 3) {
    shell_arguments_error();
    return false;
  }


  // On commence par récupérer la variable (dernier argment)
  int i = index_from_string_chain(com_getparam(pars, n - 1)->main);

  if (i == -1) {
    shell_variable_error();
    return false;
  }

  // On vérifie que l'input n'est pas un automate
  object_type thetype = objects[i]->type;
  if (thetype == AUTOMATA) {
    shell_membauto_error();
    return false;
  }

  // On récupère le morphisme associé à l'input
  morphism* morp = shell_get_tiedmor(i);
  // On aura besoin de ses relation de Green
  shell_compute_green(morp);

  // Il y a deux cas suivant le nombre d'arguments
  com_keyword arg1 = key_from_string_chain_single(pars->param->main);


  // Si il y a 2 arguments, on cherche une classe
  if (n == 2) {
    // Switch sur le keyword (une classe)
    switch (arg1) {
    case CL_AT:
      print_infoclass_at(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "AT", thetype);
      print_conclusion_comp(stdout, is_at_mono(morp->cayley, stdout), "AT", thetype);
      break;
    case CL_ATT:
      print_infoclass_att(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "ATT", thetype);
      print_conclusion_comp(stdout, is_att_mono(morp->cayley, morp->green, stdout), "ATT", thetype);
      break;
    case CL_LT:
      print_infoclass_lt(stdout);
      print_info_input(i, stdout);
      shell_compute_ddorbs(morp); // On a besoin des DD-orbites
      print_start_comp(stdout, "LT", thetype);
      print_conclusion_comp(stdout, is_lt_mono(morp->cayley, morp->green, morp->ddorbs, stdout), "LT", thetype);
      break;
    case CL_LTT:
      print_infoclass_ltt(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "LTT", thetype);
      print_conclusion_comp(stdout, is_ltt_mono(morp->cayley, morp->green, stdout), "LTT", thetype);
      break;
    case CL_SF:
      print_infoclass_sf(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "SF", thetype);
      print_conclusion_comp(stdout, is_aperiodic_mono(morp->cayley, morp->green, stdout), "SF", thetype);
      break;
    case CL_GR:
      print_infoclass_gr(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "GR", thetype);
      print_conclusion_comp(stdout, is_group_mono(morp->cayley, morp->green, stdout), "GR", thetype);
      break;
    case CL_AMT:
      print_infoclass_amt(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "AMT", thetype);
      print_conclusion_comp(stdout, is_group_mono(morp->cayley, morp->green, stdout) && is_comm_mono(morp->cayley, stdout), "AMT", thetype);
      break;
    case CL_MOD:
      print_infoclass_mod(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "MOD", thetype);
      print_conclusion_comp(stdout, is_group_mono(morp->cayley, morp->green, stdout) && is_letterind_mono(morp->cayley, stdout), "MOD", thetype);
      break;
    case CL_ST:
      print_infoclass_st(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "ST", thetype);
      print_conclusion_comp(stdout, is_trivial_monoid(morp->cayley, stdout), "ST", thetype);
      break;
    case CL_PT:
      print_infoclass_pt(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "PT", thetype);
      print_conclusion_comp(stdout, is_jtrivial_mono(morp->cayley, morp->green, stdout), "PT", thetype);
      break;
    case CL_PPT:
      print_infoclass_ppt(stdout);
      print_info_input(i, stdout);
      shell_compute_order(morp); // On a besoin de l'ordre
      print_start_comp(stdout, "PPT", thetype);
      print_conclusion_comp(stdout, is_jsat_mono(morp->cayley, stdout), "PPT", thetype);
      break;
    case CL_UL:
      print_infoclass_ul(stdout);
      print_info_input(i, stdout);
      print_start_comp(stdout, "UL", thetype);
      print_conclusion_comp(stdout, is_da_mono(morp->cayley, morp->green, stdout), "UL", thetype);
      break;
    default:
      shell_membunav_error();
      return false;
      break;
    }
  }
  // Dans ce cas on cherche un opérateur + une input class
  if (n == 3) {

    com_keyword arg2 = key_from_string_chain_single(pars->next->param->main);

    switch (arg1)
    {
    case CL_SF:
    case OP_SFC:
      print_infooper_sf(arg2, stdout);
      print_info_input(i, stdout);
      return shell_sfc_mem(arg2, morp, thetype);
      break;
    case OP_TLC:
      print_infooper_tl(arg2, stdout);
      print_info_input(i, stdout);
      return shell_tlc_mem(arg2, morp, thetype);
      break;
    case OP_TLC2:
      print_infooper_tl2(arg2, stdout);
      print_info_input(i, stdout);
      return shell_tlctwo_mem(arg2, morp, thetype);
      break;
    case OP_FLC:
      print_infooper_fl(arg2, stdout);
      print_info_input(i, stdout);
      return shell_flc_mem(arg2, morp, thetype);
      break;
    case OP_FLC2:
      print_infooper_fl2(arg2, stdout);
      print_info_input(i, stdout);
      return shell_flctwo_mem(arg2, morp, thetype);
      break;
    case OP_PLC:
      print_infooper_pl(arg2, stdout);
      print_info_input(i, stdout);
      return shell_plc_mem(arg2, morp, thetype);
      break;
    case OP_PLC2:
      print_infooper_pl2(arg2, stdout);
      print_info_input(i, stdout);
      return shell_plctwo_mem(arg2, morp, thetype);
      break;
    case OP_POL:
      print_infooper_pol(arg2, stdout);
      print_info_input(i, stdout);
      return shell_pol_mem(arg2, morp, thetype);
      break;
    case OP_BPOL:
      print_infooper_bpol(arg2, stdout);
      print_info_input(i, stdout);
      return shell_bpol_mem(arg2, morp, thetype);
      break;
    case OP_POL2:
      print_infooper_pol2(arg2, stdout);
      print_info_input(i, stdout);
      return shell_poltwo_mem(arg2, morp, thetype);
      break;
    case OP_UBPOL:
      print_infooper_ubpol(arg2, stdout);
      print_info_input(i, stdout);
      return shell_ubpol_mem(arg2, morp, thetype);
      break;
    case OP_UBPOL2:
      print_infooper_ubpol2(arg2, stdout);
      print_info_input(i, stdout);
      return shell_ubpoltwo_mem(arg2, morp, thetype);
      break;
    default:
      shell_membunav_error();
      return false;
      break;
    }


  }
  return true;
}

/************************************************/
/* Specific membership procedures for operators */
/************************************************/


// Star-free closure
bool shell_sfc_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
  case CL_DD:
  case CL_AT:
    print_start_comp(stdout, "SF", thetype);
    print_conclusion_comp(stdout, is_aperiodic_mono(morp->cayley, morp->green, stdout), "SF", thetype);
    break;
  case CL_MOD:
  case CL_MODP:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "SF(MOD)", thetype);
    print_conclusion_comp(stdout, is_aperiodic_submono(morp->mker, "MOD-kernel", stdout), "SF(MOD)", thetype);
    break;
  case CL_GR:
    shell_compute_gker(morp); // On va avoir besoin du GR-kernel
    print_start_comp(stdout, "SF(GR)", thetype);
    print_conclusion_comp(stdout, is_aperiodic_submono(morp->gker, "GR-kernel", stdout), "SF(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }
  return true;
}

// Temporal logic 
bool shell_tlc_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "TL(ST)", thetype);
    print_conclusion_comp(stdout, is_da_mono(morp->cayley, morp->green, stdout), "TL(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_ddorbs(morp); // On va avoir besoin des DD-orbites
    print_start_comp(stdout, "TL(DD)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono_opti(morp->ddorbs, "DD", stdout), "TL(DD)", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "TL(AT)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(morp->atorbs, "AT", stdout), "TL(AT)", thetype);
    break;
  case CL_MOD:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "TL(MOD)", thetype);
    print_conclusion_comp(stdout, is_da_submono(morp->mker, "MOD-kernel", stdout), "TL(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_mporbs(morp); // On va avoir besoin des MOD⁺-orbites
    print_start_comp(stdout, "TL(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono_opti(morp->mporbs, "MOD⁺", stdout), "TL(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_gker(morp); // On va avoir besoin du GR-kernel
    print_start_comp(stdout, "TL(GR)", thetype);
    print_conclusion_comp(stdout, is_da_submono(morp->mker, "GR-kernel", stdout), "TL(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Nested temporal logic
bool shell_tlctwo_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "TL₂(ST)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(morp->atorbs, "TL(ST)", stdout), "TL₂(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_bpddorbs(morp); // On va avoir besoin des BPol(DD)-orbites
    print_start_comp(stdout, "TL₂(DD)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(shell_get_bpddorbs(morp), "TL(DD)", stdout), "TL₂(DD)", thetype);
    break;
  case CL_MOD:
    shell_compute_bpmorbs(morp); // On va avoir besoin des BPol(MOD)-orbites
    print_start_comp(stdout, "TL₂(MOD)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(shell_get_bpmorbs(morp), "TL(MOD)", stdout), "TL₂(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_bpmporbs(morp); // On va avoir besoin des BPol(MOD⁺)-orbites
    print_start_comp(stdout, "TL₂(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(shell_get_bpmporbs(morp), "TL(MOD⁺)", stdout), "TL₂(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_bpgorbs(morp); // On va avoir besoin des BPol(GR)-orbites
    print_start_comp(stdout, "TL₂(GR)", thetype);
    print_conclusion_comp(stdout, is_da_orbmono(morp->bpgorbs, "TL(GR)", stdout), "TL₂(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Future temporal logic
bool shell_flc_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "FL(ST)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_mono(morp->cayley, morp->green, stdout), "FL(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_ddorbs(morp); // On va avoir besoin des DD-orbites
    print_start_comp(stdout, "FL(DD)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono_opti(morp->ddorbs, "DD", stdout), "FL(DD)", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "FL(AT)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(morp->atorbs, "AT", stdout), "FL(AT)", thetype);
    break;
  case CL_MOD:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "FL(MOD)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_submono(morp->mker, "MOD-kernel", stdout), "FL(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_mporbs(morp); // On va avoir besoin des MOD⁺-orbites
    print_start_comp(stdout, "FL(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono_opti(morp->mporbs, "MOD⁺", stdout), "FL(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_gker(morp); // On va avoir besoin du GR-kernel
    print_start_comp(stdout, "FL(GR)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_submono(morp->mker, "GR-kernel", stdout), "FL(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Nested future temporal logic
bool shell_flctwo_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "FL₂(ST)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(morp->atorbs, "FL(ST)", stdout), "FL₂(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_bpddorbs(morp); // On va avoir besoin des BPol(DD)-orbites
    print_start_comp(stdout, "FL₂(DD)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(shell_get_bpddorbs(morp), "FL(DD)", stdout), "FL₂(DD)", thetype);
    break;
  case CL_MOD:
    shell_compute_bpmorbs(morp); // On va avoir besoin des BPol(MOD)-orbites
    print_start_comp(stdout, "FL₂(MOD)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(shell_get_bpmorbs(morp), "FL(MOD)", stdout), "FL₂(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_bpmporbs(morp); // On va avoir besoin des BPol(MOD⁺)-orbites
    print_start_comp(stdout, "FL₂(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(shell_get_bpmporbs(morp), "FL(MOD⁺)", stdout), "FL₂(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_bpgorbs(morp); // On va avoir besoin des BPol(GR)-orbites
    print_start_comp(stdout, "FL₂(GR)", thetype);
    print_conclusion_comp(stdout, is_ltrivial_orbmono(morp->bpgorbs, "FL(GR)", stdout), "FL₂(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Past temporal logic
bool shell_plc_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "PL(ST)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_mono(morp->cayley, morp->green, stdout), "PL(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_ddorbs(morp); // On va avoir besoin des DD-orbites
    print_start_comp(stdout, "PL(DD)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono_opti(morp->ddorbs, "DD", stdout), "PL(DD)", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "PL(AT)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(morp->atorbs, "AT", stdout), "PL(AT)", thetype);
    break;
  case CL_MOD:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "PL(MOD)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_submono(morp->mker, "MOD-kernel", stdout), "PL(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_mporbs(morp); // On va avoir besoin des MOD⁺-orbites
    print_start_comp(stdout, "PL(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono_opti(morp->mporbs, "MOD⁺", stdout), "PL(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_gker(morp); // On va avoir besoin du GR-kernel
    print_start_comp(stdout, "PL(GR)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_submono(morp->mker, "GR-kernel", stdout), "PL(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Nested past temporal logic
bool shell_plctwo_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "PL₂(ST)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(morp->atorbs, "PL(ST)", stdout), "PL₂(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_bpddorbs(morp); // On va avoir besoin des BPol(DD)-orbites
    print_start_comp(stdout, "PL₂(DD)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(shell_get_bpddorbs(morp), "PL(DD)", stdout), "PL₂(DD)", thetype);
    break;
  case CL_MOD:
    shell_compute_bpmorbs(morp); // On va avoir besoin des BPol(MOD)-orbites
    print_start_comp(stdout, "PL₂(MOD)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(shell_get_bpmorbs(morp), "PL(MOD)", stdout), "PL₂(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_bpmporbs(morp); // On va avoir besoin des BPol(MOD⁺)-orbites
    print_start_comp(stdout, "PL₂(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(shell_get_bpmporbs(morp), "PL(MOD⁺)", stdout), "PL₂(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_bpgorbs(morp); // On va avoir besoin des BPol(GR)-orbites
    print_start_comp(stdout, "PL₂(GR)", thetype);
    print_conclusion_comp(stdout, is_rtrivial_orbmono(morp->bpgorbs, "PL(GR)", stdout), "PL₂(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Polynomial closure
bool shell_pol_mem(com_keyword c, morphism* morp, object_type thetype) {
  shell_compute_order(morp); // On va avoir besoin de l'ordre
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "Pol(ST)", thetype);
    print_conclusion_comp(stdout, is_jsat_mono(morp->cayley, stdout), "Pol(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_ddorbs(morp); // On va avoir besoin des DD-orbites
    print_start_comp(stdout, "Pol(DD)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(morp->ddorbs, "DD", stdout), "Pol(DD)", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "Pol(AT)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(morp->atorbs, "AT", stdout), "Pol(AT)", thetype);
    break;
  case CL_MOD:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "Pol(MOD)", thetype);
    print_conclusion_comp(stdout, is_jsat_submono(morp->mker, "MOD-kernel", stdout), "Pol(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_mporbs(morp); // On va avoir besoin des MOD⁺-orbites
    print_start_comp(stdout, "Pol(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(morp->mporbs, "MOD⁺", stdout), "Pol(MOD⁺)", thetype);
    break;
  case CL_GR:
    print_start_comp(stdout, "Pol(GR)", thetype);
    print_conclusion_comp(stdout, is_ejsat_mono(morp->cayley, stdout), "Pol(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Boolean polynomial closure
bool shell_bpol_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "BPol(ST)", thetype);
    print_conclusion_comp(stdout, is_jtrivial_mono(morp->cayley, morp->green, stdout), "BPol(ST)", thetype);
    break;
  case CL_DD:
    print_start_comp(stdout, "BPol(DD)", thetype);
    print_conclusion_comp(stdout, is_knast_mono(morp->cayley, morp->green, stdout), "BPol(DD)", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On a besoin des AT-orbites
    print_start_comp(stdout, "BPol(AT)", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(morp->atorbs, "AT", stdout) && is_knast_at_mono(morp->cayley, morp->green, stdout), "BPol(AT)", thetype);
    break;
  case CL_MOD:
    print_start_comp(stdout, "BPol(MOD)", thetype);
    print_conclusion_comp(stdout, is_bpolmod_mono(morp->cayley, morp->green, stdout), "BPol(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_mker(morp); // On a besoin du MOD-kernel
    print_start_comp(stdout, "BPol(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_qknast_mono(morp->mker, stdout), "BPol(MOD⁺)", thetype);
    break;
  case CL_GR:
    print_start_comp(stdout, "BPol(GR)", thetype);
    print_conclusion_comp(stdout, is_blockg_mono(morp->cayley, morp->green, stdout), "BPol(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// UPOL(BPOL)
bool shell_ubpol_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    print_start_comp(stdout, "UPol(BPol(ST))", thetype);
    print_conclusion_comp(stdout, is_da_mono(morp->cayley, morp->green, stdout), "UPol(BPol(ST))", thetype);
    break;
  case CL_DD:
    shell_compute_ddorbs(morp); // On va avoir besoin des DD-orbites
    print_start_comp(stdout, "UPol(BPol(DD))", thetype);
    print_conclusion_comp(stdout, is_da_orbmono_opti(morp->ddorbs, "DD", stdout), "UPol(BPol(DD))", thetype);
    break;
  case CL_AT:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "UPol(BPol(AT))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(morp->atorbs, "AT", stdout), "UPol(BPol(AT))", thetype);
    break;
  case CL_MOD:
    shell_compute_mker(morp); // On va avoir besoin du MOD-kernel
    print_start_comp(stdout, "UPol(BPol(MOD))", thetype);
    print_conclusion_comp(stdout, is_da_submono(morp->mker, "MOD-kernel", stdout), "UPol(BPol(MOD))", thetype);
    break;
  case CL_MODP:
    shell_compute_mporbs(morp); // On va avoir besoin des MOD⁺-orbites
    print_start_comp(stdout, "UPol(BPol(MOD⁺))", thetype);
    print_conclusion_comp(stdout, is_da_orbmono_opti(morp->mporbs, "MOD⁺", stdout), "UPol(BPol(MOD⁺))", thetype);
    break;
  case CL_GR:
    shell_compute_gker(morp); // On va avoir besoin du GR-kernel
    print_start_comp(stdout, "UPol(BPol(GR))", thetype);
    print_conclusion_comp(stdout, is_da_submono(morp->mker, "GR-kernel", stdout), "UPol(BPol(GR))", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Nested polynomial closure
bool shell_poltwo_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "Pol₂(ST)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(morp->atorbs, "BPol(ST)", stdout), "Pol₂(ST)", thetype);
    break;
  case CL_DD:
    shell_compute_bpddorbs(morp); // On va avoir besoin des BPol(DD)-orbites
    print_start_comp(stdout, "Pol₂(DD)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(shell_get_bpddorbs(morp), "BPol(DD)", stdout), "Pol₂(DD)", thetype);
    break;
  case CL_MOD:
    shell_compute_bpmorbs(morp); // On va avoir besoin des BPol(MOD)-orbites
    print_start_comp(stdout, "Pol₂(MOD)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(shell_get_bpmorbs(morp), "BPol(MOD)", stdout), "Pol₂(MOD)", thetype);
    break;
  case CL_MODP:
    shell_compute_bpmporbs(morp); // On va avoir besoin des BPol(MOD⁺)-orbites
    print_start_comp(stdout, "Pol₂(MOD⁺)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(shell_get_bpmporbs(morp), "BPol(MOD⁺)", stdout), "Pol₂(MOD⁺)", thetype);
    break;
  case CL_GR:
    shell_compute_bpgorbs(morp); // On va avoir besoin des BPol(GR)-orbites
    print_start_comp(stdout, "Pol₂(GR)", thetype);
    print_conclusion_comp(stdout, is_jsat_orbmono(morp->bpgorbs, "BPol(GR)", stdout), "Pol₂(GR)", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

// Nested polynomial closure
bool shell_ubpoltwo_mem(com_keyword c, morphism* morp, object_type thetype) {
  switch (c) {
  case CL_ST:
    shell_compute_atorbs(morp); // On va avoir besoin des AT-orbites
    print_start_comp(stdout, "UPol(BPol₂(ST))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(morp->atorbs, "BPol(ST)", stdout), "UPol(BPol₂(ST))", thetype);
    break;
  case CL_DD:
    shell_compute_bpddorbs(morp); // On va avoir besoin des BPol(DD)-orbites
    print_start_comp(stdout, "UPol(BPol₂(DD))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(shell_get_bpddorbs(morp), "BPol(DD)", stdout), "UPol(BPol₂(DD))", thetype);
    break;
  case CL_MOD:
    shell_compute_bpmorbs(morp); // On va avoir besoin des BPol(MOD)-orbites
    print_start_comp(stdout, "UPol(BPol₂(MOD))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(shell_get_bpmorbs(morp), "BPol(MOD)", stdout), "UPol(BPol₂(MOD))", thetype);
    break;
  case CL_MODP:
    shell_compute_bpmporbs(morp); // On va avoir besoin des BPol(MOD⁺)-orbites
    print_start_comp(stdout, "UPol(BPol₂(MOD⁺))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(shell_get_bpmporbs(morp), "BPol(MOD⁺)", stdout), "UPol(BPol₂(MOD⁺))", thetype);
    break;
  case CL_GR:
    shell_compute_bpgorbs(morp); // On va avoir besoin des BPol(GR)-orbites
    print_start_comp(stdout, "UPol(BPol₂(GR))", thetype);
    print_conclusion_comp(stdout, is_upbp_mono(morp->bpgorbs, "BPol(GR)", stdout), "UPol(BPol₂(GR))", thetype);
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

/********************************/
/* Summary for all main classes */
/********************************/

static void summary_answer(bool res) {
  if (res) {
    fprintf(stdout, "║" ANSI_COLOR_GREEN "     YES      " ANSI_COLOR_RESET);


  }
  else {
    fprintf(stdout, "║" ANSI_COLOR_RED "     NO       "ANSI_COLOR_RESET);

  }
}

bool shell_concat_summary(com_parameters* pars) {

  // Il doit y avoir un seul argument:
  // - la variable qui donne le langage ou le morphisme input
  if (com_nbparams(pars) != 1) {
    shell_arguments_error();
    return false;
  }

  // On commence par récupérer la variable (argment 1 ou 2)
  int i = index_from_string_chain(pars->param->main);

  if (i == -1) {
    shell_variable_error();
    return false;
  }

  // On vérifie que l'input n'est pas un automate
  if (objects[i]->type == AUTOMATA) {
    shell_membauto_error();
    return false;
  }

  // On récupère le morphisme associé à l'input
  morphism* morp = shell_get_tiedmor(i);
  // On aura besoin de ses relation de Green
  shell_compute_green(morp);
  // On aura besoin de l'ordre syntaxique
  shell_compute_order(morp);


  /***************************/
  /* Calculs pour la base ST */
  /***************************/

  bool res_sfst;
  bool res_polst;
  bool res_bpolst;
  bool res_tlst;
  bool res_pol2st;
  bool res_bpol2st;
  bool res_upolbpol2st;
  bool res_tl2st;


  res_sfst = is_aperiodic_mono(morp->cayley, morp->green, NULL); // Apériodicité
  if (!res_sfst) {
    res_polst = false;
    res_bpolst = false;
    res_tlst = false;
    res_pol2st = false;
    res_upolbpol2st = false;
    res_bpol2st = false;
    res_tl2st = false;
  }
  else {
    res_polst = is_jsat_mono(morp->cayley, NULL); // Pol(ST)
    res_bpolst = res_polst || is_jtrivial_mono(morp->cayley, morp->green, NULL); // BPol(ST)
    res_tlst = res_bpolst || is_da_mono(morp->cayley, morp->green, NULL); // TL(ST)
    if (!res_tlst) {
      shell_compute_atorbs(morp);
    }
    res_pol2st = res_tlst || is_jsat_orbmono(morp->atorbs, "BPol(ST)", NULL);
    res_upolbpol2st = res_pol2st || is_upbp_mono(morp->atorbs, "BPol(ST)", NULL);
    res_bpol2st = res_pol2st || (res_upolbpol2st && is_knast_at_mono(morp->cayley, morp->green, NULL));
    res_tl2st = res_upolbpol2st || is_da_orbmono(morp->atorbs, "BPol(ST)", NULL);
  }

  fprintf(stdout, "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
  fprintf(stdout, "║                                          Concatenation hierarchies: membership tests summary                                        ║\n");
  fprintf(stdout, "╠═════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╦══════════════╣\n");
  fprintf(stdout, "║ Basis: ST   ║    Pol(ST)   ║   BPol(ST)   ║    TL(ST)    ║   Pol₂(ST)   ║   BPol₂(ST)  ║  UBPol₂(ST)  ║   TL₂(ST)    ║    SF(ST)    ║\n");
  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║             ");
  summary_answer(res_polst);
  summary_answer(res_bpolst);
  summary_answer(res_tlst);

  summary_answer(res_pol2st);

  summary_answer(res_bpol2st);

  summary_answer(res_upolbpol2st);

  summary_answer(res_tl2st);

  summary_answer(res_sfst);
  fprintf(stdout, "║\n");


  /***************************/
  /* Calculs pour la base DD */
  /***************************/



  // On initialise les paramètres avec ceux de la base ST
  bool res_sfdd = res_sfst;
  bool res_poldd = res_polst;
  bool res_bpoldd = res_bpolst;
  bool res_tldd = res_tlst;
  bool res_pol2dd = res_pol2st;
  bool res_upolbpol2dd = res_upolbpol2st;
  bool res_tl2dd = res_tl2st;

  // Les résultats ne seront différent de ceux pour la base ST que si le langage est dans SF et que le neutre n'a que le mot vide pour antécédent
  if (res_sfdd && !cayley_neutstrict(morp->cayley, morp->green)) {
    shell_compute_ddorbs(morp);
    res_poldd = res_polst || is_jsat_orbmono(morp->ddorbs, "DD", NULL);
    res_bpoldd = res_bpolst || res_poldd || is_knast_mono(morp->cayley, morp->green, NULL);
    res_tldd = res_tlst || res_bpoldd || is_da_orbmono(morp->ddorbs, "DD", NULL);
    if (!res_tldd) {
      shell_compute_bpddorbs(morp);
    }

    res_pol2dd = res_pol2st || res_tldd || is_jsat_orbmono(shell_get_bpddorbs(morp), "BPol(DD)", NULL);
    res_upolbpol2dd = res_upolbpol2st || res_pol2dd || is_upbp_mono(shell_get_bpddorbs(morp), "BPol(DD)", NULL);
    res_tl2dd = res_tl2st || res_upolbpol2dd || is_da_orbmono(shell_get_bpddorbs(morp), "BPol(DD)", NULL);
  }



  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║ Basis: DD   ║    Pol(DD)   ║   BPol(DD)   ║    TL(DD)    ║   Pol₂(DD)   ║   BPol₂(DD)  ║  UBPol₂(DD)  ║   TL₂(DD)    ║    SF(DD)    ║\n");
  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║             ");
  summary_answer(res_poldd);
  summary_answer(res_bpoldd);
  summary_answer(res_tldd);

  summary_answer(res_pol2dd);

  if (res_pol2dd || res_bpol2st) {
    summary_answer(res_pol2dd || res_bpol2st);

  }
  else if (!res_upolbpol2dd) {
    summary_answer(res_upolbpol2dd);

  }
  else if (cayley_neutstrict(morp->cayley, morp->green)) {
    summary_answer(res_bpol2st);
  }
  else {
    fprintf(stdout, "║  Unavailable ");


  }


  summary_answer(res_upolbpol2dd);

  summary_answer(res_tl2dd);

  summary_answer(res_sfdd);
  fprintf(stdout, "║\n");


  /***************************/
  /* Calculs pour la base GR */
  /***************************/





  // On initialise les paramètres avec ceux de la base ST
  bool res_sfgr = res_sfst;
  bool res_polgr = res_polst;
  bool res_bpolgr = res_bpolst;
  bool res_tlgr = res_tlst;
  bool res_pol2gr = res_pol2st;
  bool res_upolbpol2gr = res_upolbpol2st;
  bool res_tl2gr = res_tl2st;


  shell_compute_gker(morp);
  res_sfgr = res_sfst || is_aperiodic_submono(morp->gker, "GR-kernel", NULL); // Apériodicité
  if (res_sfgr) {
    res_polgr = is_jsat_submono(morp->gker, "GR-kernel", NULL);// Pol(GR)
    res_bpolgr = res_polgr || is_blockg_mono(morp->cayley, morp->green, NULL);
    res_tlgr = res_bpolgr || is_da_submono(morp->gker, "GR-kernel", NULL);
    if (!res_tlgr) {
      shell_compute_bpgorbs(morp);
    }
    res_pol2gr = res_tlgr || is_jsat_orbmono(morp->bpgorbs, "BPol(GR)", NULL);
    res_upolbpol2gr = res_pol2gr || is_upbp_mono(morp->bpgorbs, "BPol(GR)", NULL);
    res_tl2gr = res_upolbpol2gr || is_da_orbmono(morp->bpgorbs, "BPol(GR)", NULL);

  }












  /****************************/
  /* Calculs pour la base MOD */
  /****************************/







  // On initialise les paramètres avec ceux de la base ST
  bool res_polmod = res_polst;
  bool res_bpolmod = res_bpolst;
  bool res_tlmod = res_tlst;
  bool res_pol2mod = res_pol2st;
  bool res_upolbpol2mod = res_upolbpol2st;
  bool res_tl2mod = res_tl2st;
  bool res_sfmod = res_sfst;

  if (!res_sfst && res_sfgr && !morp->cayley->neutlet) {
    shell_compute_mker(morp);
    res_sfmod = is_aperiodic_submono(morp->mker, "MOD-kernel", NULL);
  }
  // printf("survived\n");

  if (res_sfmod) {
    shell_compute_mker(morp);
    res_polmod = res_polgr && is_jsat_submono(morp->mker, "MOD-kernel", NULL);
    res_tlmod = res_polmod || (res_tlgr && is_da_submono(morp->mker, "MOD-kernel", NULL));
    res_bpolmod = res_polmod || (res_bpolgr && res_tlmod && is_bpolmod_mono(morp->cayley, morp->green, NULL));
    if (!res_tlmod) {
      shell_compute_bpmorbs(morp);
    }
    res_pol2mod = res_tlmod || (res_pol2gr && is_jsat_orbmono(shell_get_bpmorbs(morp), "BPol(MOD)", NULL));
    res_upolbpol2mod = res_pol2mod || (res_upolbpol2gr && is_upbp_mono(shell_get_bpmorbs(morp), "BPol(MOD)", NULL));
    res_tl2mod = res_upolbpol2mod || (res_tl2gr && is_da_orbmono(shell_get_bpmorbs(morp), "BPol(MOD)", NULL));


  }









  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║ Basis: MOD  ║   Pol(MOD)   ║  BPol(MOD)   ║   TL(MOD)    ║  Pol₂(MOD)   ║  BPol₂(MOD)  ║ UBPol₂(MOD)  ║  TL₂(MOD)    ║   SF(MOD)    ║\n");
  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║             ");
  summary_answer(res_polmod);
  summary_answer(res_bpolmod);
  summary_answer(res_tlmod);

  summary_answer(res_pol2mod);

  if (res_pol2mod || res_bpol2st) {
    summary_answer(res_pol2mod || res_bpol2st);

  }
  else if (!res_upolbpol2mod) {
    summary_answer(res_upolbpol2mod);

  }
  else if (morp->cayley->neutlet) {
    summary_answer(res_bpol2st);
  }
  else {
    fprintf(stdout, "║  Unavailable ");
  }


  summary_answer(res_upolbpol2mod);

  summary_answer(res_tl2mod);

  summary_answer(res_sfmod);
  fprintf(stdout, "║\n");

  // Calculs pour la base MODP




  bool res_polmodp = res_polmod;
  bool res_bpolmodp = res_bpolmod;
  bool res_tlmodp = res_tlmod;
  bool res_pol2modp = res_pol2mod;
  bool res_upolbpol2modp = res_upolbpol2mod;
  bool res_tl2modp = res_tl2mod;
  bool res_sfmodp = res_sfmod;






  if (res_sfmod && !cayley_neutstrict(morp->cayley, morp->green)) {
    shell_compute_mporbs(morp);
    res_polmodp = res_polmod || is_jsat_orbmono(morp->mporbs, "MOD⁺", NULL);
    res_bpolmodp = res_bpolmod || res_polmodp || is_qknast_mono(morp->mker, NULL);
    res_tlmodp = res_tlmod || res_bpolmodp || is_da_orbmono(morp->mporbs, "MOD⁺", NULL);
    if (!res_tlmodp) {
      shell_compute_bpmporbs(morp);
    }

    res_pol2modp = res_pol2mod || res_tlmodp || is_jsat_orbmono(shell_get_bpmporbs(morp), "BPol(MOD⁺)", NULL);
    res_upolbpol2modp = res_upolbpol2mod || res_pol2modp || is_upbp_mono(shell_get_bpmporbs(morp), "BPol(MOD⁺)", NULL);
    res_tl2modp = res_tl2mod || res_upolbpol2modp || is_da_orbmono(shell_get_bpmporbs(morp), "BPol(MOD⁺)", NULL);

  }








  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║ Basis: MOD⁺ ║   Pol(MOD⁺)  ║  BPol(MOD⁺)  ║   TL(MOD⁺)   ║  Pol₂(MOD⁺)  ║  BPol₂(MOD⁺) ║ UBPol₂(MOD⁺) ║  TL₂(MOD⁺)   ║   SF(MOD⁺)   ║\n");
  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║             ");
  summary_answer(res_polmodp);
  summary_answer(res_bpolmodp);
  summary_answer(res_tlmodp);

  summary_answer(res_pol2modp);

  if (res_pol2modp || res_bpol2st) {
    summary_answer(res_pol2modp || res_bpol2st);

  }
  else if (!res_upolbpol2modp) {
    summary_answer(res_upolbpol2modp);

  }
  else if (morp->cayley->neutlet) {
    summary_answer(res_bpol2st);
  }
  else {
    fprintf(stdout, "║  Unavailable ");
  }


  summary_answer(res_upolbpol2modp);

  summary_answer(res_tl2modp);

  summary_answer(res_sfmodp);
  fprintf(stdout, "║\n");



  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║ Basis: GR   ║    Pol(GR)   ║   BPol(GR)   ║    TL(GR)    ║   Pol₂(GR)   ║   BPol₂(GR)  ║  UBPol₂(GR)  ║   TL₂(GR)    ║    SF(GR)    ║\n");
  fprintf(stdout, "╠═════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╬══════════════╣\n");
  fprintf(stdout, "║             ");
  summary_answer(res_polgr);
  summary_answer(res_bpolgr);
  summary_answer(res_tlgr);

  summary_answer(res_pol2gr);

  if (res_pol2gr || res_bpol2st) {
    summary_answer(res_pol2gr || res_bpol2st);

  }
  else if (!res_upolbpol2gr) {
    summary_answer(res_upolbpol2gr);

  }
  else {
    fprintf(stdout, "║  Unavailable ");
  }


  summary_answer(res_upolbpol2gr);

  summary_answer(res_tl2gr);

  summary_answer(res_sfgr);
  fprintf(stdout, "║\n");

  fprintf(stdout, "╚═════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╩══════════════╝\n");

  return 1;
}

/************************/
/* Negation hierarchies */
/************************/

static void make_subscript_index(uint n, char* message) {
  if (n != 0) {
    uint d = n % 10;
    make_subscript_index(n / 10, message);
    switch (d) {
    case 0:
      strcat(message, "₀");
      break;
    case 1:
      strcat(message, "₁");
      break;
    case 2:
      strcat(message, "₂");
      break;
    case 3:
      strcat(message, "₃");
      break;
    case 4:
      strcat(message, "₄");
      break;
    case 5:
      strcat(message, "₅");
      break;
    case 6:
      strcat(message, "₆");
      break;
    case 7:
      strcat(message, "₇");
      break;
    case 8:
      strcat(message, "₈");
      break;
    case 9:
      strcat(message, "₉");
      break;
    default:
      break;
    }
  }
}

static void make_superscript_index(uint n, char* message) {
  if (n != 0) {
    uint d = n % 10;
    make_subscript_index(n / 10, message);
    switch (d) {
    case 0:
      strcat(message, "⁰");
      break;
    case 1:
      strcat(message, "¹");
      break;
    case 2:
      strcat(message, "²");
      break;
    case 3:
      strcat(message, "³");
      break;
    case 4:
      strcat(message, "⁴");
      break;
    case 5:
      strcat(message, "⁵");
      break;
    case 6:
      strcat(message, "⁶");
      break;
    case 7:
      strcat(message, "⁷");
      break;
    case 8:
      strcat(message, "⁸");
      break;
    case 9:
      strcat(message, "⁹");
      break;
    default:
      break;
    }
  }
}

bool shell_neghiera(com_parameters* pars) {
  // Il doit y avoir deux arguments:
  // - la classe de base
  // - la variable qui donne le langage ou le morphisme input
  if (com_nbparams(pars) != 2) {
    shell_arguments_error();
    return false;
  }

  // On commence par récupérer la variable (argment 2)
  int i = index_from_string_chain(pars->next->param->main);

  if (i == -1) {
    shell_variable_error();
    return false;
  }

  // On vérifie que l'input n'est pas un automate
  object_type thetype = objects[i]->type;
  if (thetype == AUTOMATA) {
    shell_membauto_error();
    return false;
  }

  // On récupère le morphisme associé à l'input
  morphism* morp = shell_get_tiedmor(i);
  // On aura besoin de ses relation de Green
  shell_compute_green(morp);

  // On prend le nom de la classe
  com_keyword key = key_from_string_chain_single(pars->param->main);

  // short level;
  bool inunion;
  switch (key) {
  case CL_ST:
    print_infooper_neghiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL", thetype);
    inunion = is_da_mono(morp->cayley, morp->green, stdout);
    print_conclusion_comp(stdout, inunion, "TL", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(ST) and MPol.\n\n");
      p_ufind C = parti_to_ufind(morp->green->JCL);
      uint level = hdet_mpol_level(morp->cayley, C, stdout);
      delete_ufind(C);
      char subscript[10];
      subscript[0] = '\0';
      make_subscript_index(level, subscript);
      char message[150];
      sprintf(message, "Conclusion: the least level containing the input is TL%s.", subscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_DD:
    print_infooper_neghiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(DD)", thetype);
    shell_compute_ddorbs(morp);
    inunion = is_da_orbmono(morp->ddorbs, "DD", stdout);
    print_conclusion_comp(stdout, inunion, "TL(DD)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(DD).\n");
      fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(DD) and MPol.\n\n");
      p_ufind C = iden_knast_mono(morp->cayley, morp->green);
      uint level = hdet_mpol_level(morp->cayley, C, stdout);
      delete_ufind(C);
      char subscript[10];
      subscript[0] = '\0';
      make_subscript_index(level, subscript);
      char message[150];
      sprintf(message, "Conclusion: the least level containing the input is TL%s(DD).", subscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_MOD:
    print_infooper_neghiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(MOD)", thetype);
    shell_compute_mker(morp);
    inunion = is_da_submono(morp->mker, "MOD-kernel", stdout);
    print_conclusion_comp(stdout, inunion, "TL(MOD)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(MOD).\n");
      fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(MOD) and MPol.\n\n");
      p_ufind C = iden_bpolmod_mono(morp->cayley, morp->green);
      uint level = hdet_mpol_level(morp->cayley, C, stdout);
      delete_ufind(C);
      char subscript[10];
      subscript[0] = '\0';
      make_subscript_index(level, subscript);
      char message[150];
      sprintf(message, "Conclusion: the least level containing the input is TL%s(MOD).", subscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_MODP:
    print_infooper_neghiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(MOD⁺)", thetype);
    shell_compute_mporbs(morp);
    inunion = is_da_orbmono(morp->mporbs, "MOD⁺", stdout);
    print_conclusion_comp(stdout, inunion, "TL(MOD⁺)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(MOD⁺).\n");
      fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(MOD⁺) and MPol.\n\n");
      p_ufind C = iden_qknast_mono(morp->cayley, morp->green, morp->mker);
      uint level = hdet_mpol_level(morp->cayley, C, stdout);
      delete_ufind(C);
      char subscript[10];
      subscript[0] = '\0';
      make_subscript_index(level, subscript);
      char message[150];
      sprintf(message, "Conclusion: the least level containing the input is TL%s(MOD⁺).", subscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_GR:
    print_infooper_neghiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(GR)", thetype);
    fprintf(stdout, "#### Checking if the GR-kernel belongs to DA.\n");
    shell_compute_gker(morp);
    inunion = is_da_submono(morp->gker, "GR-kernel", stdout);
    print_conclusion_comp(stdout, inunion, "TL(GR)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least n such that the input belongs to TLₙ(GR).\n");
      fprintf(stdout, "#### The algorithm is based on the characterizationss of BPol(GR) and MPol.\n\n");
      p_ufind C = iden_blockg_mono(morp->cayley, morp->green);
      uint level = hdet_mpol_level(morp->cayley, C, stdout);
      delete_ufind(C);
      char subscript[10];
      subscript[0] = '\0';
      make_subscript_index(level, subscript);
      char message[150];
      sprintf(message, "Conclusion: the least level containing the input is TL%s(GR).", subscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }
  return true;
}

bool shell_fphiera(com_parameters* pars) {
  // Il doit y avoir deux arguments:
  // - la classe de base
  // - la variable qui donne le langage ou le morphisme input
  if (com_nbparams(pars) != 2) {
    shell_arguments_error();
    return false;
  }

  // On commence par récupérer la variable (argment 2)
  int i = index_from_string_chain(pars->next->param->main);

  if (i == -1) {
    shell_variable_error();
    return false;
  }


  // On vérifie que l'input n'est pas un automate
  object_type thetype = objects[i]->type;
  if (thetype == AUTOMATA) {
    shell_membauto_error();
    return false;
  }

  // On récupère le morphisme associé à l'input
  morphism* morp = shell_get_tiedmor(i);
  // On aura besoin de ses relation de Green
  shell_compute_green(morp);

  // On prend le nom de la classe
  com_keyword key = key_from_string_chain_single(pars->param->main);

  // short level;
  short minf;
  short minp;
  bool inunion;
  switch (key) {
  case CL_ST:
    print_infooper_fphiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL", thetype);
    inunion = is_da_mono(morp->cayley, morp->green, stdout);
    print_conclusion_comp(stdout, inunion, "TL", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least level FLⁿ and PLⁿ containing the input.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
      p_ufind C = parti_to_ufind(morp->green->JCL);
      hdet_lrpol_level(morp->cayley, C, stdout, &minf, &minp);
      delete_ufind(C);
      char fscript[10];
      char pscript[10];
      fscript[0] = '\0';
      pscript[0] = '\0';
      make_superscript_index(minf, fscript);
      make_superscript_index(minp, pscript);
      char message[150];
      sprintf(message, "Conclusion: the least levels containing the input are FL%s and PL%s", fscript, pscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;

  case CL_DD:
    print_infooper_fphiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(DD)", thetype);
    shell_compute_ddorbs(morp);
    inunion = is_da_orbmono(morp->ddorbs, "DD", stdout);
    print_conclusion_comp(stdout, inunion, "TL(DD)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least level FLⁿ(DD) and PLⁿ(DD) containing the input.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
      p_ufind C = iden_knast_mono(morp->cayley, morp->green);
      hdet_lrpol_level(morp->cayley, C, stdout, &minf, &minp);
      delete_ufind(C);
      char fscript[10];
      char pscript[10];
      fscript[0] = '\0';
      pscript[0] = '\0';
      make_superscript_index(minf, fscript);
      make_superscript_index(minp, pscript);
      char message[150];
      sprintf(message, "Conclusion: the least levels containing the input are FL%s(DD) and PL%s(DD)", fscript, pscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }

    break;
  case CL_MOD:
    print_infooper_fphiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(MOD)", thetype);
    shell_compute_mker(morp);
    inunion = is_da_submono(morp->mker, "MOD-kernel", stdout);
    print_conclusion_comp(stdout, inunion, "TL(MOD)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least level FLⁿ(MOD) and PLⁿ(MOD) containing the input.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
      p_ufind C = iden_bpolmod_mono(morp->cayley, morp->green);
      hdet_lrpol_level(morp->cayley, C, stdout, &minf, &minp);
      delete_ufind(C);
      char fscript[10];
      char pscript[10];
      fscript[0] = '\0';
      pscript[0] = '\0';
      make_superscript_index(minf, fscript);
      make_superscript_index(minp, pscript);
      char message[150];
      sprintf(message, "Conclusion: the least levels containing the input are FL%s(MOD) and PL%s(MOD)", fscript, pscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_MODP:
    print_infooper_fphiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(MOD⁺)", thetype);
    shell_compute_mporbs(morp);
    inunion = is_da_orbmono(morp->mporbs, "MOD⁺", stdout);
    print_conclusion_comp(stdout, inunion, "TL(MOD⁺)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least level FLⁿ(MOD⁺) and PLⁿ(MOD⁺) containing the input.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
      p_ufind C = iden_qknast_mono(morp->cayley, morp->green, morp->mker);
      hdet_lrpol_level(morp->cayley, C, stdout, &minf, &minp);
      delete_ufind(C);
      char fscript[10];
      char pscript[10];
      fscript[0] = '\0';
      pscript[0] = '\0';
      make_superscript_index(minf, fscript);
      make_superscript_index(minp, pscript);
      char message[150];
      sprintf(message, "Conclusion: the least levels containing the input are FL%s(MOD⁺) and PL%s(MOD⁺)", fscript, pscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  case CL_GR:
    print_infooper_fphiera(key, stdout);
    print_info_input(i, stdout);
    print_start_comp(stdout, "TL(GR)", thetype);
    fprintf(stdout, "#### Checking if the GR-kernel belongs to DA.\n");
    shell_compute_gker(morp);
    inunion = is_da_submono(morp->gker, "GR-kernel", stdout);
    print_conclusion_comp(stdout, inunion, "TL(GR)", thetype);
    if (inunion) {
      fprintf(stdout, "#### We can now determine the least level FLⁿ(GR) and PLⁿ(GR) containing the input.\n");
      fprintf(stdout, "#### The algorithm is based on the characterizations of LPol and RPol.\n\n");
      p_ufind C = iden_blockg_mono(morp->cayley, morp->green);
      hdet_lrpol_level(morp->cayley, C, stdout, &minf, &minp);
      delete_ufind(C);
      char fscript[10];
      char pscript[10];
      fscript[0] = '\0';
      pscript[0] = '\0';
      make_superscript_index(minf, fscript);
      make_superscript_index(minp, pscript);
      char message[150];
      sprintf(message, "Conclusion: the least levels containing the input are FL%s(GR) and PL%s(GR)", fscript, pscript);
      print_dtitle_box(100, true, stdout, 1, message);
    }
    break;
  default:
    shell_membunav_error();
    return false;
    break;
  }

  return true;
}

/********************************/
/* Generic separation procedure */
/********************************/

// Message d'erreur pour les automates
static void shell_membmorp_error(void) {
  fprintf(stderr, "Error: Separation is not supported for input morphisms.\n");
}

// Message d'erreur pour les tests non-disponibles
static void shell_sepunav_error(void) {
  fprintf(stderr, "Error: Separation is not supported for this class.\n");
}


static void shell_display_inputs(p_nfa A1, p_nfa A2, FILE* out) {
  fprintf(out, "#### Displaying NFAs recognizing the two inputs.\n");
  fprintf(out, "#### First input language.\n");
  view_nfa(A1);
  fprintf(out, "#### Second input language.\n");
  view_nfa(A2);
}


// Procédure principale
bool shell_separation_test(com_parameters* pars) {

  int n = com_nbparams(pars);
  // Il doit y avoir entre trois et cinq arguments:
  // - un ou deux pour la classe (classe unique ou opérateur + input)
  // - les deux variables qui donnent les langages ou les automates pris en input
  // - un argument éventuel pour demander des détails
  if (n < 3 || n > 5) {
    shell_arguments_error();
    return false;
  }

  bool details = false;
  uchar nb_params = 3;
  com_keyword dets = key_from_string_chain_single(com_getparam(pars, n - 1)->main);
  if (dets == CM_INFO) {
    details = true;
    nb_params = n - 1;
  }
  else {
    details = false;
    nb_params = n;
  }
  if (n == 5) {
    shell_arguments_error();
    return false;
  }


  // On commence par récupérer les deux variables (deux derniers arguments)
  int i1 = index_from_string_chain(com_getparam(pars, nb_params - 2)->main);
  int i2 = index_from_string_chain(com_getparam(pars, nb_params - 1)->main);;

  if (i1 == -1 || i2 == -1) {
    shell_variable_error();
    return false;
  }

  // On calcule maintenant deux automates correspondant aux inputs
  p_nfa A1;
  p_nfa A2;
  switch (objects[i1]->type)
  {
  case AUTOMATA:
    A1 = nfa_elimeps(objects[i1]->theob.aut->nfa);
    nfa_delete_names(A1);
    break;
  case LANGUAGE:
    if (objects[i1]->theob.lan->type == SPE_REG) {
      A1 = reg2nfa(objects[i1]->theob.lan->spe.reg);
    }
    else {//if (objects[i1]->theob.lan->type == SPE_NFA) {
      A1 = nfa_elimeps(objects[objects[i1]->theob.lan->spe.nfa]->theob.aut->nfa);
    }

    nfa_elimeps_mod(A1);
    nfa_delete_names(A1);
    break;
  case MORPHISM:
    shell_membmorp_error();
    return false;
    break;
  default:
    break;
  }
  switch (objects[i2]->type)
  {
  case AUTOMATA:
    A2 = nfa_elimeps(objects[i2]->theob.aut->nfa);
    nfa_delete_names(A2);
    break;
  case LANGUAGE:
    if (objects[i2]->theob.lan->type == SPE_REG) {
      A2 = reg2nfa(objects[i2]->theob.lan->spe.reg);
    }
    else {//if (objects[i2]->theob.lan->type == SPE_NFA) {
      A2 = nfa_elimeps(objects[objects[i2]->theob.lan->spe.nfa]->theob.aut->nfa);
    }
    nfa_elimeps_mod(A2);
    nfa_delete_names(A2);
    break;
  case MORPHISM:
    shell_membmorp_error();
    return false;
    break;
  default:
    break;
  }

  // Le premier keyword
  com_keyword arg1 = key_from_string_chain_single(pars->param->main);

  // Si il n'y en a qu'un seul (3 arguments)
  if (nb_params == 3) {
    switch (arg1) {
    case CL_GR:
      print_dtitle_box(10, true, stdout, 1, "Separation: group languages (GR)");
      shell_display_inputs(A1, A2, stdout);
      print_sep_line(100, stdout);
      if (decid_grp_sep(A1, A2, details, stdout)) {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are GR-separable.");
      }
      else {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are NOT GR-separable.");
      }
      return true;
      break;
    case CL_MOD:
      print_dtitle_box(10, true, stdout, 1, "Separation: modulo languages (MOD)");
      shell_display_inputs(A1, A2, stdout);
      print_sep_line(100, stdout);
      if (decid_mod_sep(A1, A2, details, stdout)) {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are MOD-separable.");
      }
      else {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are NOT MOD-separable.");
      }
      return true;
      break;
    case CL_ST:
      print_dtitle_box(10, true, stdout, 1, "Separation: trivial languages (ST)");
      shell_display_inputs(A1, A2, stdout);
      print_sep_line(100, stdout);
      if (decid_st_sep(A1, A2, stdout)) {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are ST-separable.");
      }
      else {
        print_dtitle_box(10, true, stdout, 1, "Conclusion : the two inputs languages are NOT ST-separable.");
      }
      return true;
      break;
    default:
      delete_nfa(A1);
      delete_nfa(A2);
      shell_sepunav_error();
      return false;
      break;
    }
  }
  else {
    // Le second keyword
    com_keyword arg2 = key_from_string_chain_single(pars->next->param->main);
    switch (arg1)
    {
    case OP_POL:
      return shell_pol_sep(arg2, A1, A2, details);
      break;
    default:
      delete_nfa(A1);
      delete_nfa(A2);
      shell_sepunav_error();
      return false;
      break;
    }

  }

  delete_nfa(A1);
  delete_nfa(A2);
  return true;
}




// Pol-séparation
bool shell_pol_sep(com_keyword key, p_nfa A1, p_nfa A2, bool details) {
  switch (key) {
  case CL_ST:
    print_dtitle_box(10, true, stdout, 1, "Separation: positive piecewise testable languages (PPT = Pol(ST)).");
    shell_display_inputs(A1, A2, stdout);
    print_sep_line(100, stdout);
    if (decid_polst_sep(A1, A2, details, stdout)) {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is Pol(ST)-separable from the second one.");
    }
    else {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is NOT Pol(ST)-separable from the second one.");
    }
    break;
  case CL_MOD: {
    print_dtitle_box(
      10, true, stdout, 1, "Separation: polynomial closure of the modulo languages (Pol(MOD)).");
    shell_display_inputs(A1, A2, stdout);
    print_sep_line(100, stdout);
    if (decid_polmod_sep(A1, A2, details, stdout)) {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is Pol(MOD)-separable from the second one.");
    }
    else {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is NOT Pol(MOD)-separable from the second one.");
    }
    break;
  }
  case CL_GR: {
    print_dtitle_box(
      10, true, stdout, 1, "Separation: polynomial closure of the modulo languages (Pol(GR)).");
    shell_display_inputs(A1, A2, stdout);
    print_sep_line(100, stdout);
    if (decid_polgr_sep(A1, A2, details, stdout)) {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is Pol(GR)-separable from the second one.");
    }
    else {
      print_dtitle_box(10, true, stdout, 1,
        "Conclusion : the first input language is NOT Pol(GR)-separable from the second one.");
    }
    break;
  }
  default:
    shell_sepunav_error();
    return false;
    break;
  }
  return true;
}
