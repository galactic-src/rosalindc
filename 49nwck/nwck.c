#include <stdio.h>
#include <stdlib.h>

#include "intlist.h"

struct question_data
{
   char *nwck;
   char *to_find1;
   char *to_find2;
};

struct qd_list
{
   int count;
   // max 40 questions
   struct question_data **questions;
};

void free_qd_list(struct qd_list *list)
{
   int i;
   for (i=0; i<list->count; i++)
   {
      free(list->questions[i]->nwck);
      free(list->questions[i]->to_find1);
      free(list->questions[i]->to_find2);
      free(list->questions[i]);
   }
   free(list->questions);
   free(list);
}

struct qd_list *new_qd_list()
{
   struct qd_list *list = malloc(sizeof(struct qd_list));
   list->count = 0;
   list->questions = malloc(40 * sizeof(struct question_data));
   return list;
}

void qd_list_add(struct qd_list *list, struct question_data *qd)
{
   //printf("adding to index %d\n", list->count);
   list->questions[list->count++] = qd;
}

void log_qd(struct question_data *data)
{
   printf("nwck: %s\n", data->nwck);
   printf("find distance between: %s and %s\n", data->to_find1, data->to_find2);
}

void log_qd_list(struct qd_list *list)
{
   int i;
   for (i=0; i<list->count; i++)
   {
      printf("question %d:\n", i);
      log_qd(list->questions[i]);
   }
}

struct qd_list *read_data()
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      perror("failed to open file:");
      exit(1);
   }

   struct qd_list *questions = new_qd_list();

   char *nwck, *to_find1, *to_find2;
   int fscanf_result;
   while ((fscanf_result = fscanf(f, "%ms;", &nwck)) == 1)
   {
      if (fscanf(f, "%ms %ms\n\n", &to_find1, &to_find2) != 2)
      {
         puts("second line didn't get two results");
         exit(1);
      }
      
      struct question_data *qd = malloc(sizeof(struct question_data));
      qd->nwck = nwck;
      qd->to_find1 = to_find1;
      qd->to_find2 = to_find2;

      qd_list_add(questions, qd);
   }
   if (fscanf_result != EOF)
   {
      printf("fscanf_result ended on %d\n", fscanf_result);
      exit(1);
   }

   fclose(f);

   return questions;
}

int main(void)
{
   struct qd_list *questions = read_data();

   //log_qd_list(questions);

   struct int_list *results = new_int_list();

   int i;
   for (i=0; i<questions->count; i++)
   {
      
   }
   
   free_qd_list(questions);

   log_il(results);
   il_free(results);
}
