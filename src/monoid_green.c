/**********************************************/
/* Calcul et affichage des relations de Green */
/**********************************************/

#include "monoid_green.h"



/********************************/
/* Calcul à partir d'un monoide */
/********************************/

void jlr_green_compute(p_cayley M, p_green GREL) {
  if (GREL->RCL != NULL || GREL->LCL != NULL || GREL->JCL != NULL) {
    printf("Error, the relations L, R and J are already computed\n");
    return;
  }

  // Calcul des R-classes
  // p_graph rgraph = ldgraphs_to_graph(0, 0, 1, 1, M->trans);
  GREL->RCL = dtarjan(M->trans);

  // On commence par calculer (si besoin) le graphe gauche
  compute_left_cayley(M);
  // Calcul des L-classes
  // p_graph lgraph = ldgraphs_to_graph(0, 0, 1, 1, M->leftgraph);
  GREL->LCL = dtarjan(M->leftgraph);

  // Calcul des J-classes
  p_graph jgraph = ldgraphs_to_graph(0, 0, 2, 2, M->trans, M->leftgraph);
  GREL->JCL = tarjan(jgraph);
  delete_graph(jgraph);
}

// Calcule les relations L, J et R à partir dans un sous-monoide
// Les autres champs doivent déjà avoir été calculés (à utiliser
// si il n'y a pas de meilleure solution, quadratique)
void jlr_green_compute_sub(p_cayley M, p_green_sub S) {
  // On commence par calculer des Cayley droits et gauche du sous-monoide N (en
  // version non-étiquetée) On prend tout le monoide pour générateur
  p_graph cayr = create_graph_noedges(S->size);
  p_graph cayl = create_graph_noedges(S->size);
  p_graph cayj = create_graph_noedges(S->size);

  // Pour chaque élément q de N
  for (uint q = 0; q < S->size; q++) {
    // Initialisation des tableaux qui mémorisent les élements dans qN et Nq
    bool rmult[S->size];
    bool lmult[S->size];
    for (uint s = 0; s < S->size; s++) {
      rmult[s] = false;
      lmult[s] = false;
    }
    // Calcul des éléments dans qN et Nq
    for (uint r = 0; r < S->size; r++) {
      rmult[S->mono_to_sub[cayley_mult(M, S->sub_to_mono[q],
        S->sub_to_mono[r])]] = true;
      lmult[S->mono_to_sub[cayley_mult(M, S->sub_to_mono[r],
        S->sub_to_mono[q])]] = true;
    }
    for (uint s = 0; s < S->size; s++) {
      if (rmult[s]) {
        rigins_vertices(s, cayr->edges[q]);
      }
      if (lmult[s]) {
        rigins_vertices(s, cayl->edges[q]);
      }
      if (rmult[s] || lmult[s]) {
        rigins_vertices(s, cayj->edges[q]);
      }
    }
  }

  MALLOC(S->rels, 1);

  // Calcul des R-classes
  S->rels->RCL = tarjan(cayr);
  delete_graph(cayr);

  // Calcul des L-classes
  S->rels->LCL = tarjan(cayl);
  delete_graph(cayl);

  // Calcul des J-classes
  S->rels->JCL = tarjan(cayj);
  delete_graph(cayj);
}

