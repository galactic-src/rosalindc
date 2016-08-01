#include "fasta.h"

/*
 * This could be CONSIDERABLY improved by storing the data length on each node.
 * (Rather than tracing back counting parents each time to work out best parent)
 * Or if an accumulated char array was kept on each node.
 */

char DATA_A = 'A';
char DATA_C = 'C';
char DATA_G = 'G';
char DATA_T = 'T';

// 2D dynamic programming solver grid
struct dyn_grid2d
{
	int height;
	int width;
	struct grid_node *data;
};

struct grid_node;
struct grid_node
{
	void *data;
	struct grid_node *parent;
};

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

struct grid_node *get_parent_with_data(struct grid_node *node)
{
   //puts("parent with data...");
   while (node != NULL 
     && node->data == NULL)
   {
      node = node->parent;
      //printf("via: %p\t", node);
   }

   //printf("Parent with data: %p (%c)\n", node, *(char *)node->data);
   return node;
}

void set_node_data(struct grid_node *node, char c)
{
   //printf("Setting node data (%c)\n", c);
	switch(c)
	{
		case 'A':
			node->data = &DATA_A;
			break;
		case 'C':
			node->data = &DATA_C;
			break;
		case 'G':
			node->data = &DATA_G;
			break;
		case 'T':
			node->data = &DATA_T;
			break;
		default:
			printf("set_node_data: Unrecognised char %c (%x)", c, c);
			exit(1);
	}
}

void log_parent_chain(struct grid_node *node)
{
   //puts("logging parent chain");
   struct char_array *ca = malloc(sizeof(struct char_array));
   init_char_array(ca);

   if (node->data != NULL)
   {
      //printf("first node had data, %c", *(char *)(node->data));
      add_char(ca, *(char *)(node->data));
   }

   node = node->parent;

   /*if (node == NULL)
   {
      puts("node was null");
   }
   else if (node->data == NULL)
   {
      puts("node->dat awas null");
   }*/

   while (node != NULL 
     && node->data != NULL)
   {
      //printf("adding data: %c\n", *(char *)(node->data));
      add_char(ca, *(char *)(node->data));
      node = node->parent;
   }

   reverse_char_array(ca);
   printf("%s\n", ca->data);

   free_char_array(ca);
}

int get_data_len(struct grid_node *node)
{
   int len = 0;
   if (node->data != NULL)
   {
      ++len;
   }
   node = node->parent;

   while (node != NULL 
     && node->data != NULL)
   {
      ++len;
      node = node->parent;
   }

   //printf("data length: %d\n", len);

   return len;
}

void populate_grid(struct dyn_grid2d *grid, struct char_array *dna1, struct char_array *dna2)
{
	int dna1_ix; // x coord
	int dna2_ix;
	for (dna2_ix=0; dna2_ix<dna2->len-1; dna2_ix++)
	{
		//populate each row
		char c2 = dna2->data[dna2_ix];
		for (dna1_ix=0; dna1_ix<dna1->len-1; dna1_ix++)
		{
         //printf("Populating(%d, %d), grid data at %p\n", dna1_ix, dna2_ix, grid->data);

			// populate each column of the row
			char c1 = dna1->data[dna1_ix];
			struct grid_node *node = get_node(grid, dna1_ix, dna2_ix);

			if (c1 == c2)
			{
            //printf("MATCHED\n");
				set_node_data(node, c1);
			}

			if (dna1_ix == 0
			  && dna2_ix == 0)
			{
            //puts("Both 0, null parent");
				node->parent = NULL;
			}
			else if (dna1_ix == 0)
			{
				if (c1 == c2)
				{
               //puts("match in column 1, null parent");
					node->parent = NULL;
				}
				else
				{
					node->parent = get_parent_with_data(get_node(grid, dna1_ix, dna2_ix-1));
				}
			}
			else if (dna2_ix == 0)
			{
				if (c1 == c2)
				{
					node->parent = NULL;
				}
            else
				{
					node->parent = get_parent_with_data(get_node(grid, dna1_ix-1, dna2_ix));
				}
			}
			else
			{
				if (c1 == c2)
				{
					node->parent = get_parent_with_data(get_node(grid, dna1_ix-1, dna2_ix-1));
				}
            else
            {
               // get best parent
               struct grid_node *option1 = get_node(grid, dna1_ix, dna2_ix-1);
               struct grid_node *option2 = get_node(grid, dna1_ix-1, dna2_ix);
               if (get_data_len(option1) >= get_data_len(option2))
               {
                  node->parent = get_parent_with_data(option1);
               }
               else
               {
                  node->parent = get_parent_with_data(option2);
               }
            }
			}
		}
	}
}

struct grid_node *get_last_node(struct dyn_grid2d *grid)
{
   return get_node(grid, grid->width-1, grid->height-1);
   //puts("got node");
} 

void log_last(struct dyn_grid2d *grid)
{
   struct grid_node *last_node = get_last_node(grid);
   log_parent_chain(last_node);
}

int main(void)
{
	struct fasta_list *fl = parse_fasta_file("data");
	struct char_array *dna1 = &fl->head->fasta.dna;
	struct char_array *dna2 = &fl->head->next->fasta.dna;

	//puts(dna1->data);
	//puts(dna2->data);

	struct dyn_grid2d *grid = new_grid(dna1->len-1, dna2->len-1);
   populate_grid(grid, dna1, dna2);
   //puts("finished populating grid");
   log_last(grid);

	free_fasta_list(fl);
   free(grid->data);
   free(grid);
}
