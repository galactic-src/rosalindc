#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// gcc prob.c -lm

/**
 * first line is a list of DNA bases
 * second line is a list of numbers 0 < n < 1
 *
 * tally GC and AT
 * with n = p(G|C) content, p(G) = p(C) = n/2, P(A)=P(T) = (1-n)/2
 */

struct double_list
{
   int count;
   int alloc;
   long double *vals;
};

struct double_list *new_double_list()
{
   struct double_list *new_dl = malloc(sizeof(struct double_list));
   new_dl->count = 0;
   new_dl->alloc = 10;
   new_dl->vals = malloc(10*sizeof(long double));

   return new_dl;
}

void dl_ensure_len(struct double_list *dl, int len)
{
   while (dl->alloc < len)
   {
      dl->alloc *= 2;
   }
   dl->vals = realloc(dl->vals, dl->alloc * sizeof(long double));
}

void dl_append(struct double_list * dl, long double d)
{
   dl_ensure_len(dl, dl->count+1);
   //printf("Setting index %d to %.3Lf\n", dl->count, d);
   dl->vals[dl->count] = d;
   //printf("Set to %.3Lf\n", dl->vals[dl->count]);
   dl->count++;
}

void log_dl(struct double_list *dl)
{
   int i;
   for (i=0; i<dl->count; i++)
   {
      printf("%.3Lf\n", dl->vals[i]);
   }
}

void free_dl(struct double_list *dl)
{
   free(dl->vals);
   free(dl);
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file");
      exit(1);
   }
   
   char *dna;
   if (fscanf(f, "%ms", &dna) != 1)
   {
      puts("error reading dna from data file");
      exit(1);
   }

   //puts(dna);

   struct double_list *dl = new_double_list();
   int scan_res;
   long double result;
   while ((scan_res = fscanf(f, "%Lf", &result)) == 1)
   {
      //printf("Read ld: %.3Lf\n", result);
      dl_append(dl, result);
   }
   if (scan_res != EOF)
   {
      printf("scanning for probabilities, got %d, expected 1\n", scan_res);
      exit(1);
   }
   fclose(f);

   //puts("Logging values read");
   //log_dl(dl);

   //source file has been read. Get DNA tallies.
   int GC_tally=0;
   int AT_tally=0;
   char *c = dna;
   do 
   {
      switch(*c)
      {
         case 'A':
         case 'T':
            AT_tally++;
            break;
         case 'C':
         case 'G':
            GC_tally++;
            break;
         default:
            printf("Unrecognised char in dna [%c]", *c);
            exit(1);
      }
   }
   while (*(++c) != '\0');

   //printf("GC tally = %d, AT tally = %d\n", GC_tally, AT_tally);

   int ix;
   for (ix=0; ix<dl->count; ix++)
   {  
      long double gc = dl->vals[ix];
      long double log_p_gc = log10l(gc/2.0);
      long double log_p_at = log10l((1.0-gc)/2.0);
      long double result = (GC_tally * log_p_gc) + (AT_tally * log_p_at);
      //printf("num=%.3Lf, p(G/C)=%.3Lf, p(A/T)=%.3Lf, result=%.3Lf\n", gc, log_p_gc, log_p_at, result);
      printf("%.3Lf ", result);
   }
   putchar('\n');

   free_dl(dl);
   free(dna);
}
