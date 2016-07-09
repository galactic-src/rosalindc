#include <stdlib.h>
#include <stdio.h>

double get_mi_mass(char aa, double default_mass)
{
   switch(aa)
   {
      case 'A':
         return 71.03711;
      case 'C':
         return 103.00919;
      case 'D':
         return 115.02694;
      case 'E':
         return 129.04259;
      case 'F':
         return 147.06841;
      case 'G':
         return 57.02146;
      case 'H':
         return 137.05891;
      case 'I':
         return 113.08406;
      case 'K':
         return 128.09496;
      case 'L':
         return 113.08406;
      case 'M':
         return 131.04049;
      case 'N':
         return 114.04293;
      case 'P':
         return 97.05276;
      case 'Q':
         return 128.05858;
      case 'R':
         return 156.10111;
      case 'S':
         return 87.03203;
      case 'T':
         return 101.04768;
      case 'V':
         return 99.06841;
      case 'W':
         return 186.07931;
      case 'Y':
         return 163.06333;
      default:
         //printf("Unexpected amino acid: %c (0x%x)", aa, aa);
         return default_mass;
   }
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      exit(1);
   }

   double total_mass = 0.0;

   char c;
   while((c = fgetc(f)) != EOF)
   {
      total_mass += get_mi_mass(c, 0.0);
   }
   

   printf("%lf\n", total_mass);
}
