#include "fasta.h"

char DATA_A = 'A';
char DATA_C = 'C';
char DATA_G = 'G';
char DATA_T = 'T';

// 2D dynamic programming solver grid
struct 2d_dyn_grid
{
	int height;
	int width;
	void *data;
};

struct grid_node;
struct grid_node
{
	void *data;
	struct grid_node *parent;
};

struct 2d_dyn_grid *new_grid(int width, int height)
{
	struct 2d_dyn_grid *grid = malloc(sizeof(struct 2d_dyn_grid));
	grid->height = height;
	grid->width = width;
	int grid_bytes = height*width*sizeof(grid_node);
	grid->data = malloc(height*width*sizeof(grid_node));
	bzero(grid->data, grid_bytes);
}

struct grid_node *get_node(struct 2d_dyn_grid *grid, int x, int y)
{
	return grid->data[y*grid->width + x];
}

void set_node_data(struct grid_node *node, char c)
{
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

void populate_grid(struct 2d_dyn_grid *grid, struct char_array *dna1, struct char_array *dna2)
{
	int dna1_ix; // x coord
	int dna2_ix;
	for (dna2_ix=0; dna2_ix<dna2->len-1; dna2_ix++)
	{
		//populate each row
		char c2 = dna2->data[dna2_ix];
		for (dna1_ix=0; dna1_ix<dna1->len-1; dna1_ix++)
		{
			// populate each column of the row
			char c1 = dna1->data[dna1_ix];
			struct grid_node *node = get_node(grid, dna1_ix, dna2_ix);

			if (c1 == c2)
			{
				set_node_data(node, c1);
			}

			if (dna1_ix == 0
			  && dna2_ix == 0)
			{
				node->parent = NULL;
			}
			else if (dna1_ix == 0)
			{
				if (c1 == c2)
				{
					node->parent = NULL;
				}
				else
				{
					
				}
			}
			else if (dna2_ix == 0)
			{
				
			}
			else
			{
				
			}
		}
	}
}

int main(void)
{
	struct fasta_list *fl = parse_fasta_file("data");
	struct char_array *dna1 = &fl->head->fasta.dna;
	struct char_array *dna2 = &fl->head->next->fasta.dna;

	puts(dna1->data);
	puts(dna2->data);

	struct 2d_dyn_grid *grid = new_grid(dna1->len, dna2->len);
	populate_grid(grid, dna1, dna2);

	free_fasta_list(fl);
}
