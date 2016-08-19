#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "fasta.h"
#include "str.h"

// gcc edit.c fasta.c 

/*
  - C P E O       -  C  P  E  O
- . . . . .    -  0 D1 D2 D3 D4
A . . . . .    A I1 M1 D2 I3 D4
E . . . . .    E I2 I2 M2  2 D3
I . . . . .    I I3 I3 I3 I3 M3
O . . . . .    O I4 I4 I4 I4  3

tracing parents:
O-EPC 
 I DM
OIE-A

and reverse:
CPE-O
MD I 
A-EIO

total cost 3
could reverse strings before we start to make life easier printing

from previous node on grid, if chars don't match, add weight of change
from above = deletion
from above and left = mutation
from left = insertion
*/

#define INS_WT 1
#define MUT_WT 1
#define DEL_WT 1

char act_ins = 'I';
char act_del = 'D';
char act_mut = 'M';
char act_non = ' ';

struct grid_node
{
   char c1;
   char c2;
   char type;
   int total;
   struct grid_node *parent;
};

struct dyn_grid2d
{
	int height;
	int width;
	struct grid_node *data;
};

// a possible way to get to a new node
struct action
{
   char c1;
   char c2;
   char type;
   int total;
   struct grid_node *parent;
};

struct action *new_action(char c1, char c2, char type, struct grid_node *parent, int total)
{
   struct action *a = malloc(sizeof(struct action));
   a->c1 = c1;
   a->c2 = c2;
   a->type = type;
   a->total = total;
   a->parent = parent;
   return a;
}

// an exhaustive list of possible ways to get to a new node
struct action_list
{
   int count;
   int alloc;
   struct action **actions;
};

struct action_list *new_action_list()
{
   struct action_list *al = malloc(sizeof(struct action_list));
   al->count = 0;
   al->alloc = 5;
   al->actions = malloc(5 * sizeof(struct grid_node *));
}

void al_add(struct action_list *list, struct action *a)
{
   if (list->alloc == list->count)
   {
      puts("Error: would have added too many actions");
      exit(1);
   }

   list->actions[list->count++] = a;
}

void al_free(struct action_list *al)
{
   int i;
   for (i=0; i<al->count; i++)
   {
      free(al->actions[i]);
   }
   free(al->actions);
   free(al);
}

void apply_best_action(struct grid_node *node, struct action_list *al)
{
   node->total = INT_MAX;

   int i;
   struct action *best = NULL;
   struct action *a;
   for (i=0; i<al->count; i++)
   {
      a = al->actions[i];
      if (best == NULL
        || a->total < best->total)
      {
         best = a;
      }
   }

   node->c1 = best->c1;
   node->c2 = best->c2;
   node->total = best->total;
   node->type = best->type;
   node->parent = best->parent;
}

struct dyn_grid2d *new_grid(int width, int height)
{
   //printf("Making %dx%d grid\n", width, height);

   struct dyn_grid2d *grid = malloc(sizeof(struct dyn_grid2d));
   grid->height = height;
   grid->width = width;
   int grid_bytes = height*width*sizeof(struct grid_node);
   grid->data = malloc(grid_bytes);
   //printf("grid data %p-%p\n", grid->data, ((char *)(grid->data)) + grid_bytes);
   bzero(grid->data, grid_bytes);

   return grid;
}

struct grid_node *get_node(struct dyn_grid2d *grid, int x, int y)
{
   //printf("getting node at %p\n", ((struct grid_node *)grid->data) + (y*grid->width + x));
   return ((struct grid_node *)grid->data) + (y*grid->width + x);
}

struct grid_node *get_last_node(struct dyn_grid2d *grid)
{
   return get_node(grid, grid->width-1, grid->height-1);
} 

void populate_node(struct grid_node *node, struct dyn_grid2d *grid, int p1_ix, int p2_ix)
{
   if (!p1_ix && !p2_ix)
   {
      //puts("first node");
      // top-left, both dummy columns, always matches
      node->total = 0;
      node->type = act_non;
      node->parent = NULL;
      return;
   }

   struct action_list *actions = new_action_list();

   if (p2_ix)
   {
      //puts("Added ins option");
      struct grid_node *parent = get_node(grid, p1_ix, p2_ix-1);
      al_add(actions, new_action(node->c1, '-', act_ins, parent, parent->total + INS_WT));
   }

   if (p1_ix)
   {
      //puts("Added del option");
      struct grid_node *parent = get_node(grid, p1_ix-1, p2_ix);
      al_add(actions, new_action('-', node->c2, act_del, parent, parent->total + DEL_WT));
   }

   if (p1_ix 
     && p2_ix)
   {
      struct grid_node *parent = get_node(grid, p1_ix-1, p2_ix-1);

      if (node->c1 == node->c2)
      {
         //puts("Added matched option");
         //Matching chars
         al_add(actions, new_action(node->c1, node->c2, act_non, parent, parent->total));
      }
      else
      {
         //puts("Added mut option");
         al_add(actions, new_action(node->c1, node->c2, act_mut, parent, parent->total + MUT_WT));
      }
   }

   apply_best_action(node, actions);
   al_free(actions);
}

void populate_grid(struct dyn_grid2d *grid, struct char_array *p1, struct char_array *p2)
{
	int p1_ix; // x coord
	int p2_ix;
	for (p2_ix=0; p2_ix<p2->len; p2_ix++)
	{
		//populate each row
		char c2 = (p2_ix == 0) ? '-' : p2->data[p2_ix-1];

		for (p1_ix=0; p1_ix<p1->len; p1_ix++)
		{
         //printf("Populating(%d, %d), grid data at %p\n", dna1_ix, dna2_ix, grid->data);

			// populate each column of the row
			char c1 = (p1_ix == 0) ? '-' : p1->data[p1_ix-1];

			struct grid_node *node = get_node(grid, p1_ix, p2_ix);
         node->c1 = c1;
         node->c2 = c2;

         //printf("populating (%d, %d)\n", p1_ix, p2_ix);
         populate_node(node, grid, p1_ix, p2_ix);
		}
	}
}

int main()
{
	struct fasta_list *fl = parse_fasta_file("data");
	struct char_array *p1 = &fl->head->fasta.dna;
   //printf("%s\n", p1->data);
   reverse_char_array(p1);
	struct char_array *p2 = &fl->head->next->fasta.dna;
   //printf("%s\n", p2->data);
   reverse_char_array(p2);

   //use len rather than len-1 so we can include a dummy column at the start of each axis
	struct dyn_grid2d *grid = new_grid(p1->len, p2->len);
   populate_grid(grid, p1, p2);
   
   /* printing to show conclusion
   struct grid_node *n = get_last_node(grid);
   while (n != NULL)
   {
      printf("%c", n->c1);
      n = n->parent;
   }
   putchar('\n');n = get_last_node(grid);
   while (n != NULL)
   {
      printf("%c", n->type);
      n = n->parent;
   }
   putchar('\n');n = get_last_node(grid);
   while (n != NULL)
   {
      printf("%c", n->c2);
      n = n->parent;
   }
   putchar('\n');
   */

   printf("%d\n", get_last_node(grid)->total);
   
   free(grid->data);
   free(grid);

	free_fasta_list(fl);
}
