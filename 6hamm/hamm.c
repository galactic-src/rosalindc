#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN 1001

int main(void)
{
   char *first_string = NULL;
   char *second_string = NULL;

   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file");
      return 1;
   }

   size_t n=0;
   ssize_t line1_size = getline(&first_string, &n, f);
   if (line1_size == -1)
   {
      printf("Failure: line1 size=%lu", line1_size);
      return 1;
   }
   //printf("First length: %lu\n", line1_size);

   n=0;
   ssize_t line2_size = getline(&second_string, &n, f);
   if (line2_size == -1)
   {
      printf("Failure: line2 size=%lu", line2_size);
      free(first_string);
      return 1;
   }

   //printf("Second length: %lu\n", line2_size);
   fclose(f);

   if (line1_size != line2_size)
   {
      printf("Failure: line1 size=%lu, line2size=%lu", line1_size, line2_size);
      free(first_string);
      free(second_string);
      return 1;
   }

   int i;
   int hamming_dist=0;
   for(i=0; i<line1_size; i++)
   {
      if (first_string[i] != second_string[i])
      {
         hamming_dist++;
      }
   }
   printf("%d\n", hamming_dist);

   free(first_string);
   free(second_string);

   return 0;
}
