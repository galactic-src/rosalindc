#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "map.h"
#include "intlist.h"

struct ia_count
{
   int *ints;
   int perm_count;
   struct ia_count *next;
};

struct ia_count *new_count_node()
{
   struct ia_count *n = malloc(sizeof(struct ia_count));
   bzero(n, sizeof(struct ia_count));
   n->ints = malloc(sizeof(int) * 10);
   bzero(n->ints, sizeof(int) * 10);
   return n;
}

struct ia_node
{
   int *ints;
   struct ia_node *next;
};

struct ia_node *new_node()
{
   struct ia_node *n = malloc(sizeof(struct ia_node));
   bzero(n, sizeof(struct ia_node));
   n->ints = malloc(sizeof(int) * 10);
   bzero(n->ints, sizeof(int) * 10);
   return n;
}

int ia_contains(struct ia_node *n, int permutation[10])
{
   while (n != NULL)
   {
      if (memcmp(n->ints, permutation, sizeof(int) * 10) == 0)
      {
         return 1;
      }
      n = n->next;
   }
   return 0;
}

struct int_array_pair
{
   int ints1[10];
   int ints2[10];
};

struct iap_list
{
   int len;
   int alloc;
   struct int_array_pair **data;
};

struct iap_list *pair_list()
{
   struct iap_list *list = malloc(sizeof(struct iap_list));
   list->len = 0;
   list->alloc = 5;
   list->data = malloc(5*sizeof(struct int_array_pair));
   bzero(list->data, 5*sizeof(struct int_array_pair));
   return list;
}

// should only have 5 pairs, so always safe not to realloc
void add_pair(struct iap_list *list, struct int_array_pair *pair)
{
   list->data[list->len++] = pair;
}

/**
 * First invert array 1, then use it to calculate sigma, stored in array2
 * The transformation from sigma to 1 2 3 4 5 6 7 8 9 10 (identity) is equivalent
 * to the transformation from array1->array2
 */
void invert_arrays(struct int_array_pair *arrays)
{
   int tmp[10];

   int i, j;
   for (i=0; i<10; i++)
   {
      j = arrays->ints1[i];
      tmp[j-1] = i+1;
   }

   memcpy(arrays->ints1, tmp, 10*sizeof(int));

   for (i=0; i<10; i++)
   {
      tmp[i] = arrays->ints1[arrays->ints2[i]-1];
   }

   memcpy(arrays->ints2, tmp, 10*sizeof(int));
}

// a breakpoint is an adjacent pair of numbers (including ^ and $)
int count_breakpoints(int ints[10])
{
   //printf("Breakpoints: ");
   int breakpoints = 0;
   if (ints[0] != 1)
   {
      //printf("-1 ");
      ++breakpoints;
   }
   for (int i=0; i<9; i++)
   {
      int diff = (ints[i] - ints[i+1]);
      if (diff != 1 
        && diff != -1)
      {
         //printf("%d ", i);
         ++breakpoints;
      }
   }
   if (ints[9] != 10)
   {
      //printf("10");
      ++breakpoints;
   }

   //putchar('\n');
   return breakpoints;
}

void log_array(int ints[10])
{
   int i;
   for (i=0; i<10; i++)
   {
      printf("%d", ints[i]);
      if (i<9)
      {
         putchar(' ');
      }
   }
   putchar('\n');
}

struct int_array_pair *new_pair()
{
   struct int_array_pair *pair = malloc(sizeof(struct int_array_pair));
   bzero(pair, sizeof(struct int_array_pair));
   return pair;
}

void reverse_section(int ints[10], int start_ix, int end_ix)
{
   int tmp;
   while (end_ix > start_ix)
   {
      tmp = ints[end_ix];
      ints[end_ix] = ints[start_ix];
      ints[start_ix] = tmp;
      end_ix--;
      start_ix++;
   }
}

struct iap_list *parse_data()
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }
   struct iap_list *list = pair_list();
   struct int_array_pair *datapair = NULL;
   int res;
   while (1)
   {
      if (datapair == NULL)
      {
         datapair = new_pair();
      }
      
      if ((res = fscanf(f, 
         "%d %d %d %d %d %d %d %d %d %d", 
         datapair->ints1, 
         datapair->ints1+1, 
         datapair->ints1+2, 
         datapair->ints1+3,
         datapair->ints1+4, 
         datapair->ints1+5, 
         datapair->ints1+6,
         datapair->ints1+7, 
         datapair->ints1+8, 
         datapair->ints1+9)) == 10)
      {
         if ((res = fscanf(f, 
            "%d %d %d %d %d %d %d %d %d %d", 
            datapair->ints2, 
            datapair->ints2+1, 
            datapair->ints2+2, 
            datapair->ints2+3,
            datapair->ints2+4, 
            datapair->ints2+5, 
            datapair->ints2+6,
            datapair->ints2+7, 
            datapair->ints2+8, 
            datapair->ints2+9)) != 10)
         {
            puts("populated 1 but not the other of a data pair");
            exit(1);
         }
         //log_array(datapair->ints1);
         //log_array(datapair->ints2);

         add_pair(list, datapair);
         datapair = NULL;
      }
      else if (res == 0)
      {
         //newline
         continue;
      }
      else if (res == EOF)
      {
         free(datapair);
         break;
      }
      else
      {
         printf("fscanf returned %d\n", res);
         exit(1);
      }
   }
   //puts("Closing");
   fclose(f);
   return list;   
}

