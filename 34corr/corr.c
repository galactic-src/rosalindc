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
int hamm_dist(char *s1, char *s2)
{
   int differences = 0;
   while (1)
   {
      if (*(s1++) != *(s2++))
      {
         //printf("difference: %c != %c\n", *(s1-1), *(s2-1));
         differences++;
         if (differences == 2)
         {
            break;
         }
      }

      // We are given that the strings will have equal length/. If we've reached the end, we're done
      if (*s1 == '\0')
      {
         break;
      }
   }

   return differences;
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");

   struct char_array_list *good_reads = new_char_array_list();
   struct char_array_list *bad_reads = new_char_array_list();

   struct fasta_node *fn;
   struct char_array *fn_rev_comp;
   struct fasta_node *cmp;
   while (fl->head != NULL)
   {
      int matches_found = 0;

      /*
      puts("***REMAINING FASTAS TO MATCH***");
      log_fasta_list(fl);
      puts("*******************************");
      */

      // each string has a pair, either identical, or 1 base out.
      fn = fl->head;
      //printf("Removing %s (next to match)\n", fn->fasta.dna.data);
      fn_rev_comp = get_complement(&fn->fasta.dna);

      // remove from the list of remaining
      fl->head = fn->next;

      //printf("fn: %s\n", next->fasta.dna.data);
      //printf("fn revcomp: %s\n", next_rev_comp->data);

      //Compare both the string and the reverse complement with each remaining DNA string.
      struct fasta_node *cmp = fl->head;
      struct fasta_node *cmp_prev = NULL;
      int how_similar;
      while (cmp != NULL)
      {
         if (!strcmp(fn->fasta.dna.data, cmp->fasta.dna.data)
           || !strcmp(fn_rev_comp->data, cmp->fasta.dna.data))
         {
            matches_found++;
            
            //remove from remaining and free
            //printf("Removing %s (matched)\n", cmp->fasta.dna.data);
            if (cmp_prev != NULL)
            {
               cmp_prev->next = cmp->next;
            }
            else
            {
               fl->head = cmp->next;
            }
            free_fasta_node(cmp);
            //puts("Freed fasta");
            //cmp_prev remains the same
         }
         else
         {
            cmp_prev = cmp;
         }

         if (cmp_prev != NULL)
         {
            cmp = cmp_prev->next;
         }
         else
         {
            cmp = fl->head;
         }
      }

      if (matches_found)
      {
         //puts("Match found");
         // add the dna and its complement as "good" reads
         cal_add(good_reads, clone_ca(&fn->fasta.dna));
         cal_add(good_reads, fn_rev_comp);
      }
      else
      {
         //puts("Match not found");
         // add the dna as a bad read
         cal_add(bad_reads, clone_ca(&fn->fasta.dna));
         // in this case we need to tidy up the complement too
         free_char_array(fn_rev_comp);
      }

      // free fn, since it's been removed from the list
      free_fasta_node(fn);
      //puts("FREED");
   }

   free_fasta_list(fl);

   struct char_array_list *corrections = new_char_array_list();
   int bad_ix;
   struct char_array *bad_ca;
   int good_ix;
   struct char_array *good_ca;
   for (bad_ix=0; bad_ix<bad_reads->len; bad_ix++)
   {
      int found = 0;
      bad_ca = bad_reads->strs[bad_ix];
      for (good_ix=0; good_ix<good_reads->len; good_ix++)
      {
         good_ca = good_reads->strs[good_ix];
         if (hamm_dist(bad_ca->data, good_ca->data) == 1)
         {
            // found the correct version - add correction
            struct char_array *correction = clone_ca(bad_ca);
            append_chars(correction, "->");
            append_chars(correction, good_ca->data);
            cal_add(corrections, correction);
            found = 1;
            break;
         }
      }

      if (!found)
      {
         printf("failed to find match for bad string %s\n", bad_ca->data);
      }
   }

   log_cal(corrections);
   free_cal(good_reads);
   free_cal(bad_reads);
   free_cal(corrections);

   return 0;
}
