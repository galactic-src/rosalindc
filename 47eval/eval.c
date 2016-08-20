#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void tally_bases(char *seq, int *gc_count, int *at_count)
{
   int gc = 0;
   int at = 0;

   int ix;
   for (ix=0; ix<strlen(seq); ix++)
   {
      switch(seq[ix])
      {
         case 'A':
         case 'T':
            at++;
            break;
         case 'G':
         case 'C':
            gc++;
            break;
         default:
            printf("unexpected character in dna string at ix %d: '%c' (%x)\n", ix, seq[ix], seq[ix]);
            exit(1);
      }
   }
   *gc_count = gc;
   *at_count = at;
}

double *calculate_probs(int gc_count, int at_count, double *gc_vals, int gc_vals_count, int poss_starts)
{
   printf("Possible starts: %d\n", poss_starts);

   double *probs = malloc(sizeof(double) * gc_vals_count);

   int i;
   for (i=0; i<gc_vals_count; i++)
   {
      double gc_val = gc_vals[i];
      double p_gc = gc_val/2.0;
      double p_at = (1.0 - gc_val)/2.0;

      

      probs[i] = (double)poss_starts * pow(p_gc, (double)gc_count) * pow(p_at, (double)at_count);
   }

   return probs;
}

void log_probs(double *probs, int probs_count)
{
   int i;
   for (i=0; i<probs_count; i++)
   {
      printf("%lf", probs[i]);
      if (i<probs_count)
      {
         putchar(' ');
      }
   }
   putchar('\n');
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      perror("Failed to open data file:");
      exit(1);
   }

   int dna_len;
   char *seq;
   if (fscanf(f, "%d\n%ms", &dna_len, &seq) != 2)
   {
      puts("Failed to read data file");
      exit(1);
   }
   int seq_len = strlen(seq);
   printf("total len %d, seq %s, seq_len %d\n", dna_len, seq, seq_len);

   int gc_vals_count = 0;
   double gc_vals[20];
   double d;
   while (fscanf(f, "%lf", &d) == 1)
   {
      gc_vals[gc_vals_count++] = d;
   }
   fclose(f);
   log_probs(gc_vals, gc_vals_count);
   printf("array size = %d\n", gc_vals_count);

   int gc_count, at_count;
   tally_bases(seq, &gc_count, &at_count);
   //printf("GC: %d, AT: %d\n", gc_count, at_count);
   free(seq);

   double *probs = calculate_probs(gc_count, at_count, gc_vals, gc_vals_count, dna_len-seq_len+1);
   log_probs(probs, gc_vals_count);
   free(probs);
   return 0;
}
