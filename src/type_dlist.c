/******************************/
/* Listes doublement chaînées */
/******************************/

#include "type_dlist.h"

/* Création d'une liste */
dlist* create_dlist(void)
{
    dlist* new;
    MALLOC(new, 1);
    MALLOC(new->lsent, 1);
    MALLOC(new->rsent, 1);
    new->size = 0; // La liste est initialement vide
    new->lsent->previous = NULL;
    new->lsent->next = new->rsent;
    new->rsent->previous = new->lsent;
    new->rsent->next = NULL;
    return new;
}

/* Suppression d'une liste */
void delete_dlist(dlist* p)
{
    if (p == NULL)
    {
        return;
    }
    dcell* c = p->lsent;
    dcell* d;
    while (c != NULL)
    {
        d = c;
        c = c->next;
        free(d);
    }
    free(p);
}

/* Insertion avant/après une cellue existante */
void insertnext_dlist(dlist* p, dcell* c, int newval)
{
    CHECK_NULL(4, p, "The list", p->lsent, "The left sentry", p->rsent, "The right sentry", c, "The input cell");
    if (c->next == NULL)
    {
        fprintf(stderr, "Error, cannot insert a cell after the right sentry");
        exit(EXIT_FAILURE);
    }
    dcell* d = c->next;
    MALLOC(c->next, 1);
    c->next->val = newval;
    c->next->previous = c;
    c->next->next = d;
    d->previous = c->next;
    p->size++;
}

void insertprevious_dlist(dlist* p, dcell* c, int newval)
{
    CHECK_NULL(4, p, "The list", p->lsent, "The left sentry", p->rsent, "The right sentry", c, "The input cell");
    if (c->previous == NULL)
    {
        fprintf(stderr, "Error, cannot insert a cell before the left sentry");
        exit(EXIT_FAILURE);
    }
    dcell* d = c->previous;
    MALLOC(c->previous, 1);
    c->previous->val = newval;
    c->previous->previous = d;
    c->previous->next = c;
    d->next = c->previous;
    p->size++;
}

/* Suppression d'une cellule */
void deletecell_dlist(dlist* p, dcell* c)
{
    CHECK_NULL(4, p, "The list", p->lsent, "La left sentry", p->rsent, "The right sentry", c, "The input cell");
    if (c->next == NULL || c->previous == NULL)
    {
        fprintf(stderr, "Error, cannot delete the sentries");
        exit(EXIT_FAILURE);
    }
    dcell* bc = c->previous;
    dcell* ac = c->next;
    free(c);
    bc->next = ac;
    ac->previous = bc;
    p->size--;
}

/* Fusion de deux listes (la second est concaténée à droite de la première) */
void concat_dlist(dlist* l1, dlist* l2)
{
    if (l1 != l2)
    {
        l1->rsent->previous->next = l2->lsent->next;
        l2->lsent->next->previous = l1->rsent->previous;
        free(l1->rsent);
        free(l2->lsent);
        l1->rsent = l2->rsent;
        l2->lsent = l1->lsent;

        l1->size = l1->size + l2->size;
        l2->size = l1->size;
    }
}
