#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <stdio.h>
#include "intlist.h"
#include "str.h"

int rna_hash_value(char base);
int aa_hash(char c1, char c2, char c3);
char aa_at(char *from);

struct char_array_list *translate(struct char_array *mrna);

#endif
