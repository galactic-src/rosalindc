#include <stdio.h>
#include <stdlib.h>

#define INT_LIST_INITIAL_SIZE 10

struct int_list {
   int count;
   int allocated;
   int *values;
};

struct int_list *new_int_list()
{
   struct int_list *il = (struct int_list *)malloc(sizeof(struct int_list));
   il->count = 0;
   il->allocated = INT_LIST_INITIAL_SIZE;
   il->values = malloc(INT_LIST_INITIAL_SIZE * sizeof(int));
   return il;
}

int il_get(struct int_list *il, int index)
{
   if (il->count < (index+1))
   {
      printf("Out of range request: count %d, index %d", il->count, index);
   }

   return (il->values)[index];
}

int il_add(struct int_list *il, int i)
{
   if (il->count == il->allocated)
   {
      il->allocated *= 2;
      //printf("Reallocating to %d\n", il->allocated);
      il->values = realloc(il->values, il->allocated * sizeof(int));
   }
   il->values[il->count++] = i;
}

int il_free(struct int_list *il)
{
   free(il->values);
   free(il);
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      return 1;
   }

   char *super_string = NULL;
   size_t n = 0;
   ssize_t super_string_len = getline(&super_string, &n, f);
   if (super_string_len == -1)
   {
      puts("Error reading super string");
      free(super_string);
      fclose(f);
      return 1;
   }

   char *sub_string = NULL;
   n = 0;
   ssize_t sub_string_len = getline(&sub_string, &sub_string_len, f);
   if (sub_string_len == -1)
   {
      puts("Error reading sub string");
      free(super_string);
      free(sub_string);
      fclose(f);
      return 1;
   }

   //printf("superstring length:%lu, substring length:%lu\n", super_string_len, sub_string_len);

   struct int_list *il = new_int_list();

   int start_index;
   char *possible_substring = super_string;
   char *substring_comparison = sub_string;
   for (start_index=0; start_index < super_string_len; start_index++)
   {
      //printf("starting at %d\n", start_index);
      for (possible_substring = super_string + start_index; possible_substring < super_string + start_index + sub_string_len-1; possible_substring++)
      {
         //printf("Comparing %c and %c\n", *possible_substring, *substring_comparison); 
         if (*(possible_substring) != *(substring_comparison++))
         {
            break;
         }
         if (possible_substring == super_string + start_index + sub_string_len-2)
         {
            //printf("Adding int to list: %d\n", start_index+1);
            il_add(il, (start_index+1));
         }
      }
      substring_comparison = sub_string;
   }
   
   if (il->count > 0)
   {
      int index;
      for (int i=0; i<(il->count); i++)
      {
         printf("%d ", il_get(il, i));
      }
      putchar('\n');
   }
   else
   {
      puts("Didn't find substring");
   }

   il_free(il);
   return 0;
}
