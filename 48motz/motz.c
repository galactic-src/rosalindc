#include "fasta.h"

const char BASE_A = 'A';
const char BASE_U = 'U';
const char BASE_C = 'C';
const char BASE_G = 'G';

const char *pointer_for(char c)
{
   switch (c)
   {
      case 'A':
         return &BASE_A;
      case 'U':
         return &BASE_U;
      case 'C':
         return &BASE_C;
      case 'G':
         return &BASE_G;
      default:
         printf("Unexpected char for pointer request %c (%x)\n", c, c);
         exit(1);
   }
}

/* when comparing bonding patterns, the first base encountered is recorded as first and first_cmp can be
 * calculated. The first other base encountered is second. This makes parsing slightly longer but
 * reduces required nodes by a factor of 2^2.
 */ 
struct base_mapping
{
   const char *first;
   const char *first_cmp;
   const char *second;
   const char *second_cmp;
};

struct base_mapping *new_base_mapping()
{
   struct base_mapping *bm = malloc(sizeof(struct base_mapping));
   bm->first = NULL;
   bm->first_cmp = NULL;
   bm->second = NULL;
   bm->second_cmp = NULL;
   return bm;
}

struct rna_bond_node
{
   struct rna_bond_node *first;
   struct rna_bond_node *first_cmp;
   struct rna_bond_node *second;
   struct rna_bond_node *second_cmp;
   long bondings; //mod 1000000
};

struct rna_bond_node *new_node()
{
   struct rna_bond_node *node = malloc(sizeof(struct rna_bond_node));
   node->first = NULL;
   node->first_cmp = NULL;
   node->second = NULL;
   node->second_cmp = NULL;
   node->bondings = -1;

   return node;
}

void free_rna_bonding_tree(struct rna_bond_node *node)
{
   if (node->first != NULL)
   {
      free_rna_bonding_tree(node->first);
   }
   if (node->first_cmp != NULL)
   {
      free_rna_bonding_tree(node->first_cmp);
   }
   if (node->second != NULL)
   {
      free_rna_bonding_tree(node->second);
   }
   if (node->second_cmp != NULL)
   {
      free_rna_bonding_tree(node->second_cmp);
   }
   free(node);
}

char rna_cmp(char rna_base)
{
   switch(rna_base)
   {
      case 'A':
         return 'U';
      case 'U':
         return 'A';
      case 'C':
         return 'G';
      case 'G':
         return 'C';
      default:
         printf("rna_cmp: Unexpected rna base [%c] (0x%x)\n", rna_base, rna_base);
         exit(1);
   }
}

struct rna_bond_node *get_bond_tree_node(char *rna, struct rna_bond_node *current, struct base_mapping *map)
{
   struct rna_bond_node **next;
   char c = rna[0];
   if (c == '\0')
   {
      return current;
   }
   else if (map->first == NULL)
   {
      map->first = pointer_for(c);
      map->first_cmp = pointer_for(rna_cmp(c));
      next = &current->first;
   }
   else if (c == *map->first)
   {
      next = &current->first;
   }
   else if (c == *map->first_cmp)
   {
      next = &current->first_cmp;
   }
   else if (map->second == NULL)
   {
      map->second = pointer_for(c);
      map->second_cmp = pointer_for(rna_cmp(c));
      next = &current->second;
   }
   else if (c == *map->second)
   {
      next = &current->second;
   }
   else if (c == *map->second_cmp)
   {
      next = &current->second_cmp;
   }
   else
   {      
      printf("get_bond_tree_node: Unexpected rna base [%c] (0x%x)\n", c, c);
      puts("Map:");
      if(map->first == NULL)
      {
         puts("first: NULL");
      }
      else
      {
         printf("first: %c (%x)\n", *map->first, *map->first);
      }
      if(map->first_cmp == NULL)
      {
         puts("first_cmp: NULL");
      }
      else
      {
         printf("first_cmp: %c (%x)\n", *map->first_cmp, *map->first_cmp);
      }
      if(map->second == NULL)
      {
         puts("second: NULL");
      }
      else
      {
         printf("second: %c (%x)\n", *map->second, *map->second);
      }
      if(map->second_cmp == NULL)
      {
         puts("second_cmp: NULL");
      }
      else
      {
         printf("second_cmp: %c (%x)\n", *map->second_cmp, *map->second_cmp);
      }
      exit(1);
   }

   if (*next == NULL)
   {
      *next = new_node();
   }

   return get_bond_tree_node(rna+1, *next, map);
}

long count_bondings(struct char_array *rna, struct rna_bond_node *root)
{
   //printf("Counting for: \"%s\"\n", rna->data);
   if (rna->len == 1)
   {
      //puts("Length 1 (0 bases)");
      return 1;
   }
   if (rna->len == 2)
   {
      //puts("Length 2 (1 base)");
      return 1;
   }
   else if (rna->len == 3)
   {
      //puts("Length 3 (2 base)");
      if (rna->data[0] == rna_cmp(rna->data[1]))
      {
         // 2 options: either with or without the bond
         return 2;
      }
      else
      {
         // no bond possible
         return 1;
      }
   }

   //printf("Entering count_bondings: %s\n", rna->data);
   struct base_mapping *mapping = new_base_mapping();
   struct rna_bond_node *node = get_bond_tree_node(rna->data, root, mapping);
   free(mapping);
   if (node->bondings > -1)
   {
      // we already calculated this one
      return node->bondings;
   }

   long total_bondings = 0;

   //count bondings when the first base isn't bonded
   struct char_array *ca = substr(rna, 1, rna->len-2);
   total_bondings += count_bondings(ca, root);
   free_char_array(ca);

   char looking_for = rna_cmp(rna->data[0]);
   //printf("Searching for %c\n", looking_for);

   //see if base 0 matches with each base (to allow for non-overlapping matching)
   int i;
   for (i=1; i<rna->len; i++)
   {
      //printf("found %c\n", rna->data[i]);
      if (rna->data[i] == looking_for)
      {
         //for each match, add the product of possibilities from the two sides. calculate the smaller side first, and if it is 0, skip the second.
         if (i==1)
         {
            //puts("i==1");
            struct char_array *remainder = substr(rna, 2, rna->len-3);
            total_bondings += count_bondings(remainder, root);
            free_char_array(remainder);
         }
         else if (i==rna->len-1)
         {
            //puts("i==rna->len-1");
            struct char_array *remainder = substr(rna, 1, rna->len-3);
            total_bondings += count_bondings(remainder, root);
            free_char_array(remainder);
         }
         else
         {
            //printf("Splitting, i==%d\n", i);
            // split into two subproblems
            //puts("side1");
            struct char_array *remainder1 = substr(rna, 1, i-1);
            long opts1 = count_bondings(remainder1, root) % 1000000;
            free_char_array(remainder1);

            //puts("side2");
            struct char_array *remainder2 = substr(rna, i+1, rna->len-i-2);
            long opts2 = count_bondings(remainder2, root) % 1000000;
            free_char_array(remainder2);

            total_bondings += (opts1 * opts2) % 1000000;
         }
      }
   }
   //printf("%ld bondings for %s\n", total_bondings, rna->data);

   //memoize result
   node->bondings = total_bondings % 1000000;

   //printf("total bondings: %ld\n", total_bondings);

   return node->bondings;
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");
   struct char_array *rna = clone_ca(&fl->head->fasta.dna);
   free_fasta_list(fl);

   struct rna_bond_node *root = new_node();

   long perfect_bondings = count_bondings(rna, root);
   free_char_array(rna);
   free_rna_bonding_tree(root);

   printf("%ld\n", perfect_bondings);

   return 0;
}