// Calcule la relation J à partir de L et R
// Attention, cette approche ne donne pas un classement par ordre topologique
// des J-classes (pas assez d'info dans L et R)
void j_green_compute(p_green G) {
  // Initialisation
  MALLOC(G->JCL, 1);
  G->JCL->size_set = G->LCL->size_set;
  MALLOC(G->JCL->numcl, G->JCL->size_set);

  // Compteur qui comptera le nombre de J-classe
  uint num = 0;

  // Tableau qui mémorise les éléments déjà rangés dans une J-classe
  bool done[G->LCL->size_set];
  for (uint s = 0; s < G->LCL->size_set; s++) {
    done[s] = false;
  }

  // Boucle sur toutes les L-classes
  for (uint c = 0; c < G->LCL->size_par; c++) {
    // Si cette L-classe n'a pas déjà été rangé dans une J-classe,
    // on a une nouvelle J-classe qui sera l'union des R-classes
    // intersectées par la L-classe c (on donne le numéro num à
    // tous ces éléments)
    if (!done[lefread_vertices(G->LCL->cl[c], 0)]) {
      // Boucle sur les éléments de la L-classe c
      for (uint j = 0; j < size_vertices(G->LCL->cl[c]); j++) {
        // Si cet élément n'a pas encore été traité
        if (!done[lefread_vertices(G->LCL->cl[c], j)]) {
          // On prend sa R-classe
          uint rc = G->RCL->numcl[lefread_vertices(G->LCL->cl[c], j)];
          // On donne le numéro de J-classe courant à tous les éléments de cette
          // R-classe et on les marque comme traités
          for (uint k = 0; k < size_vertices(G->RCL->cl[rc]); k++) {
            G->JCL->numcl[lefread_vertices(G->RCL->cl[rc], k)] = num;
            done[lefread_vertices(G->RCL->cl[rc], k)] = true;
          }
          // On marque cette R-classe comme traitée
        }
      }
      // On incrément le compteur de J-classes
      num++;
    }
  }
  // Création du tableau des J-classes
  MALLOC(G->JCL->cl, num);
  G->JCL->size_par = num;
  for (uint c = 0; c < num; c++) {
    G->JCL->cl[c] = create_vertices();
  }

  for (uint s = 0; s < G->JCL->size_set; s++) {
    rigins_vertices(s, G->JCL->cl[G->JCL->numcl[s]]);
  }
}

/* Tri rapide */
// Tri entre les indices i et j-1 (inclus)
static void aux_quick_sort_green(uint* array, uint i, uint j, uint* rnums,
  uint* lnums) {
  // printf("i: %d, j:%d\n", i, j);
  if (i + 1 >= j) { // Il y a moins d'un élément
    return;
  }
  else {
    uint pivot = array[i];
    uint l = j;

    for (uint k = j - 1; i < k; k--) {
      if (rnums[pivot] < rnums[array[k]] ||
        (rnums[pivot] == rnums[array[k]] && lnums[pivot] < lnums[array[k]])) {
        l--;
        uint temp = array[l];
        array[l] = array[k];
        array[k] = temp;
      }
    }
    uint temp = array[l - 1];
    array[l - 1] = array[i];
    array[i] = temp;

    aux_quick_sort_green(array, i, l - 1, rnums, lnums);
    aux_quick_sort_green(array, l, j, rnums, lnums);
  }
}

static void quick_sort_green(uint* array, uint size, uint* lnums, uint* rnums) {
  aux_quick_sort_green(array, 0, size, lnums, rnums);
}

// Calcule la relation H à partir de J, L et R (qui doivent déjà avoir été
// calculées)
void h_green_compute(p_green GREL) {
  if (GREL->RCL == NULL || GREL->LCL == NULL || GREL->JCL == NULL) {
    printf("Error. Cannot compute the H-classes without the J-classes, the "
      "L-classes and the R-classes\n");
    return;
  }

  if (GREL->HCL == NULL) {
    uint num = 0;

    // Calcul des H-classes
    MALLOC(GREL->HCL, 1);
    GREL->HCL->size_set = GREL->JCL->size_set;
    MALLOC(GREL->HCL->numcl, GREL->HCL->size_set);

    // Pour chaque J-classe
    for (uint cr = 0; cr < GREL->JCL->size_par; cr++) {
      // On enregistre les éléments de la J-classe dans un tableau
      uint thejclass[size_vertices(GREL->JCL->cl[cr])];
      for (uint i = 0; i < size_vertices(GREL->JCL->cl[cr]); i++) {
        thejclass[i] = lefread_vertices(GREL->JCL->cl[cr], i);
      }
      // On trie les éléments selon leut numéro de R-classe, puis de L-classe
      // (ordre lexicographique)
      quick_sort_green(thejclass, size_vertices(GREL->JCL->cl[cr]),
        GREL->RCL->numcl, GREL->LCL->numcl);

      // Les éléments d'une même H-classe sont maintenant contigus dans le
      // tableau
      uint elem = thejclass[0];
      GREL->HCL->numcl[elem] = num;
      num++;
      for (uint i = 1; i < size_vertices(GREL->JCL->cl[cr]); i++) {
        if (GREL->LCL->numcl[thejclass[i]] == GREL->LCL->numcl[elem] &&
          GREL->RCL->numcl[thejclass[i]] == GREL->RCL->numcl[elem]) {
          GREL->HCL->numcl[thejclass[i]] = GREL->HCL->numcl[elem];
        }
        else {
          GREL->HCL->numcl[thejclass[i]] = num;
          num++;
          elem = thejclass[i];
        }
      }
    }

    // On a maintenant le nombre de H-classes
    GREL->HCL->size_par = num;

    // Il reste à construire le tableau des classes
    MALLOC(GREL->HCL->cl, GREL->HCL->size_par);

    for (uint c = 0; c < GREL->HCL->size_par; c++) {
      GREL->HCL->cl[c] = create_vertices();
    }
    for (uint v = 0; v < GREL->HCL->size_set; v++) {
      rigins_vertices(v, GREL->HCL->cl[GREL->HCL->numcl[v]]);
    }
  }
}

