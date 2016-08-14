#include <limits.h>

#include "fasta.h"
#include "bigint.h"

// gcc mmch.c bigint.c fasta.c str.c -o mmch

struct bigint *n_factorial_over_m_factorial(unsigned long n, unsigned long m)
{
   if (n < m)
   {
      puts("something wrong: n<m");
      exit(1);
   }   

   if (n==m)
   {
      return bigint_from_long(1L);
   }
   else
   {
      struct bigint *f = n_factorial_over_m_factorial(n-1, m);
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
   unsigned long G_count=0;
   unsigned long C_count=0;
   unsigned long A_count=0;
   unsigned long U_count=0;
   int i;
   for (i=0; i<dna_str->len-1; i++)
   {
      switch(dna_str->data[i])
      {
         case 'A':
            A_count++;
            break;
         case 'U':
            U_count++;
            break;
         case 'C':
            C_count++;
            break;
         case 'G':
            G_count++;
            break;
         default:
            printf("Unexpected character '%c'\n", dna_str->data[i]);
            exit(0);
      }
      //printf("Switched on %c, now GC:%lu, AU:%lu\n", dna_str->data[i], GC_count, AU_count);
   }

   unsigned long AU_more = A_count > U_count ? A_count : U_count;
   unsigned long AU_less = A_count < U_count ? U_count - A_count : A_count - U_count;
   struct bigint *AU_options = n_factorial_over_m_factorial(AU_more, AU_less);
   printf("AU_options = "); log_bi(AU_options);

   unsigned long GC_more = G_count > C_count ? G_count : C_count;
   unsigned long GC_less = G_count < C_count ? C_count - G_count : G_count - C_count;
   struct bigint *GC_options = n_factorial_over_m_factorial(GC_more, GC_less);
   printf("GC_options = "); log_bi(GC_options);

   struct bigint *result = bi_mul(AU_options, GC_options);
   log_bi(result);

   free_fasta_list(fastas);

   free_bi(AU_options);
   free_bi(GC_options);
   free_bi(result);

   return 0;
}
