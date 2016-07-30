#include <string.h>

#include "fasta.h"
#include "intlist.h"

int main(void)
{
	struct fasta_list *fl = parse_fasta_file("data");
	char *dna = fl->head->fasta.dna.data;
	int dna_len = strlen(dna);
	//puts(dna);

	int failure_array_sz = dna_len*sizeof(int);
	int *failure_array = malloc(failure_array_sz);
	bzero(failure_array, failure_array_sz);

	//try matching from each index in the string
	int start_ix=1;
	while (dna[start_ix] != '\0')
	{
		//printf("starting at %c\n", dna[start_ix]);
		if (dna[start_ix] == *dna)
		{
			int ix=0;
			while (dna[start_ix+ix] == dna[ix])
			{
				//printf("matched %c\n", dna[start_ix+ix]);
				if (!failure_array[start_ix+ix])
				{
					failure_array[start_ix+ix] = ++ix;
				}
				else
				{
					++ix;
				}
			}
			//printf("didn't match %c and %c\n", dna[start_ix+ix], dna[ix]);
		}
		start_ix++;
	}

	free_fasta_list(fl);

	struct int_list *il = malloc(sizeof(struct int_list));
	il->count=dna_len;
	il->allocated=dna_len;
	il->values = failure_array;
	log_il(il);
	il_free(il);
}
