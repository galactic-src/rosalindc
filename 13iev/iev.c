#include <stdio.h>

/* genotype pairings
AA-AA p = 1.0
AA-Aa p = 1.0
AA-aa p = 1.0
Aa-Aa p = 0.75
Aa-aa p = 0.5
aa-aa p = 0.0
 */

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      return 1;
   }

   long AA_AA, AA_Aa, AA_aa, Aa_Aa, Aa_aa, aa_aa;
   if (fscanf(f, "%ld %ld %ld %ld %ld %ld", &AA_AA, &AA_Aa, &AA_aa, &Aa_Aa, &Aa_aa, &aa_aa) < 6)
   {
      puts("Failed to read all variables from data file");
      fclose(f);
      return 1;
   }

   double expected =
      2.0 * ((AA_AA + AA_Aa + AA_aa)
      + Aa_Aa * 0.75
      + Aa_aa * 0.5);
      //+ aa_aa * 0.0;

   printf("%lf", expected);
   return 0;
}
