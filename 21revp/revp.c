#include <stdlib.h>

#include "str.h"
#include "fasta.h"
#include "locations.h"

char dna_dna_comp(char base)
{
   switch (base)
   {
      case 'A':
         return 'T';
      case 'C':
         return 'G';
      case 'G':
         return 'C';
      case 'T':
         return 'A';
      default:
         printf("Not a dna base: %c (0x%x)\n", base, base);
         exit(1);
   }
}

struct palindrome;
struct palindrome
{
   int location;
   int length;
   struct palindrome *next;
};

struct palindrome *new_palindrome(int location, int length)
{
   struct palindrome *p = malloc(sizeof(struct palindrome));
   p->location = location;
   p->length = length;
   p->next = NULL;
}

struct palindrome_list
{
   struct palindrome *head;
   struct palindrome *current;
};

struct palindrome_list *new_palindrome_list()
{
   struct palindrome_list *pl = malloc(sizeof(struct palindrome_list));
   pl->head = NULL;
   pl->current = NULL;
   return pl;
}

void add_palindrome(struct palindrome_list *pl, struct palindrome *p)
{
   if (p == NULL)
   {
      puts("Tried to add null palindrome");
      exit(1);
   }

   if (pl->head == NULL)
   {
      pl->head = p;
      pl->current = p;
   }
   else
   {
      pl->current->next = p;
      pl->current = p;
   }
}

void log_palindromes(struct palindrome_list *pl)
{
   pl->current = pl->head;
   while (pl->current != NULL)
   {
      printf("%d %d\n", pl->current->location, pl->current->length);
      pl->current = pl->current->next;
   }
}

int palindrome_at(char *dna, int max_len)
{
   max_len = (max_len < 12 ? max_len : 12);

   int best_palin_len = 0;

   int check_len;
   //printf("Max len = %d\n", max_len);
   for (check_len = 4; check_len <= max_len; check_len+=2)
   {
      //printf("Checking for palindrome length %d, max_len %d\n", check_len, max_len);
      if (check_len > max_len)
      {
         break;
      }
      
      int is_palin = 1;
      char *p_end = dna + check_len-1;
      int i;
      for(i=0; i<check_len/2; i++)
      {
         //printf("Comparing %c and %c\n", *(dna+i), *(p_end-i));
         if (*(dna+i) != dna_dna_comp(*(p_end-i)))
         {
            is_palin = 0;
            break;
         }
      }

      if (is_palin)
      {
         //printf("Found palin length %d", check_len);
         best_palin_len = check_len;
      }
   }

   return best_palin_len;
}

void free_palindrome_list(struct palindrome_list *pl)
{
   struct palindrome *p = pl->head;
   struct palindrome *tmp;
   while (p != NULL)
   {
      tmp = p->next;
      free(p);
      p=tmp;
   }
   free(pl);
}

struct palindrome_list *find_palindromes(struct fasta_node *fn)
{
   int dna_len = fn->fasta.dna.len;
   //printf("dna length %d\n", dna_len);
   struct palindrome_list *palindromes = new_palindrome_list();

   int i;
   for (i=0; i<(dna_len-2); i++)
   {
      //printf("Finding palindromes at index %d\n", i);
      int chars_remaining = dna_len - 1 - i;
      int palindrome_len = palindrome_at(fn->fasta.dna.data + i, chars_remaining);
      if (palindrome_len > 0)
      {
         //printf("Adding palindrome at %d, len %d\n", i, palindrome_len);
         struct palindrome *p = new_palindrome(i+1, palindrome_len);
         add_palindrome(palindromes, p);
      }
   }

   return palindromes;
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");
   log_fasta_list(fl);
   struct fasta_node *fn = fl->head;
   while (fn != NULL)
   {
      struct palindrome_list *pl = find_palindromes(fn);
      if (pl->head != NULL)
      {
         log_palindromes(pl);
         free_palindrome_list(pl);
      }
      fn = fn->next;
   }

   free_fasta_list(fl);
}