/**
 * Get the minimum number of reversals to get from this array to ordered 1-10
 * maintain a list of permutations 'tried' and a list of permutations to try 
 * (with the number of reversals performed)
 */
int calculate_distance(struct ia_count *to_try_head, struct ia_count *to_try_tail, struct hashmap *tried)
{
   // don't accept worse than the current best number of breakpoints
   int accept_bp = 11;

   if (count_breakpoints(to_try_head->ints) == 0)
   {
      // special case: already sorted
      free(to_try_head->ints);
      free(to_try_head);
      return 0;
   }

   while (to_try_head != NULL)
   {
      //puts("NEXT");
      // pop first from to_try list
      struct ia_count *popped = to_try_head;
      int *ints = popped->ints;
      int reversals = popped->perm_count;
      if (to_try_head == to_try_tail)
      {
         // popped last from list, shouldn't happen
         to_try_tail = NULL;
      }
      to_try_head = to_try_head->next;

      //printf("POPPED struct @ %p, ints @ %p\n", popped, ints);
      free(popped);

      //printf("Looking for a difference of %d\n", accept_bp_diff);

      // try every reversal that doesn't create a breakpoint and that
      // doesn't result in a permutation we have already queued at some point
      int i, j;
      for (i=0; i<9; i++)
      {
         if (i==0)
         {
            if (ints[0] == 1)
            {
               continue;
            }
         }
         else if (ints[i] - ints[i-1] == 1
            || ints[i] - ints[i-1] == -1)
         {
            continue;
         }
         
         for (j=i+1; j<10; j++)
         {
            if (j==9)
            {
               if (ints[9] == 10)
               {
                  break;
               }
            }
            else if (ints[j+1] - ints[j] == 1
               || ints[j+1] - ints[j] == -1)
            {
               continue;
            }
            //printf("Trying perm (%d,%d)\n", i, j);

            struct ia_count *count_n = new_count_node();
            count_n->perm_count = reversals+1;
            memcpy(count_n->ints, ints, 10*sizeof(int));
            reverse_section(count_n->ints, i, j);
            count_n->next = NULL;
            //log_array(count_n->ints);

            int new_breakpoints = count_breakpoints(count_n->ints);
            //printf("counted breakpoints: %d\n", new_breakpoints);
            if (new_breakpoints == 0)
            {
               // finished
               struct ia_count *next;
               struct ia_count *to_free = to_try_head;
               while (to_free != NULL)
               {
                  next = to_free->next;
                  free(to_free->ints);
                  free(to_free);
                  to_free = next;
               }

               free(count_n->ints);
               free(count_n);
               free(ints);

               return reversals+1;
            }
            else if (new_breakpoints <= accept_bp
               && !hm_contains(tried, count_n->ints)) 
            {
               //printf("ACCEPTED (%d)\n", new_breakpoints);
               if (new_breakpoints < accept_bp)
               {
                  accept_bp = new_breakpoints;
                  //printf("*** DOWN TO %d BREAKPOINTS\n", accept_bp);
               }

               //printf("Keeping (%d, %d, ->%d): ", i, j, new_breakpoints);
               //log_array(count_n->ints);
               // we'll keep this to try building from
               if (to_try_head == NULL)
               {
                  // this becomes only node in list
                  to_try_head = count_n;
               }
               else
               {
                  // append to end of list
                  to_try_tail->next = count_n;
               }
               to_try_tail = count_n;

               // record that we shouldn't try this one again
               hm_add(tried, ints);
            }
            else
            {
               // not keeping it
               free(count_n->ints);
               free(count_n);
            }
         }
      }
      //printf("Freeing ints @ %p\n", ints);
      //printf(".");
      free(ints);
   }
   
   puts("Failed to resolve permutation (shouldn't happen)");
   exit(1);
}

int main(void)
{
   struct iap_list *list = parse_data();
   //puts("finished parsing");
   struct int_list *results = new_int_list();
   int pair_ix;
   for (pair_ix=0; pair_ix<list->len; pair_ix++)
   {
      struct int_array_pair *pair = list->data[pair_ix];
      invert_arrays(pair);

      struct ia_count *to_try = new_count_node();
      memcpy(to_try->ints, pair->ints2, 10*sizeof(int));

      struct hashmap *tried = new_map();

      int distance = calculate_distance(to_try, to_try, tried);
      il_add(results, distance);
      hm_free(tried);
      free(pair);
   }

   puts("Results:");
   log_il(results);
   il_free(results);

   free(list->data);
   free(list);
}

