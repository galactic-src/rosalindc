#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/**
 * (1 1), (1 2 1), (1 3 3 1), (1 4 6 4 1), (1 5 10 10 5 1) (1 6 15 20 15 6 1)
 * It can be shown that the sum k=0->n nCk = 2^n
 * So it follows that sum k=m->n nCk = 2^n - 2^(m-1) = 2^(m-1)[2^(n-m+1) - 1]
 * for example data, n=6, m=3
 * 6C3 + 6C4 + 6C5 + 6C6 = 20 + 15 + 6 + 1 = 42

 * Actually this approach doesn't work because the subtracted sum is only summing *m*Ck terms, not nCk. Starting over.
 */

unsigned long apowbmod(int a, int b, int mod)
{
   unsigned long mod_at = ULONG_MAX / (unsigned long)a;

   unsigned long result = 1;
   int i;
   for (i=0; i<b; i++)
   {
      printf("Result is %lu\n", result);
      result *= a;
      if (result > mod_at)
      {
         // avoid overflow
         puts("Avoiding overflow");
         result %= mod;
      }
   }

   result %= mod;
   printf("pow2mod returning, result is %lu\n", result);
   return result;
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Couldn't open data file");
      exit(1);
   }

   unsigned long n;
   unsigned long m;
   if (fscanf(f, "%lu %lu", &n, &m) != 2)
   {
      puts("error reading data");
      exit(1);
   }
   fclose(f);
   //printf("n=%lu, m=%lu\n", n, m);

   // calculate 2^(n-m+1)
   unsigned long target_pow = n-m+1;
   unsigned long result = pow2mod(target_pow, 1000000);

   // [2^(n-m+1)-1] % 1000000
   if (result < 1)
   {
      result += 1000000;
   }
   result -= 1;
   printf("after -1, result is %lu\n", result);
   
   result *= pow2mod(m-1, 1000000);
   printf("before final mod, result is %lu\n", result);
   result %= 1000000;

   printf("%lu\n", result);
}
