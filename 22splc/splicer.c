#include "str.h"
#include "fasta.h"
#include "translate.h"

// gcc splicer.c str.c intlist.c transcribe.c translate.c fasta.c -o splicer

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");
   //log_fasta_list(fl);
   
   struct fasta_node *fn = fl->head;
   struct char_array *exons = clone_ca(&(fn->fasta.dna));

   while (fn->next != NULL)
   {
      fn = fn->next;
      //printf("Removing %s\n", fn->fasta.dna.data);
      remove_all(exons, fn->fasta.dna.data);
      //printf("Remaining dna (%lu): %s\n", exons->len, exons->data);
   }
   free_fasta_list(fl);

   //printf("Remaining dna (%lu): %s\n", data->len, data->data);

   //Transcribe the DNA
   //struct char_array *mrna = transcribe_dna(data);
   //printf("mRNA: %s\n", mrna->data);
   
   //Maybe they just want T replaced with U?! No actual transcription..
   size_t chars_to_change = strlen(exons->data);
   int i;
   for (i=0; i<chars_to_change; i++)
   {
      if (exons->data[i] == 'T')
      {
         exons->data[i] = 'U';
      }
   }

   struct char_array_list *proteins = translate(exons);
   printf("Proteins\n");   
   log_cal(proteins);

   free_char_array(exons);
   free_cal(proteins);
   return 0;
}
