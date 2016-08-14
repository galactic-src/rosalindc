#ifndef INTLIST_H
#define INTLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct int_list 
{
   int count;
   int allocated;
   int *values;
};

struct int_list *new_int_list();
int il_get(struct int_list *il, int index);
void il_add(struct int_list *il, int i);
void il_free(struct int_list *il);
void il_set(struct int_list *il, int ix, int i);
void il_clear(struct int_list *il);
void log_il(struct int_list *il);
struct int_list *il_clone(const struct int_list *src);
int il_contains(struct int_list *il, int needle);

#endif
