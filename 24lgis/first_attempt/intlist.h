#ifndef INTLIST_H
#define INTLIST_H

#include <stdio.h>
#include <stdlib.h>

struct int_list 
{
   int count;
   int allocated;
   int *values;
};

struct int_list *new_int_list();
int il_get(struct int_list *il, int index);
int il_add(struct int_list *il, int i);
int il_free(struct int_list *il);
void il_clear(struct int_list *il);
void log_il(struct int_list *il);

#endif
