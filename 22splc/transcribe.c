#include <stdio.h>

#include "transcribe.h"
#include "str.h"

char transcribe_dna_base(char base)
{
   switch(base)
   {
      case 'A': 
         return 'U';
      case 'C':
         return 'G';
      case 'G':
         return 'C';
      case 'T':
         return 'A';
      default:
         printf("Unexpected base letter read: %c\n", base);
         return -1;
   }
}

struct char_array *transcribe_dna(struct char_array *dna)
{
   struct char_array *mrna = clone_ca(dna);
   int ix;
   for (ix=0; ix<mrna->len-1; ix++)
   {
      mrna->data[ix] = transcribe_dna_base(dna->data[ix]);
   }
   reverse_char_array(mrna);
   return mrna;
}
