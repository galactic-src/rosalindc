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

int il_shorter(struct int_list *il1, struct int_list *il2)
{
   return il1 != NULL
         && il2 != NULL
         && il1->count < il2->count;
}
int il_longer(struct int_list *il1, struct int_list *il2)
{
   return il1 != NULL
         && il2 != NULL
         && il1->count > il2->count;
}

int il_free(struct int_list *il)
{
   free(il->values);
   free(il);
}

void il_clear(struct int_list *il)
{
   il->count = 0;
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

struct int_list *il_clone(const struct int_list *src)
{
   struct int_list *dest = malloc(sizeof(struct int_list));
   memcpy(dest, src, sizeof(struct int_list));
   dest->values = malloc(dest->allocated * sizeof(int));
   memcpy(dest->values, src->values, src->count * sizeof(int));
   return dest;
}

int il_contains(struct int_list *il, int needle)
{
   int i;
   for (i=0; i<il->count; i++)
   {
      if (il->values[i] == needle)
      {
         return 1;
      }
   }
   return 0;
}
