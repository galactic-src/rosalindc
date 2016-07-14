#include <stdio.h>

#include "intlist.h"

struct int_list_node;
struct int_list_node
{
   int value;
   struct int_list *inc_values;
   struct int_list *dec_values;
   struct int_list_node *next;
};

struct int_list_node *new_int_list_node(int i, struct int_list *inc_values, struct int_list *dec_values)
{
   struct int_list_node *n = malloc(sizeof(struct int_list_node));
   n->value = i;
   il_add(inc_values, i);
   n->inc_values = inc_values;
   il_add(dec_values, i);
   n->dec_values = dec_values;
   n->next = NULL;
}

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

int main(void)
{
   struct int_list *data = read_data();
   if (data->count == 0)
   {
      puts("no data provided");
      exit(0);
   }

   struct int_list_node *head = new_int_list_node(data->values[0], new_int_list(), new_int_list());

   struct int_list_node *best_prev_dec;
   struct int_list_node *best_prev_inc;
   struct int_list_node *iln;
   int i; //next data value
   int ix;
   for(ix=0; ix<data->count; ix++)
   {
      best_prev_dec = NULL;
      best_prev_inc = NULL;

      i=data->values[ix];

      // check each existing node to work out the best one to follow
      iln = head;
      while(1)
      {
         if (iln->inc_values->values[iln->inc_values->count-1] < i)
         {
            if (best_prev_inc == NULL || best_prev_inc->inc_values->count < iln->inc_values->count)
            {
               best_prev_inc = iln;
            }
         }
         if (iln->dec_values->values[iln->dec_values->count-1] > i)
         {
            if (best_prev_dec == NULL || best_prev_dec->dec_values->count < iln->dec_values->count)
            {
               best_prev_dec = iln;
            }
         }

         if (iln->next == NULL)
         {
            break;
         }
         else
         {
            iln = iln->next;
         }
      }

      struct int_list_node *new_node = new_int_list_node(i,
            best_prev_inc == NULL ? new_int_list() : il_clone(best_prev_inc->inc_values),
            best_prev_dec == NULL ? new_int_list() : il_clone(best_prev_dec->dec_values));

      iln->next = new_node;
   }

   struct int_list *longest_dec = head->dec_values;
   struct int_list *longest_inc = head->inc_values;
   //log_il(head->inc_values);
   iln = head->next;
   
   while(iln != NULL)
   {
      //log_il(iln->dec_values);
      if (iln->dec_values->count > longest_dec->count)
      {      
         longest_dec = iln->dec_values;
         //printf("new longest dec:");
         //log_il(longest_dec);
      }
      if (iln->inc_values->count > longest_inc->count)
      {
         longest_inc = iln->inc_values;
         //printf("new longest inc:");
         //log_il(longest_inc);
      }
      iln = iln->next;
   } 

   //printf("Results:\n");
   log_il(longest_inc);
   log_il(longest_dec);

   return 0;
}  
