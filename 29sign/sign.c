#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// n<=6

void log_signed_ints(int number, int *ints)
{
   int num_to_print;
   int sign_mask;
   for (sign_mask=0; sign_mask<pow(2,number); sign_mask++)
   {
      int print_ix;
      for (print_ix=0; print_ix<number; print_ix++)
      {
         if ((1 << print_ix) & sign_mask)
         {
            num_to_print = ints[print_ix];
         }
         else
         {
            num_to_print = -ints[print_ix];
         }

         printf("%d", num_to_print);
         if (print_ix<number-1)
         {
            putchar(' ');
         }
      }
   putchar('\n');
   }
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
void permute_signed(int *ints, int start, int end)
{
    if(start == end)
    {
        log_signed_ints(end, ints);
        return;
    }

    permute_signed(ints, start + 1, end);
    int i;
    for(i = start + 1; i < end; i++)
    {
        swap_ints(ints, start, i);
        permute_signed(ints, start + 1, end);
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

int *get_populated_ints(int number)
{
   int *ints = malloc(number * sizeof(int));
   int ix;
   for (ix=0; ix<number; ix++)
   {
      ints[ix] = ix+1;
   }
   return ints;
}

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      puts("Expected argc == 2");
      exit(1);
   }

   int number = atoi(argv[1]);
   if (number < 1 || number > 6)
   {
      printf("expected a number between 1 and 6 but got [%s]\n", argv[1]);
      exit(1);
   }
   
   int *ints = get_populated_ints(number);
   int permutations = factorial(number) * pow(2, number);
   printf("%d\n", permutations);

   permute_signed(ints, 0, number);
   return 0;
}