// Calcule les informations sur les sur les Groupes et les J-classes régulière
// (J, L, R et H doivent avoir étét déjà calculées)
void gr_green_compute(p_vertices idem_list, p_green G) {
  // Il reste à remplir les listes des H-classes qui sont des groupes et des
  // J-classes régulières
  MALLOC(G->regular_array, G->HCL->size_set);
  G->regular_list = create_vertices();
  MALLOC(G->group_array, G->HCL->size_par);
  G->group_list = create_vertices();

  // Tableau temporaire contenant les J-classes régulières
  bool reg_temp[G->JCL->size_par];
  for (uint c = 0; c < G->JCL->size_par; c++) {
    reg_temp[c] = false;
  }

  // Initialisation du tableau marquant les groupes
  for (uint c = 0; c < G->HCL->size_par; c++) {
    G->group_array[c] = false;
  }

  // On remplit les tableaux de Booléens en regardant tous les idempotents
  for (uint i = 0; i < size_vertices(idem_list); i++) {
    // On prend l'idempotent
    uint e = lefread_vertices(idem_list, i);
    G->group_array[G->HCL->numcl[e]] = true;
    reg_temp[G->JCL->numcl[e]] = true;
  }

  // Construction de la liste des groupes
  for (uint c = 0; c < G->HCL->size_par; c++) {
    if (G->group_array[c]) {
      rigins_vertices(c, G->group_list);
    }
  }

  // Construction des listes d'éléments réguliers
  for (uint s = 0; s < G->JCL->size_set; s++) {
    if (reg_temp[G->JCL->numcl[s]]) {
      G->regular_array[s] = true;
      rigins_vertices(s, G->regular_list);
    }
  }
}

// Calcul complet des relations de Green
p_green cayley_green_compute(p_cayley M) {
  // On créé la structure à retourner
  p_green result;
  MALLOC(result, 1);

  result->JCL = NULL;
  result->LCL = NULL;
  result->RCL = NULL;
  result->HCL = NULL;
  result->group_list = NULL;
  result->group_array = NULL;

  // Calcul des relations J, R et L
  jlr_green_compute(M, result);

  // Calcul des H-classes
  h_green_compute(result);

  // Il reste à remplir les listes des H-classes qui sont des groupes et des
  // J-classes régulières
  gr_green_compute(M->idem_list, result);

  return result;
}

void delete_green(p_green G) {
  if (G == NULL) {
    return;
  }
  delete_vertices(G->group_list);
  free(G->group_array);
  delete_parti(G->HCL);
  delete_parti(G->RCL);
  delete_parti(G->LCL);
  delete_parti(G->JCL);
  free(G);
}



// Teste si le neutre a un antécédent non-vide
bool cayley_neutstrict(p_cayley M, p_green G) {
  // On teste si il y a une lettre J-équivalente à 1;
  return M->neutlet || size_vertices(G->HCL->cl[G->HCL->numcl[ONE]]) != 1;
}

/*************************/
/* Fonctions d'affichage */
/*************************/

void print_infos_green(p_green G, FILE* out) {
  fprintf(out, "#### Size of the monoid                 : %d\n",
    G->HCL->size_set);
  fprintf(out, "#### Number of idempotents              : %d\n",
    size_vertices(G->group_list));
  fprintf(out, "#### Number of regular elements         : %d\n",
    size_vertices(G->regular_list));
  fprintf(out, "#### Number of J-classes                : %d.\n",
    G->JCL->size_par);
  fprintf(out, "#### Number of L-classes                : %d.\n",
    G->LCL->size_par);
  fprintf(out, "#### Number of R-classes                : %d.\n",
    G->RCL->size_par);
  fprintf(out, "#### Number of H─classes                : %d.\n",
    G->HCL->size_par);
}

