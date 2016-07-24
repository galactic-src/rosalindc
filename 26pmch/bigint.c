#include <limits.h>
#include "bigint.h"

struct bigint *new_bigint()
{
   struct bigint *bi = malloc(sizeof(struct bigint));
   bi->count = 0;
   bi->alloc = 10;
   bi->nums = malloc(10);

   return bi;
}

static void bi_ensure_len(struct bigint *bi, int len)
{
   if (bi->alloc < len)
   {
      while (bi->alloc < len)
      {
         bi->alloc *= 2;
      }
      bi->nums = realloc(bi->nums, bi->alloc);
   }
}

static void bi_append_num(struct bigint *bi, unsigned char num)
{
   //printf("append %d\n", num);
   bi_ensure_len(bi, bi->count+1);
   bi->nums[bi->count] = num;
   bi->count++;
}

struct bigint *bigint_from_long(unsigned long n)
{
   struct bigint *bi = new_bigint();
   //printf("Making new big int from %ld\n", n);
   while (n > 0) {
      //printf("appending %lu\n", n%10);
      bi_append_num(bi, n%10);
      n /= 10;
   }
   return bi;
}

struct bigint *bi_clone(struct bigint *bi)
{
   struct bigint *new_bi = new_bigint();
   memcpy(new_bi, bi, sizeof(struct bigint));

   new_bi->nums = malloc(new_bi->alloc);
   memcpy(new_bi->nums, bi->nums, new_bi->alloc);

   return new_bi;
}

void log_bi(struct bigint *bi)
{
   int i;
   for (i=bi->count-1; i>=0; i--)
   {
      printf("%d", bi->nums[i]);
   }
   putchar('\n');
}

void free_bi(struct bigint *bi)
{
   free(bi->nums);
   free(bi);
}

unsigned char bi_get(struct bigint *bi, int ix)
{
   if (ix >= bi->count)
   {
      return 0;
   }
   else
   {
      return bi->nums[ix];
   }
}

struct bigint *bi_add(struct bigint *bi1, struct bigint *bi2)
{
   //printf("BI ADD\n");
   struct bigint *new_bi = new_bigint();
   int max_len = (bi1->count < bi2->count ? bi2->count+1 : bi1->count+1);
   bi_ensure_len(new_bi, max_len);
   int i;
   unsigned char carry = 0;
   unsigned char tmp;
   for (i=0; i<max_len; i++)
   {
      //printf("bi1 nums[%d] = %d bi2 nums[%d] = %d\n", i, bi_get(bi1, i), i, bi_get(bi2, i)); 
      tmp = carry + bi_get(bi1, i) + bi_get(bi2, i);
      carry = tmp/10;
      bi_append_num(new_bi, tmp%10);
      //printf("new_bi nums[i] = %d, carry was %d\n", new_bi->nums[i], carry);
   }

   new_bi->count = new_bi->nums[max_len-1] == 0 ? max_len-1 : max_len;
   //printf("new bi count = %d\n", new_bi->count);
   
   return new_bi;
}

static struct bigint *bi_mul_ix(struct bigint *bi, unsigned char val, int pow10)
{
   //printf("BI MUL IX\n");
   //printf("adding %d * bi * 10^%d\n", val, pow10);
   struct bigint *new_bi = new_bigint();
   int max_len = bi->count + pow10 + 1;
   bi_ensure_len(new_bi, max_len);

   int i;
   for (i=0; i<pow10; i++)
   {
      //printf("appending a ZERO for pow10, pow10=%d\n", pow10);
      bi_append_num(new_bi, 0);
   }

   unsigned char intermediate;
   unsigned char carry = 0;
   for (i=0; i<bi->count; i++)
   {
      //printf("calculating intermediate: bi nums[%d] = %d, val = %d, carry = %d\n", i, bi->nums[i], val, carry);
      intermediate = bi->nums[i] * val + carry;
      carry = intermediate / 10;
      //printf("mul_ix append\n");
      bi_append_num(new_bi, intermediate % 10);
   }
   if (carry > 0)
   {
      //printf("mul_ix append final\n");
      bi_append_num(new_bi, carry);
   }

   //printf("Result of mul_ix="); log_bi(new_bi);
   return new_bi;
}

struct bigint *bi_mul(struct bigint *bi1, struct bigint *bi2)
{
   //printf("BI MUL\n");
   struct bigint *new_bi = new_bigint();
   int max_len = bi1->count + bi2->count;
   bi_ensure_len(new_bi, max_len);
   int i;
   struct bigint *tmp1; // hold ref for freeing
   struct bigint *tmp2; // hold intermediate value
   for (i=0; i<bi2->count; i++)
   {
      tmp1 = new_bi;
      tmp2 = bi_mul_ix(bi1, bi2->nums[i], i);
      //printf("MUL ix intermediate val = ");
      //log_bi(tmp2);
      new_bi = bi_add(new_bi, tmp2);
      free_bi(tmp1);
      free_bi(tmp2);
   }

   return new_bi;
}

/*int main(void)
{
   //printf("%lu,\t", ULONG_MAX);
   struct bigint *bi = bigint_from_long(ULONG_MAX);
   log_bi(bi);
   struct bigint *bi2 = bi_add(bi, bi);
   log_bi(bi2);
   struct bigint *bi3 = bigint_from_long(100L);
   struct bigint *bi4 = bi_add(bi2, bi3);
   log_bi(bi4);

   struct bigint *bi5a = bigint_from_long(ULONG_MAX);
   log_bi(bi5a);
   struct bigint *bi5b = bigint_from_long(1001L);
   log_bi(bi5b);
   struct bigint *bi6 = bi_mul(bi5a, bi5a);
   log_bi(bi6);

   free_bi(bi);
   free_bi(bi2);
   free_bi(bi3);
   free_bi(bi4);
   free_bi(bi5a);
   free_bi(bi5b);
   free_bi(bi6);
   
}*/
