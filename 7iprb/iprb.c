#include <stdio.h>

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file");
      return 1;
   }

   int ihom_dom;
   int ihet;
   int ihom_rec;
   if (fscanf(f, "%d %d %d", &ihom_dom, &ihet, &ihom_rec) != 3)
   {
      printf("Something wrong with scan: hom_dom=%d, het=%d, hom_rec=%d", ihom_dom, ihet, ihom_rec);
      fclose(f);
      return 1;
   }

   fclose(f);

   double pop_size = ihom_dom + ihet + ihom_rec;
   double dom = (double)ihom_dom;
   double het = (double)ihet;
   double rec = (double)ihom_rec;
   printf("Data: hom_dom=%d, het=%d, hom_rec=%d, pop_size=%f\n", ihom_dom, ihet, ihom_rec, pop_size);

   // find prob that offspring has at least one dominant allele

/*
      dom   het   rec
dom   1     1     1
het   1     0.75  0.5     
rec   1     0.5   0
*/

   double pr_has_dom = (1.0/(pop_size*(pop_size-1))) * (dom*(dom-1.0) + 2.0*het*dom + 2.0*rec*dom + 0.75*het*(het-1.0) + rec*het);

   printf("%f\n", pr_has_dom);
   return 0;
}
