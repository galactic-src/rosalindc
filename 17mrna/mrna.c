#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

const int A_CODS = 4;
const int C_CODS = 2;
const int D_CODS = 2;
const int E_CODS = 2;
const int F_CODS = 2;
const int G_CODS = 4;
const int H_CODS = 2;
const int I_CODS = 3;
const int K_CODS = 2;
const int L_CODS = 6;
const int M_CODS = 1; //START
const int N_CODS = 2;
const int P_CODS = 4;
const int Q_CODS = 2;
const int R_CODS = 6;
const int S_CODS = 6;
const int T_CODS = 4;
const int V_CODS = 4;
const int W_CODS = 1;
const int Y_CODS = 2;
const int STOP_CODS = 3;

int get_cods_count(char aa)
{
   //printf("char 0x%x (%c)\n", aa, aa);
   switch (aa)
   {
      case 'A':
         return A_CODS;
      case 'C':
         return C_CODS;
      case 'D':
         return D_CODS;
      case 'E':
         return E_CODS;
      case 'F':
         return F_CODS;
      case 'G':
         return G_CODS;
      case 'H':
         return H_CODS;
      case 'I':
         return I_CODS;
      case 'K':
         return K_CODS;
      case 'L':
         return L_CODS;
      case 'M':
         return M_CODS;
      case 'N':
         return N_CODS;
      case 'P':
         return P_CODS;
      case 'Q':
         return Q_CODS;
      case 'R':
         return R_CODS;
      case 'S':
         return S_CODS;
      case 'T':
         return T_CODS;
      case 'V':
         return V_CODS;
      case 'W':
         return W_CODS;
      case 'Y':
         return Y_CODS;
      default:
         // e.g. newline at end of file. Don't change number.
         //printf("Unrecognised amino acid 0x%x (%c)\n", aa, aa);
         return 1;
   }
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file\n");
      exit(1);
   }
   
   char aas[1501];
   if (fgets(aas, 1500, f) == NULL)
   {
      puts("error reading from file\n");
      exit(1);
   }
   fclose(f);

   unsigned long result = 1UL;
   unsigned long max_pre_mul = (ULONG_MAX/6)-1; //prevent overflow

   char *aa = aas;
   int cod_count;
   while (*aa)
   {
      cod_count = get_cods_count(*aa);
      //printf("%c->%d\n",*aa, cod_count);
      if (cod_count != 1)
      {
         if (result > max_pre_mul)
         {
            result %= 1000000;
         }
         result *= cod_count;
      }

      //printf("result=%lu\n", proper_result);

      aa++;
   }

   if (result > max_pre_mul)
   {
      result %= 1000000;
   }
   result *= STOP_CODS;
   result %= 1000000;

   printf("%lu\n", result);
}
