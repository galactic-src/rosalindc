#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>

/*
Binomial calculation to avoid overflow from https://rosettacode.org/wiki/Evaluate_binomial_coefficients#C
(Not used, in the end)
*/

static unsigned long gcd_ui(unsigned long x, unsigned long y) {
  unsigned long t;
  if (y < x) { t = x; x = y; y = t; }
  while (y > 0) {
    t = y;  y = x % y;  x = t;  /* y1 <- x0 % y0 ; x1 <- y0 */
  }
  return x;
}
 
unsigned long binomial(unsigned long n, unsigned long k) {
  unsigned long d, g, r = 1;
  if (k == 0) return 1;
  if (k == 1) return n;
  if (k >= n) return (k == n);
  if (k > n/2) k = n-k;
  for (d = 1; d <= k; d++) {
    if (r >= ULONG_MAX/n) {  /* Possible overflow */
      unsigned long nr, dr;  /* reduced numerator / denominator */
      g = gcd_ui(n, d);  nr = n/g;  dr = d/g;
      g = gcd_ui(r, dr);  r = r/g;  dr = dr/g;
      if (r >= ULONG_MAX/nr) 
      {
         printf("UNAVOIDABLE OVERFLOW");
         exit(1);
      }
      r *= nr;
      r /= dr;
      n--;
    } else {
      r *= n--;
      r /= d;
    }
  }
  return r;
}

struct geno_probs
{
   double p_AA;
   double p_Aa;
   double p_aa;
};

long dec_pow(long a, long b)
{
   if (a==1)
   {
      return 1;
   }

   if (a==2)
   {
      return a << b;
   }

   long res = 1;
   int p;
   for(p=0; p<b; p++)
   {
      res *= a;
   }

   return res;
}

long factorial(int n)
{
   if (n == 1)
   {
      return 1;
   }
   else
   {
      return n * factorial(n-1);
   }
}

long choose(int N, int R)
{
   return factorial(N)/(factorial(R) * factorial(N-R));
}

double bin_coeff(long n, long k)
{
   long bigger_factor = (k > n-k)? k : (n-k);
   long smaller_factor = n-bigger_factor;
   
   double result = 1;

   long i=(bigger_factor+1);
   while (i <= n)
   {
      result *= i++;
      printf("UP current res=%lf\n", result);
   }

   i=1;
   while (i<=smaller_factor) {
      result /= i++;
      printf("DOWN current res=%lf\n", result);
   }
   return result;
}

double bernoulli_trial(long n, long k, double p)
{
   printf("Trial: n=%ld, k=%ld, p=%lf\n", n, k, p);
   double result = bin_coeff(n, k);
   printf("Res1:%lf\n", result);
   double q = 1.0 - p;
   result *= pow(p, (double)k);
   printf("Res2:%lf\n", result);
   result *= pow(q, (double)(n-k));
   printf("Result: %lf\n", result);
   return result;
}

void calc_next_gen(struct geno_probs *gp)
{
   struct geno_probs tmp;

   tmp.p_AA = (gp->p_AA * 0.5) + (gp->p_Aa * 0.25);
   tmp.p_Aa = (gp->p_AA * 0.5) + (gp->p_Aa * 0.5) + (gp->p_aa * 0.5);
   tmp.p_aa = (gp->p_Aa * 0.25) + (gp->p_aa * 0.5);

   gp->p_AA = tmp.p_AA;
   gp->p_Aa = tmp.p_Aa;
   gp->p_aa = tmp.p_aa;
}

void log_gen(struct geno_probs *g)
{
   printf("GEN: p(AA)=%lf, p(Aa)=%lf, p(aa)=%lf\n", g->p_AA, g->p_Aa, g->p_aa);
}

double prob_AaBb(struct geno_probs *g)
{
   return g->p_Aa * g->p_Aa;
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }

   int n, k;
   if (fscanf(f, "%d %d", &k, &n) != 2)
   {
      puts("Error reading data from data file");
      fclose(f);
      exit(1);
   }

   struct geno_probs gp;
   gp.p_AA = 0.0;
   gp.p_Aa = 1.0;
   gp.p_aa = 0.0;

   int generation;
   for (generation=0; generation<k; generation++)
   {
      //log_gen(&gp);
      calc_next_gen(&gp);
   }
   //log_gen(&gp);

   fclose(f);

   double p_AaBb = prob_AaBb(&gp);

   double result = 0.0;
   int total=pow(2, k);
   printf("Total = %d", total);
   int i;
   for(i=n; i<=total; i++)
   {
      double bt = bernoulli_trial(total, i, p_AaBb);
      result += bt;
      printf("round for n=%d, %lf, total=%lf\n", i, bt, result);
   }
   printf("%lf\n", result);
}
