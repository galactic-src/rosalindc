#include <stdio.h>

char *data_filename = "data";

struct base_tally {
	int A;
	int C;
	int G;
	int T;
};

void count_bases(FILE *f)
{
	struct base_tally tally = { .A=0, .C=0, .G=0, .T=0 };

	char c;
	while (fread(&c, 1, 1, f) > 0)
	{
		switch (c)
		{
			case 'A':
				tally.A++;
				break;
			case 'C':
				tally.C++;
				break;
			case 'G':
				tally.G++;
				break;
			case 'T':
				tally.T++;
				break;
			default:
				break;
		}	
 	}

	printf("%d %d %d %d\n", tally.A, tally.C, tally.G, tally.T);
}

int main(void)
{
	FILE *f = fopen(data_filename, "rb");
	if (f == NULL)
	{
		printf("Failed to open data file. Does it exist?");
		return 1;
	}
	else
	{
		count_bases(f);
		fclose(f);
	}

	return 0;
}

