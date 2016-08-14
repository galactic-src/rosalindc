#ifndef BIGINT_H
#define BIGINT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT_TO_CHAR 48;

struct bigint
{
   int count;
   int alloc;
   unsigned char *nums; //starting with lowest value, holding 0-99
};

struct bigint *bigint_from_long(unsigned long n);
struct bigint *bi_clone(struct bigint *bi);
void log_bi(struct bigint *bi);
void free_bi(struct bigint *bi);
struct bigint *bi_add(struct bigint *bi1, struct bigint *bi2);
struct bigint *bi_mul(struct bigint *bi1, struct bigint *bi2);

#endif
