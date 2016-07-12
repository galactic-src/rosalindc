#include "intlist.h"

struct int_list *new_int_list()
{
   struct int_list *il = (struct int_list *)malloc(sizeof(struct int_list));
   il->count = 0;
   il->allocated = 10;
   il->values = malloc(10 * sizeof(int));
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

void log_il(struct int_list *il)
{
   int i;
   for (i=0; i<il->count; i++)
   {
      printf("%d", il->values[i]);
      if (i == il->count)
      {
         break;
      }
      putchar(' ');
   }
   putchar('\n');
}
