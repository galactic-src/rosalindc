#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// every element in the set can be included or not
// so we are just calculating (2 ^ N) mod 1,000,000 (N <= 1000)

long unsigned pow2(int pow)
{
   long unsigned result = 1;
   while (pow > 0)
   {
      result *= 2;
      pow--;
   }
   return result;
}

void update_pow(int pow, int *N, long unsigned *total)
{
   long unsigned total_tmp = *total;
   int N_tmp = *N;

   long unsigned factor = pow2(pow);
   long unsigned mod_at = ULONG_MAX / factor;
   while (N_tmp >= pow)
   {
      total_tmp *= factor;
      N_tmp -= pow;
      if (total_tmp >= mod_at)
      {
         total_tmp %= 1000000;
      }
   }

   total_tmp %= 1000000;

   *N = N_tmp;
   *total = total_tmp;
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      puts("Usage: sset <n> for integer n");
      exit(1);
   }
   
   int set_size = atoi(argv[1]);

   unsigned long total = 1;
   
   int pow = 32;
   while (pow >= 1)
   {
      update_pow(pow, &set_size, &total);
      pow /= 2;
   }

   printf("%ld\n", total);

   return 0;
}
