#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

#include "intlist.h"

/*
 * This approach feels inefficient, but it works.
 * Essentially, keep a list of all the factors and then for each prime less than
 * the maximum divisor, go through all the divisors and tally the total power.
 * Then go through the multipliers and divide them down by the divisors as much as required.
 * Finally, multiply the remaining multipliers together.
 *
 * Repeat and sum.
 * Could probably afford to mod 1000000 a bit less often too, but it's still pretty speedy.
 * Could generate the list of primes too, with a bit of effort.
 */

int primes[] = {
   2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,
   53,59,61,67,71,73,79,83,89,97,101,103,
   107,109,113,127,131,137,139,149,151,
   157,163,167,173,179,181,191,193,197,
   199,211,223,227,229,233,239,241,251,
   257,263,269,271,277,281,283,293,307,
   311,313,317,331,337,347,349,353,359,
   367,373,379,383,389,397,401,409,419,
   421,431,433,439,443,449,457,461,463,
   467,479,487,491,499,503,509,521,523,
   541,547,557,563,569,571,577,587,593,
   599,601,607,613,617,619,631,641,643,
   647,653,659,661,673,677,683,691,701,
   709,719,727,733,739,743,751,757,761,
   769,773,787,797,809,811,821,823,827,
   829,839,853,857,859,863,877,881,883,
   887,907,911,919,929,937,941,947,953,
   967,971,977,983,991,997,1009,1013,
   1019,1021,1031,1033,1039,1049,1051,
   1061,1063,1069,1087,1091,1093,1097,
   1103,1109,1117,1123,1129,1151,1153,
   1163,1171,1181,1187,1193,1201,1213,
   1217,1223,1229,1231,1237,1249,1259,
   1277,1279,1283,1289,1291,1297,1301,
   1303,1307,1319,1321,1327,1361,1367,
   1373,1381,1399,1409,1423,1427,1429,
   1433,1439,1447,1451,1453,1459,1471,
   1481,1483,1487,1489,1493,1499,1511,
   1523,1531,1543,1549,1553,1559,1567,
   1571,1579,1583,1597,1601,1607,1609,
   1613,1619,1621,1627,1637,1657,1663,
   1667,1669,1693,1697,1699,1709,1721,
   1723,1733,1741,1747,1753,1759,1777,
   1783,1787,1789,1801,1811,1823,1831,
   1847,1861,1867,1871,1873,1877,1879,
   1889,1901,1907,1913,1931,1933,1949,
   1951,1973,1979,1987,1993,1997,1999};

/* Expects that for any factor in a, there will be a matching one in b */

void remove_common_factors(int prime, struct int_list *a, struct int_list *b)
{
   int primes_found=0;
   int i;
   for (i=0; i<b->count; i++)
   {
      while (b->values[i] > 0
         && b->values[i] % prime == 0)
      {
         b->values[i] /= prime;
         primes_found++;
      }
   }
   //printf("Found %d factors of %d\n", primes_found, prime);

   if (primes_found == 0)
   {
      return;
   }

   for(i=0; i<a->count; i++)
   {
      //printf("Checking %d for factors of %d\n", a->values[i], prime);
      while (a->values[i] % prime == 0)
      {
         //printf("%d divides %d\n", a->values[i], prime);
         a->values[i] /= prime;

         if (--primes_found == 0)
         {
            return;
         }
         //printf("still need to find %d of %d\n", primes_found, prime);
      }
   }

   if (primes_found > 0)
   {
      printf("Failed to match all the factors of %d in a with matching ones in b\n", prime);
      printf("muls: ");
      log_il(a);
      printf("divs: ");
      log_il(b);
      exit(1);
   }
}

unsigned long il_product(struct int_list *il)
{
   unsigned long result = 1;
   int i;
   for (i=0; i<il->count; i++)
   {
      result *= il->values[i];
      result %= 1000000;
   }
   return result;
}

unsigned long exact_large_choose(int n, int m)
{
   int a = n-m;
   int greatest_factor = (m > a) ? m : a;
   int least_factor = n-greatest_factor;
   //printf("big: %d small: %d\n", greatest_factor, least_factor);

   struct int_list *muls = new_int_list();
   int mul_factor;
   for (mul_factor=greatest_factor+1; mul_factor <= n; mul_factor++)
   {
     il_add(muls, mul_factor);
   }
   //printf("muls: ");
   //log_il(muls);

   struct int_list *divs = new_int_list();
   int div_factor;
   for (div_factor=1; div_factor<=least_factor; div_factor++)
   {
      il_add(divs, div_factor);
   }
   //printf("divs: ");
   //log_il(divs);

   int primes_count = (sizeof primes)/(sizeof(int));
   
   int prime;
   int prime_ix=0;
   while ((prime = primes[prime_ix++]) <= m)
   {
      remove_common_factors(prime, muls, divs);
   }

   unsigned long divs_res = il_product(divs);
   unsigned long muls_res = il_product(muls);
   //printf("INTERMEDIATE END: divs=%lu muls=%lu\n", divs_res, muls_res);

   il_free(muls);
   il_free(divs);

   return muls_res;
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Couldn't open data file");
      exit(1);
   }

   int n;
   int m;
   if (fscanf(f, "%d %d", &n, &m) != 2)
   {
      puts("error reading data");
      exit(1);
   }
   fclose(f);
   //printf("n=%d, m=%d\n", n, m);

   unsigned long total = 0;

   int k;
   for (int k=m; k<=n; k++)
   {
      total += exact_large_choose(n, k);
      total %= 1000000;
   }

   printf("%lu\n", total);
   
   return 0;
}
