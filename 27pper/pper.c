#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

/**
 * # ways to choose r items from n = nCr = n!/(n-r)!r!
 * then r! ways to order them = n!/(n-r)!
 * which becomes n x (n-1) x ... x (n-r+1)
 *
 * n <= 100, r <=10, 
 * result mod 1000000 required
 */

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Couldn't open data file");
      exit(1);
   }

   unsigned long n;
   unsigned long r;
   if (fscanf(f, "%lu %lu", &n, &r) != 2)
   {
      puts("error reading data");
      exit(1);
   }
   printf("n=%lu, r=%lu\n", n, r);

   unsigned long threshold = ULONG_MAX / n;
   unsigned long lowest = n-r;
   printf("threshold=%lu, lowest factor=%lu\n", threshold, lowest);

   unsigned long result = 1L;
   unsigned long factor = n;
   while (factor > lowest)
   {
      result *= factor;
      if (result > threshold)
      {
         result %= 1000000;
      }
      factor--;
   }
   result %= 1000000;
   printf("%lu\n", result);
}