void print_jclass_green(p_cayley M, p_green G, uint jclass, uint padding, FILE* out) {
  // On prend un élément quelconque dans la H-classe
  uint elem = lefread_vertices(G->JCL->cl[jclass], 0);

  // Taille des h-classes dans jclass
  uint size_hc = size_vertices(G->HCL->cl[G->HCL->numcl[elem]]);
  // Taille des r-classes dans jclass
  uint size_rc = size_vertices(G->RCL->cl[G->RCL->numcl[elem]]);
  // Taille des l-classes dans jclass
  uint size_lc = size_vertices(G->LCL->cl[G->LCL->numcl[elem]]);

  uint row_length = size_rc / size_hc;
  uint col_length = size_lc / size_hc;

  // Taille maximale d'un nom dans la J-classe à afficher
  uint name_size = cayley_max_name_size(M, G->JCL->cl[jclass]);

  // Nombre d'éléments affichés par ligne dans une H-classe
  double db = sqrt(size_hc);
  uint num_elem_line = (uint)ceil(db);

  uint height_hclass;
  if (num_elem_line * (num_elem_line - 1) < size_hc) {
    height_hclass = num_elem_line;
  }
  else {
    height_hclass = num_elem_line - 1;
  }

  // Longueur d'une ligne dans une H-classe
  uint h_line_size = name_size * num_elem_line + num_elem_line + 3;

  // Tri de la j-classe dans un tableau
  uint thejclass[size_vertices(G->JCL->cl[G->JCL->numcl[elem]])];

  for (uint i = 0; i < size_vertices(G->JCL->cl[G->JCL->numcl[elem]]); i++) {
    thejclass[i] = lefread_vertices(G->JCL->cl[G->JCL->numcl[elem]], i);
  }
  quick_sort_green(thejclass, size_vertices(G->JCL->cl[G->JCL->numcl[elem]]), G->RCL->numcl, G->LCL->numcl);

  // Écriture de la première ligne
  print_spaces(padding, out);
  fprintf(out, "┌");
  for (uint i = 0; i < row_length; i++) {
    print_char(h_line_size, "─", out);
    if (i == row_length - 1) {
      fprintf(out, "┐\n");
    }
    else {
      fprintf(out, "┬");
    }
  }

  // écritures des classes
  for (uint j = 0; j < col_length; j++) // Boucle sur les L-classes
  {
    print_spaces(padding, out);
    // Top ligne vide
    fprintf(out, "│");
    for (uint i = 0; i < row_length; i++) {
      uint s = thejclass[(j * size_rc) + i * size_hc];

      if (G->group_array[G->HCL->numcl[s]]) {
        print_spaces(h_line_size - 1, out);
        fprintf(out, "*");
      }
      else {
        print_spaces(h_line_size, out);
      }
      if (i == row_length - 1) {
        fprintf(out, "│\n");
      }
      else {
        fprintf(out, "│");
      }
    }

    for (uint k = 0; k < height_hclass; k++) {
      print_spaces(padding, out);
      for (uint i = 0; i < row_length; i++) {
        fprintf(out, "│  ");
        for (uint h = 0; h < num_elem_line; h++) {
          uint count = k * (num_elem_line)+h;
          if (count < size_hc) {

            cayley_print_name_aligned(M, thejclass[(j * size_rc) + i * size_hc + count], name_size, out);


          }
          else {
            print_spaces(name_size, out);
          }

          fprintf(out, " ");
        }
        fprintf(out, " ");
      }
      fprintf(out, "│\n");
    }

    print_spaces(padding, out);
    // Bottom ligne vide
    fprintf(out, "│");
    for (uint i = 0; i < row_length; i++) {
      print_char(h_line_size, " ", out);
      if (i == row_length - 1) {
        fprintf(out, "│\n");
      }
      else {
        fprintf(out, "│");
      }
    }
    print_spaces(padding, out);
    if (j == col_length - 1) {
      // Écriture de la dernière ligne
      fprintf(out, "└");
      for (uint i = 0; i < row_length; i++) {
        print_char(h_line_size, "─", out);
        if (i == row_length - 1) {
          fprintf(out, "┘\n");
        }
        else {
          fprintf(out, "┴");
        }
      }
    }
    else {
      // Écriture d'une ligne intermédiaire
      fprintf(out, "├");
      for (uint i = 0; i < row_length; i++) {
        print_char(h_line_size, "─", out);
        if (i == row_length - 1) {
          fprintf(out, "┤\n");
        }
        else {
          fprintf(out, "┼");
        }
      }
    }
  }
}

