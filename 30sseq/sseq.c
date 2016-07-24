
#include "intlist.h"
#include "fasta.h"

int main(void)
{
   struct fasta_list *fastas = parse_fasta_file("data");
   char *dna = fastas->head->fasta.dna.data;
   char *sseq = fastas->head->next->fasta.dna.data;
   struct int_list *locations = new_int_list();

   int dna_ix=0;
   int sseq_ix=0;

   puts(dna);
   puts(sseq);

   while (1)
   {
      if (dna[dna_ix] == sseq[sseq_ix])
      {
         printf("%c == %c - adding %d\n", dna[dna_ix], sseq[sseq_ix], dna_ix+1);
         il_add(locations, dna_ix+1);
         if (sseq[++sseq_ix] == '\0')
         {
            //done
            break;
         }
         if (dna[++dna_ix] == '\0')
         {
            printf("Didn't find all substring chars, got up to %d (%c)\n", sseq_ix, sseq[sseq_ix]);
            exit(1);
         }
      }
      else
      {
         ++dna_ix;
      }
   }

   log_il(locations);

   free_fasta_list(fastas);
   il_free(locations);
}

