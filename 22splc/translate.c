#include "translate.h"

char aas[] =
{     /*A*/            /*C*/            /*G*/            /*U*/
/*A*/ 'K','N','K','N', 'T','T','T','T', 'R','S','R','S', 'I','I','M','I',
/*C*/ 'Q','H','Q','H', 'P','P','P','P', 'R','R','R','R', 'L','L','L','L',
/*G*/ 'E','D','E','D', 'A','A','A','A', 'G','G','G','G', 'V','V','V','V',
/*U*/ '\0','Y','\0','Y','S','S','S','S','\0','C','W','C','L','F','L','F'
};

int rna_hash_value(char base)
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
         return 3;
      default:
         printf("Unexpected base letter read: %c (0x%x)\n", base, base);
         return -1;
   }
}

int aa_hash(char c1, char c2, char c3)
{
   //printf("Base values: %d,%d,%d\n", base_value(c1)<<4, base_value(c2)<<2, base_value(c3));
   return (rna_hash_value(c1) << 4) 
        | (rna_hash_value(c2) << 2)
        | rna_hash_value(c3);
}

// get the letter of the amino acid if you translate the 3 letters as bases
char aa_at(char *from)
{
   return aas[aa_hash(from[0], from[1], from[2])];
}

struct int_list *get_locations(char *start, size_t max_len, char to_find)
{
   struct int_list *locations = new_int_list();
   int i;
   for (i=0; i<max_len-2; i+=3)
   {
      printf("i=%d, codon=%c%c%c, aa=%c, looking for %c\n", i, *(start+i), *(start+i+1), *(start+i+2), aa_at(start+i), to_find);
      if (aa_at(start+i) == to_find)
      {
         printf("Found\n");
         il_add(locations, i);
      }
   }
   return locations;
}

struct int_list *get_starts(char *start, size_t max_len)
{
   return get_locations(start, max_len, 'M');
}

struct int_list *get_ends(char *start, size_t max_len)
{
   return get_locations(start, max_len, '\0');
}

void add_protein(struct char_array_list *proteins, char *start)
{
   struct char_array *protein = malloc(sizeof(struct char_array));
   init_char_array(protein);

   char *codon_start;
   char c;
   for (codon_start=start; ; codon_start+=3)
   {
      c = aa_at(codon_start);
      add_char(protein, c);
      if (c == '\0')
      {
         //stop codon - end of protein
         break;
      }
   }

   cal_add(proteins, protein);
}

struct char_array_list *translate(struct char_array *mrna)
{
   struct char_array_list *proteins = new_char_array_list();
   size_t dna_len = strlen(mrna->data);

   int i;
   for (i=0; i<3; i++)
   {
      char *frame_start = mrna->data+i;
      printf("Frame from: %s\n", frame_start);

      struct int_list *starts = get_starts(frame_start, dna_len-i);
      //printf("Starts:");
      //log_il(starts);
      if (starts->count == 0)
      {
         il_free(starts);
         continue;
      }
      struct int_list *ends = get_ends(frame_start, dna_len-i);
      //printf("Ends:");
      //log_il(ends);
      if (ends->count == 0)
      {
         il_free(starts);
         il_free(ends);
         continue;
      }
      int last_end = il_get(ends, ends->count-1);
      int last_possible_start = last_end - 3;

      int starts_ix;
      for (starts_ix=0; starts_ix<starts->count-1; starts_ix++)
      {
         int index = il_get(starts, starts_ix);
         if (index >= last_possible_start)
         {
            break;
         }
         add_protein(proteins, frame_start+index);
      }
      il_free(starts);
      il_free(ends);
   }

   return proteins;
}


