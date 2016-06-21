#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fasta.h"

struct substring_start;
struct substring_start
{
   int string_id;
   int string_ix;
   struct substring_start *next;
};

struct base_tree_node;
struct base_tree_node
{
   struct substring_start *substring_starts;
   struct base_tree_node *A;
   struct base_tree_node *C;
   struct base_tree_node *G;
   struct base_tree_node *T;
};

struct base_tree_node *new_base_tree_node()
{
   struct base_tree_node *tn = malloc(sizeof(struct base_tree_node));
   tn->substring_starts = NULL;
   tn->A = NULL;
   tn->C = NULL;
   tn->G = NULL;
   tn->T = NULL;
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL) 
   {
      puts("Failed to open data file");
      return 1;
   }

   struct fasta_list fastas;
   fastas.head = NULL;
   fastas.current_node = NULL;

   ssize_t chars_read;
   char *current_line = NULL;
   size_t current_line_alloc = 0;
   while ((chars_read = getline(&current_line, &current_line_alloc, f)) > -1)
   {
      //printf("READ FROM FILE: %s\n", current_line);
      if (*current_line == '>')
      {
         // fasta name - skip '>' character
         char *name = malloc((chars_read-1)*sizeof(char));
         if (sscanf(current_line, ">%s\n", name) != 1)
         {
            puts("Failed to read line name");
            return 1;
         }
         //printf("%s\n", name);
         struct fasta_node *fn = new_fasta_node(name); 
         if (fastas.head == NULL)
         {
            // first entry
            fastas.current_node = fastas.head = fn;
         }
         else if (fastas.current_node->fasta.dna.len == 0)
         {
            puts("Found title line but current fasta is empty");
            return 1;
         }
         else
         {
            //printf("%s: %s", fastas.current_node->fasta.name, fastas.current_node->fasta.dna.data);
            fastas.current_node = fastas.current_node->next = fn;
         }
      }
      else
      {
         //printf("Adding chars to %s: %s", fastas.current_node->fasta.name, current_line);
         if (current_line[chars_read] == '\0'
           && current_line[chars_read-1] == '\n')
         {
            current_line[chars_read-1] = '\0';
         }
         else
         {
            puts("Failed end of dna string newline replacement");
            return 1;
         }
         append_chars(&(fastas.current_node->fasta.dna), current_line);
      }
   }
   free(current_line);

   struct base_tree_node *base = new_base_tree_node();
   struct fasta_node *fn = fastas.head;
   while (fn != NULL)
   {
      //printf("%s\n%s\n", fn->fasta.name, fn->fasta.dna.data);
      fn = fn->next;
   }

   free_fasta_nodes(&fastas);
}
