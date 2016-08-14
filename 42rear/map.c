#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "map.h"

struct hashmap *new_map()
{
   struct hashmap *m = malloc(sizeof(struct hashmap));
   bzero(m, sizeof(struct hashmap));
   return m;
}

int hash_ints(int *ints)
{
   return (ints[0] * ints[1] 
        + ints[2] * ints[3]
        + ints[4] * ints[5]
        + ints[6] * ints[7]
        + ints[8] * ints[9]) % BUCKETS;
}

void hm_add(struct hashmap *hm, int *ints)
{
   int *new_ints = malloc(10 * sizeof(int));
   memcpy(new_ints, ints, 10 * sizeof(int));

   struct hm_node *bucket = &hm->nodes[hash_ints(ints)];
   
   // append to the end of the linked list for the appropriate bucket
   if (bucket->ints == NULL)
   {
      bucket->ints = new_ints;
   }
   else
   {
      while(bucket->next != NULL)
      {
         bucket = bucket->next;
      }
      struct hm_node *to_add = bucket->next = malloc(sizeof(struct hm_node));
      to_add->ints = new_ints;
      to_add->next = NULL;
   }
}

int hm_contains(struct hashmap *hm, int *ints)
{
   struct hm_node *node = hm->nodes + hash_ints(ints);
   if (node->ints == NULL)
   {
      // nothing in this bucket
      return 0;
   }
   else
   {
      while (1)
      {
         if (!memcmp(ints, node->ints, 10*sizeof(int)))
         {
            return 1;
         }
         else if (node->next == NULL)
         {
            return 0;
         }
         else
         {
            node = node->next;
         }
      }
   }
}

void hm_free_node_list(struct hm_node *bucket)
{
   //bucket doesn't need freeing but any linked nodes do
   struct hm_node *n = bucket->next;
   struct hm_node *n_ref;
   while (n != NULL)
   {
      n_ref = n;
      n = n->next;
      free(n_ref->ints);
      free(n_ref);
   }
}

void hm_free(struct hashmap *hm)
{
   struct hm_node *bucket = hm->nodes;
   int ix;
   for(ix=0; ix<BUCKETS; ix++)
   {
      hm_free_node_list(bucket);
      if (bucket->ints != NULL)
      {
         free(bucket->ints);
      }
      bucket++;
   }
   free(hm);
}

/*int main(void)
{
   struct hashmap *map = new_map();
   int example_ints[10] = {0,0,0,0,0,0,0,0,0,0};
   int example_ints2[10] = {0,1,0,0,0,0,0,0,0,0};
   int example_ints3[10] = {0,2,0,0,0,0,0,0,0,0};
   int contains = hm_contains(map, example_ints);
   printf("Empty map contains zeros: %d\n", contains);
   hm_add(map, example_ints);
   contains = hm_contains(map, example_ints);
   printf("After addition: %d\n", contains);
   contains = hm_contains(map, example_ints2);
   printf("After addition of other: %d\n", contains);
   hm_add(map, example_ints2);
   contains = hm_contains(map, example_ints);
   printf("After addition: %d\n", contains2);
   contains = hm_contains(map, example_ints2);
   printf("After addition of other: %d\n", contains);
}*/
