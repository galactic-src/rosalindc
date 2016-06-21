#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fasta.h"

struct base_tree_node;
struct base_tree_node
{
   int fasta_id; // last fasta to get to this node.
   struct base_tree_node *A;
   struct base_tree_node *C;
   struct base_tree_node *G;
   struct base_tree_node *T;
   struct char_array *base_list;
};

struct base_tree_node *new_base_tree_node(char *parent_bases, char *base)
{
   struct base_tree_node *tn = malloc(sizeof(struct base_tree_node));
   tn->fasta_id = -1;
   tn->A = NULL;
   tn->C = NULL;
   tn->G = NULL;
   tn->T = NULL;

   char *base_list = copystr(parent_bases);
   tn->base_list = attach_str(base_list);
   append_chars(tn->base_list, base);

   return tn;
}

void free_base_tree_node(struct base_tree_node *n)
{
   free_char_array(n->base_list);
   free(n);
}

/**
 * Add a single string to the suffix tree
 */
void suffix_tree_add(char *c, struct base_tree_node *n, int id)
{
   while (*c != '\0')
   {
      n->fasta_id = id;
      switch(*c)
      {
         case 'A':
            if (n->A == NULL)
            {
               if (id == 0)
               {
                  n->A = new_base_tree_node(n->base_list->data, "A");
               }
               else
               {
                  // substring not matched by other fastas
                  return;
               }
            }
            n = n->A;
            break;
         case 'C':
            if (n->C == NULL)
            {
               if (id == 0)
               {
                  n->C = new_base_tree_node(n->base_list->data, "C");
               }
               else
               {
                  // substring not matched by other fastas
                  return;
               }
            }
            n = n->C;
            break;
         case 'G':
            if (n->G == NULL)
            {
               if (id == 0)
               {
                  n->G = new_base_tree_node(n->base_list->data, "G");
               }
               else
               {
                  // substring not matched by other fastas
                  return;
               }
            }
            n = n->G;
            break;
         case 'T':
            if (n->T == NULL)
            {
               if (id == 0)
               {
                  n->T = new_base_tree_node(n->base_list->data, "T");
               }
               else
               {
                  // substring not matched by other fastas
                  return;
               }
            }
            n = n->T;
            break;
         default:
            printf("unexpected character in DNA data: '%c'", *c);
            exit(1);
            break;
      }
      c++;
   }
}

void remove_subtree(struct base_tree_node *n)
{
   if (n->A != NULL)
   {
      remove_subtree(n->A);
   }
   if (n->C != NULL)
   {
      remove_subtree(n->C);
   }
   if (n->G != NULL)
   {
      remove_subtree(n->G);
   }
   if (n->T != NULL)
   {
      remove_subtree(n->T);
   }
   free_base_tree_node(n);
}

void prune_tree(struct base_tree_node *n, int fasta_id)
{
   if (n->A != NULL)
   {
      if (n->A->fasta_id < fasta_id)
      {
         remove_subtree(n->A);
         n->A = NULL;
      }
      else
      {
         prune_tree(n->A, fasta_id);
      }
   }
   if (n->C != NULL)
   {
      if (n->C->fasta_id < fasta_id)
      {
         remove_subtree(n->C);
         n->C = NULL;
      }
      else
      {
         prune_tree(n->C, fasta_id);
      }
   }
   if (n->G != NULL)
   {
      if (n->G->fasta_id < fasta_id)
      {
         remove_subtree(n->G);
         n->G = NULL;
      }
      else
      {
         prune_tree(n->G, fasta_id);
      }
   }
   if (n->T != NULL)
   {
      if (n->T->fasta_id < fasta_id)
      {
         remove_subtree(n->T);
         n->T = NULL;
      }
      else
      {
         prune_tree(n->T, fasta_id);
      }
   }
}

void find_longest_substring(struct base_tree_node *n, int level, char **longest, int *longest_level)
{
   //printf("Level %d, fasta_id %d, string %s\n", level, n->fasta_id, n->base_list->data);
   if (n->A != NULL)
   {
      //puts("A");
      find_longest_substring(n->A, level+1, longest, longest_level);
   }
   if (n->C != NULL)
   {
      //puts("C");
      find_longest_substring(n->C, level+1, longest, longest_level);
   }
   if (n->G != NULL)
   {
      //puts("G");
      find_longest_substring(n->G, level+1, longest, longest_level);
   }
   if (n->T != NULL)
   {
      //puts("T");
      find_longest_substring(n->T, level+1, longest, longest_level);
   }

   if (n->A == NULL && n->C == NULL && n->G == NULL && n->T == NULL) 
   {
      //printf("leaf at lvl %d (%s) - ", level, n->base_list->data);
      if (level > *longest_level)
      {
         *longest_level = level;
         *longest = n->base_list->data;
      }
   }

   //puts("UP");
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
   fclose(f);

   struct base_tree_node *base = new_base_tree_node("", "");
   struct fasta_node *fn = fastas.head;
   int fasta_id = 0;
   while (fn != NULL)
   {
      //printf("%s\n%s\n", fn->fasta.name, fn->fasta.dna.data);
      char *start = fn->fasta.dna.data;
      while (*start != '\0')
      {
         //printf("next substring: %s\n", start);
         suffix_tree_add(start, base, fasta_id);
         start++;
      }

      prune_tree(base, fasta_id);
      
      fn = fn->next;
      fasta_id++;
   }

   char *longest = NULL;
   int longest_level=-1;
   find_longest_substring(base, 0, &longest, &longest_level);
   
   printf("%s\n", longest);

   remove_subtree(base);
   free_fasta_nodes(&fastas);
}
