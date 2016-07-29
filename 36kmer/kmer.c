#include "fasta.h"
#include "intlist.h"

//gcc kmer.c intlist.c fasta.c str.c -o kmer

#define ALPHA_SIZE 4
char alphabet[ALPHA_SIZE] = {'A', 'C', 'G', 'T'};

void tally_instances(char const * const dna, char const *kmer_buf, struct int_list *results)
{
   int tally = 0;
   int offset = -1;
   char *instance;
   while ((instance = strstr(dna+offset+1, kmer_buf)) != NULL)
   {
      offset = (int)(instance - dna);
      ++tally;
   }
   //printf("Adding %d\n", tally);
   il_add(results, tally);
}

void count_substrings(char const * const dna, char *kmer_buf, int depth, struct int_list *results)
{
   int i;
   for (i=0; i<ALPHA_SIZE; i++)
   {
      kmer_buf[depth] = alphabet[i];
      if (depth < ALPHA_SIZE-1) 
      {
         count_substrings(dna, kmer_buf, depth+1, results);
      }
      else
      {
         tally_instances(dna, kmer_buf, results);
      }
   }
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");
   char *dna = fl->head->fasta.dna.data;

   char kmer_buf[ALPHA_SIZE+1];
   kmer_buf[ALPHA_SIZE] = '\0';
   struct int_list *results = new_int_list();

   count_substrings(dna, kmer_buf, 0, results);

   free_fasta_list(fl);
   log_il(results);
   il_free(results);
}
