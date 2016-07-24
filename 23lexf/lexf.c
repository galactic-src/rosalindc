#include <stdio.h>
#include <stdlib.h>

void print_permutations(const char const *alphabet, const int alphabet_size, char * const print_buf, int chars_set, const int chars_target)
{
   int do_print = (chars_set+1 == chars_target);

   int i;
   for (i=0; i<alphabet_size; i++)
   {
      print_buf[chars_set] = alphabet[i];
      if (do_print)
      {
         /*FILE *out_f = fopen("output", "a+");
         if (out_f == NULL)
         {
            puts("Failed to open output file");
         }
         fprintf(out_f, "%s\r\n", print_buf);
         fclose(out_f);*/
         printf("%s\n", print_buf);
      }
      else
      {
         print_permutations(alphabet, alphabet_size, print_buf, chars_set+1, chars_target);
      }
   }
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("problem opening data file");
   }

   char alphabet[10];
   int alphabet_size = 0;

   int c;
   while(1)
   {
      c = fgetc(f);
      
      if (c == EOF)
      {
         puts("End of file before newline - unknown length to print");
         exit(1);
      }
      if (c == '\n')
      {
         //end of alphabet.
         break;
      }
      else if (c == ' ')
      {
         continue;
      }
      else
      {
         alphabet[alphabet_size++] = (char)(0xFF & c);
      }
   }

   int length_to_write;
   if (fscanf(f, "%d", &length_to_write) != 1)
   {
      puts("Error reading length to write");
      exit(1);
   }
   fclose(f);

   // at most we require 10 symbols plus a terminating null
   char print_buf[11];
   print_buf[length_to_write] = '\0';
   print_permutations(alphabet, alphabet_size, print_buf, 0, length_to_write);
   
   exit(0);
}


