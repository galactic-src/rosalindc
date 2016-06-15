#include <stdio.h>

#define MAX_BP 10000

int base_value(char base);
int aa_hash(char c1, char c2, char c3);

char aas[] =
{     /*A*/            /*C*/            /*G*/            /*U*/
/*A*/ 'K','N','K','N', 'T','T','T','T', 'R','S','R','S', 'I','I','M','I',
/*C*/ 'Q','H','Q','H', 'P','P','P','P', 'R','R','R','R', 'L','L','L','L',
/*G*/ 'E','D','E','D', 'A','A','A','A', 'G','G','G','G', 'V','V','V','V',
/*U*/ '\0','Y','\0','Y','S','S','S','S','\0','C','W','C','L','F','L','F'
};

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file.\n");
      return 1;
   }

   char bases[MAX_BP + 1];
   int bases_read = fread(bases, sizeof(char), MAX_BP, f);
   if (bases[bases_read] != '\0' || bases[bases_read-1] != '\n' || (bases_read-1) % 3 > 0)
   {
      printf("should be 0: %d", bases[bases_read-1]);
      printf("Read %d bases\n", bases_read-1);
   }

   int aa_string_index=0;
   char aa_string[MAX_BP/3];
   int i, codon_start;
   for(i=0; i<(bases_read-1)/3; i++)
   {
      codon_start=i*3;
      printf("Switching on codon at %d, %c%c%c\n", codon_start, bases[codon_start], bases[codon_start+1], bases[codon_start+2]);
      
      int aa_index = aa_hash(bases[codon_start], bases[codon_start+1], bases[codon_start+2]);
      char aa = aas[aa_index];
      printf("Resulting AA: %c(%d) (index %d)\n", aa, aa, aa_index);
      aa_string[aa_string_index++] = aa;
   }

   puts(aa_string);

   fclose(f);
   return 0;
}

int aa_hash(char c1, char c2, char c3)
{
   printf("Base values: %d,%d,%d\n", base_value(c1)<<4, base_value(c2)<<2, base_value(c3));
   return (base_value(c1) << 4) 
        | (base_value(c2) << 2)
        | base_value(c3);
}

int base_value(char base)
{
   switch(base)
   {
      case 'A': 
         return 0;
      case 'C':
         return 1;
      case 'G':
         return 2;
      case 'U':
         return 3;
      default:
         printf("Unexpected base letter read: %c\n", base);
         return -1;
   }
}
