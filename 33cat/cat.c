#include "fasta.h"

/*
 * Could improve this by a factor of ~ 4 by treating bases as 1a 1b 2a 2b where 1a is the first base, 1b is the complement of the 1st 
 * and 2a is the first other encountered, 2b its complement. This cuts down the number of nodes from the first node from 4->1 and 2nd layer from 4->3.
 * However, it makes it a bit less intuitive and it's plenty fast enough already.
 */

struct rna_bond_node;
struct rna_bond_node
{
   struct rna_bond_node *A;
   struct rna_bond_node *U;
   struct rna_bond_node *G;
   struct rna_bond_node *C;
   long bondings; //mod 1000000
};

struct rna_bond_node *new_node()
{
   struct rna_bond_node *node = malloc(sizeof(struct rna_bond_node));
   node->A = NULL;
   node->U = NULL;
   node->C = NULL;
   node->G = NULL;
   node->bondings = -1;

   return node;
}

void free_rna_bonding_tree(struct rna_bond_node *node)
{
   if (node->A != NULL)
   {
      free_rna_bonding_tree(node->A);
   }
   if (node->U != NULL)
   {
      free_rna_bonding_tree(node->U);
   }
   if (node->G != NULL)
   {
      free_rna_bonding_tree(node->G);
   }
   if (node->C != NULL)
   {
      free_rna_bonding_tree(node->C);
   }
   free(node);
}

char rna_pair(char rna_base)
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
         printf("rna_pair: Unexpected rna base [%c] (0x%x)\n", rna_base, rna_base);
         exit(1);
   }
}

struct rna_bond_node *get_bond_tree_node(char *rna, struct rna_bond_node *current)
{
   struct rna_bond_node **next;
   switch (rna[0])
   {
      case '\0':
         return current;
      case 'A':
         next = &current->A;
         break;
      case 'U':
         next = &current->U;
         break;
      case 'C':
         next = &current->C;
         break;
      case 'G':
         next = &current->G;
         break;
      default:
         printf("get_bond_tree_node: Unexpected rna base [%c] (0x%x)\n", rna[0], rna[0]);
         exit(1);
   }

   if (*next == NULL)
   {
      *next = new_node();
   }

   return get_bond_tree_node(rna+1, *next);
}

long count_bondings(struct char_array *rna, struct rna_bond_node *root)
{
   //printf("Entering count_bondings: %s\n", rna->data);
   struct rna_bond_node *node = get_bond_tree_node(rna->data, root);
   if (node->bondings > -1)
   {
      // we already calculated this one
      return node->bondings;
   }

   char looking_for = rna_pair(rna->data[0]);
   //printf("Searching for %c\n", looking_for);
   long total_bondings = 0;

   //see if base 1 matches with every 2nd base (to allow for non-overlapping, perfect matching)
   int i;
   for (i=1; i<rna->len; i+=2)
   {
      //printf("found %c\n", rna->data[i]);
      if (rna->data[i] == looking_for)
      {
         //for each match, add the product of possibilities from the two sides. calculate the smaller side first, and if it is 0, skip the second.
         if (i==1)
         {
            //puts("i==1");
            if (rna->len == 3)
            {
               total_bondings += 1;
            }
            else
            {
               struct char_array *remainder = substr(rna, 2, rna->len-3);
               total_bondings += count_bondings(remainder, root);
               free_char_array(remainder);
            }
         }
         else if (i==rna->len-2)
         {
            //puts("i==rna->len-2");
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
