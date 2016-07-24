#include "fasta.h"

// transition A<>G or C<>T
// transversion A<>T/C etc
// ratio = transition/transversion

int is_transition(char c1, char c2)
{
   return (c1=='A' && c2=='G'
     || c1=='G' && c2=='A'
     || c1=='C' && c2=='T'
     || c1=='T' && c2=='C');
}

int main(void)
{
   struct fasta_list *fastas = parse_fasta_file("data");
   char *dna1 = fastas->head->fasta.dna.data;
   char *dna2 = fastas->head->next->fasta.dna.data;

   //puts(dna1);
   //puts(dna2);

   if (strlen(dna1) != strlen(dna2))
   {
      puts("different length dna strings... not ready for this!");
      exit(1);
   }

   double transitions = 0;
   double transversions = 0;

   int ix=0;
   while (1)
   {
      if (dna1[ix] != dna2[ix])
      {
         if (is_transition(dna1[ix], dna2[ix]))
         {
            transitions++;
         }
         else
         {
            transversions++;
         }
      }
      if (dna1[++ix] == '\0')
      {
         break;
      }
   }

   printf("%lf\n", transitions/transversions);

   free_fasta_list(fastas);
}
