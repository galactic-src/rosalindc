#include <limits.h>

#include "fasta.h"
#include "bigint.h"


struct bigint *factorial(unsigned long n)
{
   
   if (n==0 || n==1)
   {
      return bigint_from_long(1L);
   }
   else
   {
      struct bigint *f = factorial(n-1);
      struct bigint *bi_n = bigint_from_long(n);
      struct bigint *ret = bi_mul(bi_n, f);
      free_bi(f);
      free_bi(bi_n);
      return ret;
   }
}

int main(void)
{
   struct fasta_list *fastas = parse_fasta_file("data");

   struct char_array *dna_str = &fastas->head->fasta.dna;
   unsigned long GC_count=0;
   unsigned long AU_count=0;
   int i;
   for (i=0; i<dna_str->len-1; i++)
   {
      switch(dna_str->data[i])
      {
         case 'A':
         case 'U':
            AU_count++;
            break;
         case 'C':
         case 'G':
            GC_count++;
            break;
         default:
            printf("Unexpected character '%c'\n", dna_str->data[i]);
            exit(0);
      }
      //printf("Switched on %c, now GC:%lu, AU:%lu\n", dna_str->data[i], GC_count, AU_count);
   }

   struct bigint *AU_options = factorial(AU_count/2);
   printf("AU_options = "); log_bi(AU_options);
   struct bigint *GC_options = factorial(GC_count/2);
   printf("GC_options = "); log_bi(GC_options);
   struct bigint *result = bi_mul(AU_options, GC_options);
   log_bi(result);

   free_fasta_list(fastas);

   free_bi(AU_options);
   free_bi(GC_options);
   free_bi(result);

   return 0;
}
