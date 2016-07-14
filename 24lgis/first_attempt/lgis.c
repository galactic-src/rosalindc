#include <stdio.h>

#include "intlist.h"

//gcc lgis.c intlist.c -o lgis

struct int_list *read_data()
{
   FILE *f = fopen("data", "r");
   if (f==NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }

   int count;
   if (fscanf(f, "%d", &count) != 1)
   {
      puts("Failed to read numbers count");
      exit(1);
   }

   int i;
   struct int_list *il = new_int_list();
   while(fscanf(f, "%d", &i) != EOF)
   {
      il_add(il, i);
      
   }
   if (il->count < count)
   {
      printf("got %d numbers, but expected %d\n", il->count, count);
      exit(1);
   }
   
   return il;
}

void end_run(struct int_list **current_run, struct int_list **longest_run)
{
   if ((*current_run)->count > (*longest_run)->count)
   {
      // new longest - swap lists
      struct int_list *tmp = *longest_run;
      *longest_run = *current_run;
      *current_run = tmp;
   }
   // ready for new run
   il_clear(*current_run);
}

int main(void)
{
   struct int_list *il = read_data();
   log_il(il);

   struct int_list *longest_inc = new_int_list();
   struct int_list *longest_dec = new_int_list();

   struct int_list *longest_inc_tmp = new_int_list();
   struct int_list *longest_dec_tmp = new_int_list();

   int ix;
   int current;
   int next;
   for (ix=0; ix<il->count; ix++)
   {
      next = il_get(il, ix);
      if (ix > 0)
      {
         printf("next: %d, current: %d\n", next, current);
         // end existing runs as necessary
         if (next <= current)
         {
            end_run(&longest_inc_tmp, &longest_inc);
         }
         if (next >= current)
         {
            end_run(&longest_dec_tmp, &longest_dec);
         }
      }

      // add this number to existing runs
      il_add(longest_inc_tmp, next);
      printf("inc: ");
      log_il(longest_inc_tmp);
      il_add(longest_dec_tmp, next);
      printf("dec: ");
      log_il(longest_dec_tmp);
      current = next;
   }

   log_il(longest_inc);
   log_il(longest_dec);

   return 0;
}

