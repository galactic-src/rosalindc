#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//gcc rstr.c -lm -o rstr

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      perror("Error opening data file:");
      exit(1);
   }

   int num_rands;
   double GC_content;
   char *target_string;
   if (fscanf(f, "%d %lf\n%ms", &num_rands, &GC_content, &target_string) != 3)
   {
      perror("error reading input data:");
      exit(1);
   }
   //printf("%d, %lf, %s\n", num_rands, GC_content, target_string);

   double p_GC = GC_content/2.0L;
   double p_AT = (1.0L - GC_content)/2.0L;
   
   double p_dna = 1.0L;
   char *c = target_string;
   while (*c != '\0')
   {
      switch (*c)
      {
         case 'A':
         case 'T':
            p_dna *= p_AT;
            break;
         case 'C':
         case 'G':
            p_dna *= p_GC;
            break;
         default:
            printf("Unexpected char in dna string: %c (%x)\n", *c, *c);
            exit(1);
      }
      c++;
   }
   free(target_string);
   
   //printf("Prob randomly generate string = %lf\n", p_dna);
   double p_not_dna = 1.0L - p_dna;

   double prob_not_generated = powl(p_not_dna, (double)num_rands);
   double prob_at_least_one_generated = 1.0L - prob_not_generated;
   printf("%lf\n", prob_at_least_one_generated);

   fclose(f);

   return 0;
}
