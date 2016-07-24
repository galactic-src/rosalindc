#include <stdlib.h>

#include "intlist.h"

/**
 * Definite scope for improvement by using lists rather than array lists to keep track of nodes in trees
 * On the other hand, maybe should be actually building the trees themselves!
 */

struct int_edge
{
   int from;
   int to;   
};

struct int_edge_list
{
   int count;
   int alloc;
   struct int_edge *data;
};

struct int_edge_list *new_int_edge_list()
{
   struct int_edge_list *new = malloc(sizeof(struct int_edge_list));
   new->count = 0;
   new->alloc = 10;
   new->data = malloc(10 * sizeof(struct int_edge));
}

void iel_ensure_len(struct int_edge_list *iel, int len)
{
   if (len > iel->alloc)
   {
      while(len > iel->alloc)
      {
         iel->alloc *= 2;
      }
      iel->data = realloc(iel->data, iel->alloc * sizeof(struct int_edge));
   }   
}

void iel_add(struct int_edge_list *iel, int from, int to)
{
   iel_ensure_len(iel, iel->count+1);
   iel->data[iel->count].from = from;
   iel->data[iel->count].to = to;
   iel->count++;
}

void log_iel(struct int_edge_list *iel)
{
   int i;
   for (i=0; i<iel->count; i++)
   {
      printf("%d->%d\n", iel->data[i].from, iel->data[i].to);
   }
}

void free_iel(struct int_edge_list *iel)
{
   free(iel->data);
   free(iel);
}

struct il_list
{
   int count;
   int alloc;
   struct int_list *data;
};

struct il_list *new_il_list()
{
   struct il_list *new = malloc(sizeof(struct il_list));
   new->count = 0;
   new->alloc = 10;
   new->data = malloc(10 * sizeof(struct int_list));
}

void ill_ensure_len(struct il_list *ill, int len)
{
   if (len > ill->alloc)
   {
      while (len > ill->alloc)
      {
         ill->alloc *= 2;
      }
      ill->data = realloc(ill->data, ill->alloc * sizeof(struct int_list));
   }
}

struct int_list *add_new_il(struct il_list *ill)
{
   ill_ensure_len(ill, ill->count+1);
   struct int_list *next = &(ill->data[ill->count]);
   ill->count++;

   next->count = 0;
   next->allocated = 10;
   next->values = malloc(10 * sizeof(int));

   return next;
}

void free_ill(struct il_list *ill)
{
   int i;
   for (i=0; i<ill->count; i++)
   {
      free(ill->data[i].values);
   }
   free(ill->data);
   free(ill);
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f==NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }

   int count;
   if (fscanf(f, "%d", &count) != 1)
   {
      printf("Failed to read node count");
      exit(1);
   }

   struct int_edge_list *iel = new_int_edge_list();

   int tmp_from;
   int tmp_to;
   while (fscanf(f, "%d %d", &tmp_from, &tmp_to) == 2)
   {
      iel_add(iel, tmp_from, tmp_to);
   }
   fclose(f);

   //puts("finished reading data file");

   struct il_list *ill = new_il_list();

   int added = 0;
   struct int_list *current;
   while(1)
   {
      //puts("Next tree");
      current = add_new_il(ill);

      //puts("Next tree acquired");

      // iel should only contain remaining edges, so can always take the first one
      il_add(current, iel->data[0].from);
      il_add(current, iel->data[0].to);

      do
      {
         added = 0;
         int ix;
         //printf("iterating over IEL's %d edges\n", iel->count);
         for (ix=0; ix<iel->count; ix++)
         {
            // if the current list contains one but not the other of an edge, add the edge
            struct int_edge ie = iel->data[ix];
            if (il_contains(current, ie.from))
            {
               if (!il_contains(current, ie.to))
               {
                  il_add(current, ie.to);
                  added++;
               }
            }
            else if (il_contains(current, ie.to))
            {
               il_add(current, ie.from);
               added++;
            }
         }
      }
      // continue until we have gone through the entire list without matching any more
      while (added > 0);

      //puts("populating new list");

      // make a new list of remaining edges
      struct int_edge_list *iel_new = new_int_edge_list();
      int ix;
      struct int_edge ie;
      for (ix=0; ix<iel->count; ix++)
      {
         ie = iel->data[ix];
         if (!il_contains(current, ie.from))
         {
            iel_add(iel_new, ie.from, ie.to);
         }
      }

      //puts("finished populating new list");

      free_iel(iel);
      iel=iel_new;
      if (iel->count == 0)
      {
         //all done
         break;
      }
   }

   //puts("finished trees");

   // Now account for any additional nodes that weren't on any edge
   int nodes_found=0;
   int il_ix;
   for(il_ix=0; il_ix<ill->count; il_ix++)
   {
      struct int_list *il = &ill->data[il_ix];
      nodes_found += il->count;
   }

   int unclaimed_nodes = count - nodes_found;
   //printf("%d unclaimed nodes\n", unclaimed_nodes);

   /*int ix;
   for(ix=0; ix<ill->count; ix++)
   {
      printf("list %d\n", ix);
      log_il(&ill->data[ix]);
   }*/

   // take 1 tree as 'root'. Need one edge per unclaimed node, plus one edge for each additional tree.
   int edges_required = unclaimed_nodes + ill->count - 1;
   printf("%d\n", edges_required);

   free_iel(iel);
   free_ill(ill);
}