// Affichage complet des informations sur les relations de Green d'un monoide
void print_full_green(p_cayley M, p_green G, bool title, FILE* out) {
  if (title) {
    print_title_box(0, true, out, 1, "Green relations of the monoid.");
  }
  print_infos_green(G, out);

  if (M->trans->size_graph <= 1000) {

    fprintf(out, "#### The J-classes (ordered using a topological sort).\n");
    for (uint i = G->JCL->size_par; i > 0; i--) {
      print_jclass_green(M, G, i - 1, 5, out);
    }
  }
}


static void shell_print_alphabet(uint n) {
  fprintf(stdout, "{a");
  for (uint i = 1; i < n;i++) {
    fprintf(stdout, ",%c", 'a' + i);
  }
  fprintf(stdout, "}\n");
}


void view_morphism(cayley* cayley, green* G) {
  fprintf(stdout, "#### Alphabet  : ");
  shell_print_alphabet(cayley->trans->size_alpha);
  fprintf(stdout, "#### Monoid    :\n");
  print_full_green(cayley, G, false, stdout);
  fprintf(stdout, "#### Morphism  :\n");
  cayley_print_morphism(cayley, stdout);
}


/**********************************/
/* Manipulation des sous-monoides */
/**********************************/


// Initialisation d'un p_green_sub à partir d'un morphisme
// À la création le sous-monoide est égal au monoide original
p_green_sub init_submono(p_cayley M, p_green G) {
  p_green_sub S;
  MALLOC(S, 1);
  S->original = M;
  S->originalrels = G;
  S->size = M->trans->size_graph;
  S->neut = ONE;

  // Les uatres champs ne sont pas utilisés
  S->mono_in_sub = NULL;
  S->mono_to_sub = NULL;
  S->idem_array = NULL;
  S->idem_list = NULL;
  S->sub_to_mono = NULL;
  S->rels = NULL;
  return S;
}

// Suppression d'un sous-monoide
// Ne supprime pas le monoide original
void delete_green_sub(p_green_sub S) {
  if (S == NULL) {
    return;
  }
  free(S->mono_in_sub);
  free(S->mono_to_sub);
  free(S->sub_to_mono);
  free(S->idem_array);
  delete_vertices(S->idem_list);
  delete_green(S->rels);
  free(S);
}


// Pour une liste d'éléments du monoide d'origine, retourne la liste obtenue en
// ne gardant que ceux présents dans le sous-monoide
p_vertices restrict_list_to_submono(p_green_sub S, p_vertices l) {
  p_vertices result = create_vertices();
  for (uint i = 0; i < size_vertices(l); i++) {
    if (S->mono_in_sub == NULL || S->mono_in_sub[lefread_vertices(l, i)]) {
      rigins_vertices(lefread_vertices(l, i), result);
    }
  }

  return result;
}

// Idempotence
bool is_idem_submono(p_green_sub S, FILE* out) {

  // On prend le monoide original
  p_cayley M = S->original;

  // Cas particulier où le sous-monoide est le monoide entier 
  if (S->mono_in_sub == NULL) {
    return is_idem_mono(M, out);
  }

  // Sinon
  uint i = 0;
  uint s = 0;
  while (s < S->size) {
    if (S->sub_to_mono[s] > lefread_vertices(M->idem_list, i)) {
      i++;
    }
    else if (S->sub_to_mono[s] == lefread_vertices(M->idem_list, i)) {
      i++;
      s++;
    }
    else {
      fprintf(out, "#### The monoid is NOT idempotent.\n");
      fprintf(out, "#### For instance, ");
      cayley_print_name(M, S->sub_to_mono[s], out);
      fprintf(out, " is not an idempotent\n");
      return false;
    }
  }
  fprintf(out, "#### The monoid is idempotent.\n");
  return true;
}

