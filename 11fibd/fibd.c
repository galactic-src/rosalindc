#include <stdio.h>
#include <stdlib.h>

#define N_MAX 100

// each rabbit pair 2 months or older (F(n-2)) creates a single offspring in the next generation
// each rabbit pair that was born m months ago dies, and is subtracted from the total

// this implementation won't work for all sample data - it overflows. Needs a bigger number implementation.

struct mortal_fib
{
   int m;             // rabbits die after m months
   int calculated;    // how many generations have been calculated. (calculated-1 total available, calculated-2 births available). 
   unsigned long adults[N_MAX];  // totals at start of each month. 
   unsigned long children[N_MAX]; // number born in each month
};

struct mortal_fib *new_mortal_fib(int m)
{
   struct mortal_fib *mf = malloc(sizeof(struct mortal_fib));
   mf->m = m;
   mf->adults[0] = 0;
   mf->children[0] = 1;
   mf->calculated = 1;
   return mf;
}

void next_gen(struct mortal_fib *mf)
{
   mf->calculated++;

   unsigned long deaths = 0;
   int dying_gen = mf->calculated - 1 - mf->m;
   if (dying_gen >= 0)
   {
      // all individuals born m months ago now die
      deaths = mf->children[dying_gen];
      //printf("Deaths: %d\n", deaths);
   }

   // all adults last month now have children, regardless of whether they die or not
   // this only works because calculated starts at 1 and has been incremented by now
   //printf("Next children = adults[mf->calculated-2] = %d\n", mf->adults[mf->calculated-2]);
   mf->children[mf->calculated-1] = mf->adults[mf->calculated-2];

   mf->adults[mf->calculated-1] = mf->adults[mf->calculated-2] - deaths + mf->children[mf->calculated-2];
   
}

void print_gen(struct mortal_fib *mf)
{
   int ix = mf->calculated - 1; 
   printf("Gen %d: children=%lu, adults=%lu, total=%lu\n", mf->calculated, mf->children[ix], mf->adults[ix], mf->children[ix] + mf->adults[ix]);
}

unsigned long nth_gen(struct mortal_fib *mf, int gen)
{
   while(1)
   {
      print_gen(mf);
      if (mf->calculated >= gen)
      {
         return mf->children[gen-1] + mf->adults[gen-1];
      }
      next_gen(mf);
   }
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      return 1;
   }

   int n, m;
   if (fscanf(f, "%d %d", &n, &m) != 2)
   {
      printf("Problem reading data file, n=%d, m=%d", n, m);
      fclose(f);
      return 1;
   }
   
   fclose(f);

   struct mortal_fib *mf = new_mortal_fib(m);

   printf("%lu\n", nth_gen(mf, n));

   free(mf);
}
