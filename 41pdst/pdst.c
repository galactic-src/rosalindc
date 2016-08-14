#include <stdio.h>
#include "fasta.h"

struct distance_matrix
{
   int height;
   int width;
   double *data;
};

double hamm_p(char *s1, char *s2)
{
   // should be equal
   int len = strlen(s1);
   int hamm = 0;
   while (*s1 != '\0')
   {
      if (*s1++ != *s2++)
      {
         hamm++;
      }
   }

   return (double)hamm/(double)len;
}

struct distance_matrix *new_matrix(int height, int width)
{
   struct distance_matrix *dm = malloc(sizeof(struct distance_matrix));
   dm->height = height;
   dm->width = width;
   dm->data = malloc(sizeof(double) * height * width);
   return dm;
}

void print_matrix(struct distance_matrix *dm)
{
   int i, j;
   for (j=0; j<dm->height; j++)
   {
      for (i=0; i<dm->width; i++)
      {
         printf("%.4lf\t", dm->data[j*dm->width + i]);
      }
      putchar('\n');
   }
}

int main(void)
{
   struct fasta_list *fl = parse_fasta_file("data");
   int count = fl_len(fl);
   //printf("%d\n", count);

   struct fasta **fasta_array = malloc(sizeof(void *) * count);
   bzero(fasta_array, sizeof(void *) * count);

   struct fasta_node *fn = fl->head;
   int i=0;
   while (fn != NULL)
   {
      fasta_array[i++] = &fn->fasta;
      fn = fn->next;
   }

   struct distance_matrix *dm = new_matrix(count, count);

   int j;
   for (i=0; i<count; i++)
   {
      char *s1 = fasta_array[i]->dna.data;
      // this never puts anything where j=i, but that is correct
      for (j=0; j<i; j++)
      {
         char *s2 = fasta_array[j]->dna.data;
         double d = hamm_p(s1, s2);
         dm->data[i*count + j] = dm->data[j*count + i] = d;
      }
      dm->data[i*count + i] = 0.0L;
   }

   print_matrix(dm);   
   
}