// Commutativité
bool is_comm_submono(p_green_sub S, FILE* out) {
  // On prend le monoide original
  p_cayley M = S->original;

  // Cas particulier où le sous-monoide est le monoide entier 
  if (S->mono_in_sub == NULL) {
    return is_comm_mono(M, out);
  }

  if (out) {
    fprintf(out, "#### Checking if the monoid is commutative.\n");
  }

  // Sinon
  for (uint s = 0; s < S->size - 1; s++) {
    for (uint t = s + 1; t < S->size; t++) {

      uint st = cayley_mult(M, S->sub_to_mono[s], S->sub_to_mono[t]);
      uint ts = cayley_mult(M, S->sub_to_mono[t], S->sub_to_mono[s]);
      if (st != ts) {
        fprintf(out, "#### The monoid is NOT commutative.\n");
        fprintf(out, "#### For instance, ");
        cayley_print_name(M, S->sub_to_mono[s], out);
        fprintf(out, " · ");
        cayley_print_name(M, S->sub_to_mono[t], out);
        fprintf(out, " ≠ ");
        cayley_print_name(M, S->sub_to_mono[t], out);
        fprintf(out, " · ");
        cayley_print_name(M, S->sub_to_mono[s], out);
        fprintf(out, ".\n");
        return false;
      }
    }
  }
  fprintf(out, "#### The monoid is commutative.\n");
  return true;
}

// Affichage du diagramme boite à oeufs (©TM JEP) pour les sous-monoïdes
void print_jclass_green_sub(p_green_sub S, uint jclass, uint padding, FILE* out) {


  if (S->mono_in_sub == NULL) {
    fprintf(stderr, "Green, should not happen\n");
    return;
  }
  // On prend le morphisme original
  p_cayley M = S->original;

  // On prend un élément quelconque dans la H-classe
  uint elem = lefread_vertices(S->rels->JCL->cl[jclass], 0);

  // Taille des h-classes dans jclass
  uint size_hc = size_vertices(S->rels->HCL->cl[S->rels->HCL->numcl[elem]]);
  // Taille des r-classes dans jclass
  uint size_rc = size_vertices(S->rels->RCL->cl[S->rels->RCL->numcl[elem]]);
  // Taille des l-classes dans jclass
  uint size_lc = size_vertices(S->rels->LCL->cl[S->rels->LCL->numcl[elem]]);

  uint row_length = size_rc / size_hc;
  uint col_length = size_lc / size_hc;


  // Calcul de la liste des éléments de la J-classe dans le monoide original
  p_vertices origelem = create_vertices();
  for (uint i = 0; i < size_vertices(S->rels->JCL->cl[jclass]); i++) {
    rigins_vertices(S->sub_to_mono[lefread_vertices(S->rels->JCL->cl[jclass], i)], origelem);
  }


  // Taille maximale d'un nom dans la J-classe à afficher
  uint name_size = cayley_max_name_size(M, origelem);
  // Nombre d'éléments affichés par ligne dans une H-classe
  double db = sqrt(size_hc);
  uint num_elem_line = (uint)ceil(db);



  uint height_hclass;
  if (num_elem_line * (num_elem_line - 1) < size_hc) {
    height_hclass = num_elem_line;
  }
  else {
    height_hclass = num_elem_line - 1;
  }

  // Longueur d'une ligne dans une H-classe
  uint h_line_size = name_size * num_elem_line + num_elem_line + 3;

  // Tri de la j-classe dans un tableau
  uint thejclass[size_vertices(S->rels->JCL->cl[S->rels->JCL->numcl[elem]])];
  for (uint i = 0;
    i < size_vertices(S->rels->JCL->cl[S->rels->JCL->numcl[elem]]); i++) {
    thejclass[i] =
      lefread_vertices(S->rels->JCL->cl[S->rels->JCL->numcl[elem]], i);
    // printf("numj: %d\n", thejclass[i]);
  }
  // printf("The size %d\n", size_vertices(result->JCL->cl[cr]));
  quick_sort_green(thejclass,
    size_vertices(S->rels->JCL->cl[S->rels->JCL->numcl[elem]]),
    S->rels->RCL->numcl, S->rels->LCL->numcl);

  // Écriture de la première ligne
  print_spaces(padding, out);
  fprintf(out, "┌");
  for (uint i = 0; i < row_length; i++) {
    print_char(h_line_size, "─", out);
    if (i == row_length - 1) {
      fprintf(out, "┐\n");
    }
    else {
      fprintf(out, "┬");
    }
  }

  // écritures des classes
  for (uint j = 0; j < col_length; j++) // Boucle sur les L-classes
  {
    print_spaces(padding, out);
    // Top ligne vide
    fprintf(out, "│");
    for (uint i = 0; i < row_length; i++) {
      uint s = thejclass[(j * size_rc) + i * size_hc];

      if (S->rels->group_array[S->rels->HCL->numcl[s]]) {
        print_spaces(h_line_size - 1, out);
        fprintf(out, "*");
      }
      else {
        print_spaces(h_line_size, out);
      }
      if (i == row_length - 1) {
        fprintf(out, "│\n");
      }
      else {
        fprintf(out, "│");
      }
    }

    for (uint k = 0; k < height_hclass; k++) {
      print_spaces(padding, out);
      for (uint i = 0; i < row_length; i++) {
        fprintf(out, "│  ");
        for (uint h = 0; h < num_elem_line; h++) {
          uint count = k * (num_elem_line)+h;
          if (count < size_hc) {
            cayley_print_name_aligned(M, S->sub_to_mono[thejclass[(j * size_rc) + i * size_hc + count]], name_size, out);
          }
          else {
            print_spaces(name_size, out);
          }

          fprintf(out, " ");
        }
        fprintf(out, " ");
      }
      fprintf(out, "│\n");
    }

    print_spaces(padding, out);
    // Bottom ligne vide
    fprintf(out, "│");
    for (uint i = 0; i < row_length; i++) {
      print_char(h_line_size, " ", out);
      if (i == row_length - 1) {
        fprintf(out, "│\n");
      }
      else {
        fprintf(out, "│");
      }
    }
    print_spaces(padding, out);
    if (j == col_length - 1) {
      // Écriture de la dernière ligne
      fprintf(out, "└");
      for (uint i = 0; i < row_length; i++) {
        print_char(h_line_size, "─", out);
        if (i == row_length - 1) {
          fprintf(out, "┘\n");
        }
        else {
          fprintf(out, "┴");
        }
      }
    }
    else {
      // Écriture d'une ligne intermédiaire
      fprintf(out, "├");
      for (uint i = 0; i < row_length; i++) {
        print_char(h_line_size, "─", out);
        if (i == row_length - 1) {
          fprintf(out, "┤\n");
        }
        else {
          fprintf(out, "┼");
        }
      }
    }
  }
  delete_vertices(origelem);
}




