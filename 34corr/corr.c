#include <stdlib.h>
#include <stdio.h>

#include "fasta.h"

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
         printf("complement: Unexpected character 0x%x (%c), returning NUL", base, base);
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

/*
 * Return 0 if identical, 1 if off-by-1, 2 if more than 1
 */
int how_similar(char *s1, char *s2)
{
   int differences = 0;
   while (1)
   {
      if (*(s1++) != *(s2++))
      {
         differences++;
         if (differences == 2
            // We are given that the strings will have equal length/. If we've reached the end, we're done
            || *s1 == '\0')
         {
            break;
         }
      }
   }

   return differences;
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");

   struct char_array_list *corrections = new_char_array_list();

   struct fasta_node *next;
   struct char_array *next_rev_comp;
   struct fasta_node *cmp;
   while (fl->head != NULL)
   {
      // each string has a pair, either identical, or 1 base out.
      next = fl->head;
      next_rev_comp = get_complement(&next->fasta.dna);
      fl->head = next->next;

      //printf("next: %s\n", next->fasta.dna.data);
      //printf("next revcomp: %s\n", next_rev_comp->data);

      //Compare both the string and the reverse complement with each remaining DNA string.
      struct fasta_node *cmp = fl->head;
      struct fasta_node *cmp_prev = NULL;
      int how_similar;
      while (cmp != NULL)
      {
         how_similar = how_similar(next->fasta.dna.data, cmp->fasta.dna.data);
         if (how_similar < 2)
         {
            if (how_similar == 1)
            {
               //add to list of corrections
               struct char_array *corr = clone_ca(
               corrections, next->
            }

            //remove from remaining and free
            cmp_prev->next = cmp->next;
            free_fasta_node(cmp);
         }

         // try the reverse complement

         cmp_prev = cmp;
         cmp = cmp->next;
      }
      if (cmp == NULL)
      {
         printf("didn't match anything for string %s - cmp was NULL\n", next->fasta.dna.data);
         exit(1);
      }
      
      // since we've matched up, we can remove the match from the list too
      

      // free next, since it's been removed from the list
   }
}
