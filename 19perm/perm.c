#include <stdlib.h>
#include <stdio.h>

void log_ints(int number, int *ints)
{
   int print_ix;
   for (print_ix=0; print_ix<number; print_ix++)
   {
      printf("%d", ints[print_ix]);
      if (print_ix<number-1)
      {
         putchar(' ');
      }
   }

   putchar('\n');
}

void swap_ints(int *ints, int ix1, int ix2)
{
   int tmp = ints[ix1];
   ints[ix1] = ints[ix2];
   ints[ix2] = tmp;
}

/**
 * taken from http://stackoverflow.com/questions/9148543/program-to-print-permutations-of-given-elements
 * strategy is recursively to swap the first element with each of the later elements in turn, for each index
 * of the array.
 * 'end' doesn't change, but start grows by 1 each time. Once start==end, we have reached the final element
 * and no more permutation is necessary.
 */
void permute(int *ints, int start, int end)
{
    if(start == end)
    {
        log_ints(end, ints);
        return;
    }

    permute(ints, start + 1, end);
    int i;
    for(i = start + 1; i < end; i++)
    {
        swap_ints(ints, start, i);
        permute(ints, start + 1, end);
        swap_ints(ints, start, i);
    }
}

unsigned long factorial(int i)
{
   if (i<0)
   {
      printf("Failed to take factorial of %d\n", i);
      exit(1);
   }
   else if (i==0 || i==1)
   {
      return 1L;
   }
   else
   {
      unsigned long result = 1L;
      int j;
      for(j=2; j<=i; j++)
      {
         result *= j;
      }
      return result;
   }
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      puts("Expected arg number < 8");
      exit(1); 
   }

   int number = atoi(argv[1]);
   //printf("%d\n", number);

   int *ints = malloc(number * sizeof(int));
   int ix;
   for (ix=0; ix<number; ix++)
   {
      ints[ix] = ix+1;
   }

   //log_ints(number, ints);
   printf("%lu\n", factorial(number));
   permute(ints, 0, number);

   return 0;
}