// Affichage complet des informations sur les relations de Green d'un
// sous-monoide
void print_full_green_sub(p_green_sub S, bool title, FILE* out) {
  if (S->mono_in_sub == NULL) {
    print_full_green(S->original, S->originalrels, false, out);
    return;
  }
  else {
    if (title) {
      print_title_box(0, true, out, 1, "Green relations of the monoid.");
    }
    print_infos_green(S->rels, out);
    if (S->size <= 1000) {


      fprintf(out, "#### The J-classes (ordered using a topological sort).\n");
      for (uint i = S->rels->JCL->size_par; i > 0; i--) {
        print_jclass_green_sub(S, i - 1, 5, out);
      }
    }
  }
}

// Affichage des idempotents
void submono_print_idems(p_green_sub S, FILE* out) {
  p_cayley M = S->original;
  if (S->mono_in_sub == NULL) {
    cayley_print_idems(M, out);
    return;
  }
  print_top_line(100, out);
  fprintf(out, "│Idempotents : ");
  p_vertices idemsinsub = restrict_list_to_submono(S, M->idem_list);
  print_sub_cayley_aligned(M, idemsinsub, 100, 14, out);
  print_bot_line(100, out);
  delete_vertices(idemsinsub);
}

// Affichage de l'ordre syntaxique
void submono_print_order(p_green_sub S, FILE* out) {
  p_cayley M = S->original;

  if (S->mono_in_sub == NULL) {
    cayley_print_order(M, out);
    return;
  }
  compute_syntac_order(M); // Calcul (si nécessaire de l'ordre)
  print_top_line(100, out);
  // print_title_box(100, false, out, 1, "Computation on the syntactic morphism.
  // Syntactic order.");

  // Calcul de la longueur maximale d'un nom
  uint size_max = 1;
  for (uint i = 0; i < S->size; i++) {
    size_max = max(size_max, size_vertices(M->names[S->sub_to_mono[i]]));
  }

  uint padding = 24 + size_max;

  for (uint i = 0; i < S->size; i++) {
    p_vertices elems = restrict_list_to_submono(S, M->order[S->sub_to_mono[i]]);
    fprintf(out, "│Elements larger than ");
    cayley_print_name_aligned(M, S->sub_to_mono[i], size_max, out);
    fprintf(out, " : ");
    print_sub_cayley_aligned(M, elems, 100, padding, out);
    delete_vertices(elems);
  }
  print_bot_line(100, out);
}



