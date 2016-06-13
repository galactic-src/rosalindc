#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argv, char *argc[])
{
	if (argv < 3)
	{
		printf("Expect arguments n k\n");
		return 1;
	}

	int target_n = atoi(argc[1]);
	int k = atoi(argc[2]);

	if (target_n == 0 || k == 0)
	{
		printf("Either entered 0 or an unparsable value: n=%d k=%d\n", target_n, k);
		return 1;
	}

	int n=1;
	int f_n = 1;	
	int f_n_plus_1 = 1;

	int temp;	
	while (n < target_n-1)
	{
		temp = f_n_plus_1 + f_n * k;
		f_n = f_n_plus_1;
		f_n_plus_1 = temp;
		++n;
		printf("F%d = %d\n", n, f_n);
	}

	printf("F%d = %d\n", n+1, f_n_plus_1);
}
