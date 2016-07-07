#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "str.h"

/**
 * This should really only print unique strings, but that adds a layer of complexity, storing results in a single strings list. I just looked to check that results were unique instead.
 */

char aas[] =
{     /*A*/            /*C*/            /*G*/            /*U*/
/*A*/ 'K','N','K','N', 'T','T','T','T', 'R','S','R','S', 'I','I','M','I',
/*C*/ 'Q','H','Q','H', 'P','P','P','P', 'R','R','R','R', 'L','L','L','L',
/*G*/ 'E','D','E','D', 'A','A','A','A', 'G','G','G','G', 'V','V','V','V',
/*U*/ '\0','Y','\0','Y','S','S','S','S','\0','C','W','C','L','F','L','F'
};

int base_value(char base)
{
   switch(base)
   {
      case 'A': 
         return 0;
      case 'C':
         return 1;
      case 'G':
         return 2;
      case 'U':
      case 'T':   // technically just U. This only works because we're interested in both strands' mRNA.
         return 3;
      default:
         printf("Unexpected base letter read: %c\n", base);
         return -1;
   }
}

int aa_hash(char c1, char c2, char c3)
{
   //printf("Base values: %d,%d,%d\n", base_value(c1)<<4, base_value(c2)<<2, base_value(c3));
   return (base_value(c1) << 4) 
        | (base_value(c2) << 2)
        | base_value(c3);
}

// get the letter of the amino acid if you translate the 3 letters as bases
char aa_at(char *from)
{
   return aas[aa_hash(from[0], from[1], from[2])];
}

char complement(char base)
{
   switch (base)
   {
      case 'A':
         return 'T';
      case 'T':
         return 'A';
      case 'C':
         return 'G';
      case 'G':
         return 'C';
      default:
         printf("Unexpected character 0x%x (%c), returning NUL", base, base);
         return '\0';
   }
}

struct char_array *get_complement(struct char_array *dna)
{
   char *complementary = malloc(dna->len);
   int ix;
   for (ix=0; ix<dna->len-1; ix++)
   {
      complementary[ix] = complement((dna->data)[ix]);
   }
   char last_char = complementary[dna->len-1];
   complementary[dna->len-1] = '\0';

   struct char_array *complement = attach_str(complementary);
   reverse_char_array(complement);
   return complement;
}

void log_protein_from(char *protein_start)
{
   int found_stop = 0;
   //printf("aa at protein start is %c\n", aa_at(protein_start));
   if (aa_at(protein_start) == 'M')
   {
      //printf("might be a protein, if we can find an end");
      char protein[400];
      int src_ix, dest_ix;
      for (src_ix=0, dest_ix=0; src_ix<strlen(protein_start)-3; src_ix+=3, dest_ix++)
      {
         protein[dest_ix] = aa_at(protein_start + src_ix);
         if (protein[dest_ix] == '\0')
         {
            found_stop = 1;
            break;
         }
      }
      
      if (found_stop)
      {
         printf("%s\n", protein);
      }
   }
}

void log_proteins(char *bases)
{
   int ix;
   for (ix=0; ix<strlen(bases)-3; ix++)
   {
      //printf("try at %d\n", ix);
      log_protein_from(bases+ix);
   }
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }

   struct char_array *dna = malloc(sizeof(struct char_array));
   init_char_array(dna);

   ssize_t chars_read;
   size_t n=0;
   char *title = NULL;
   getline(&title, &n, f);

   n=0;
   char *line = NULL;
   while ((chars_read = getline(&line, &n, f)) > -1)
   {
      line[chars_read-1] = '\0'; // strip newline
      append_chars(dna, line);
   }
   free(line);
   //printf("%s\n", dna->data);

   struct char_array *complementary = get_complement(dna);
   //printf("%s\n", complementary->data);
   
   //puts("Proteins from dna");
   log_proteins(dna->data);
   //puts("Proteins from complementary");
   log_proteins(complementary->data);

   free_char_array(dna);
   free_char_array(complementary);
   free(title);
   fclose(f);
}
