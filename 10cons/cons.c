#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct base_tally;

struct base_tally {
   int A_count;
   int C_count;
   int G_count;
   int T_count;
   struct base_tally *next;
};

void tally_add_base(struct base_tally *t, char base)
{
   //printf("Tally add %c\n", base);
   switch(base)
   {
      case 'A':
         t->A_count++;
         break;
      case 'C':
         t->C_count++;
         break;
      case 'G':
         t->G_count++;
         break;
      case 'T':
         t->T_count++;
         break;
      default:
         printf("Failed to add base %c\n", base);
         break;
   }
}

void print_tally(struct base_tally *t)
{
   printf("Tally: A=%d, C=%d, G=%d, T=%d\n", t->A_count, t->C_count, t->G_count, t->T_count);
}

struct base_tally *new_tally()
{
   struct base_tally *tally = malloc(sizeof(struct base_tally));
   if (tally == NULL)
   {
      printf("failed to allocate memory for base tallies");
      return NULL;
   }

   tally->A_count = 0;
   tally->C_count = 0;
   tally->G_count = 0;
   tally->T_count = 0;
   tally->next = NULL;

   return tally;
}

int tally_get_A(struct base_tally *t)
{
   return t->A_count;
}
int tally_get_C(struct base_tally *t)
{
   return t->C_count;
}
int tally_get_G(struct base_tally *t)
{
   return t->G_count;
}
int tally_get_T(struct base_tally *t)
{
   return t->T_count;
}

struct base_tally *next_tally(struct base_tally *current)
{
   if (current->next == NULL)
   {
      current->next = new_tally();
   }

   return current->next;
}

char tally_consensus(struct base_tally *t)
{
   int current_max = t->A_count;
   char consensus = 'A';

   if (t->C_count > current_max)
   {
      current_max = t->C_count;
      consensus = 'C';
   }
   if (t->G_count > current_max)
   {
      current_max = t->G_count;
      consensus = 'G';
   }
   if (t->T_count > current_max)
   {
      current_max = t->T_count;
      consensus = 'T';
   }
   return consensus;
}

static void print_profile_line(char base, struct base_tally *t, int (*get_method)(struct base_tally *))
{
   fprintf(stdout, "\n%c:", base);
   while (1)
   {
      printf(" %d", get_method(t));
      if ((t = t->next) == NULL)
      {
         break;
      }
   }
   putchar('\n');
}

void print_profile(struct base_tally *list_head)
{
   struct base_tally *tally = list_head;
   print_profile_line('A', tally, tally_get_A);
   print_profile_line('C', tally, tally_get_C);
   print_profile_line('G', tally, tally_get_G);
   print_profile_line('T', tally, tally_get_T);
}

struct fasta {
   char *name;
   char *DNA;
   int DNA_len;
   int DNA_alloc;
};

void free_fasta(struct fasta *f)
{
   free(f->name);
   free(f->DNA);
   free(f);
}

void fasta_add_DNA(struct fasta *fs, char *DNA, int count)
{
   while (1)
   {
      //printf("Realloc check: DNA len=%d, count=%d, alloc=%d\n", fs->DNA_len, count, fs->DNA_alloc);
      //ensure space for terminating NUL
      if (fs->DNA_alloc > fs->DNA_len + count + 1)
      {
         break;
      }
      fs->DNA_alloc *= 2;
      //printf("enlarge DNA buffer to %d\n", fs->DNA_alloc);
      fs->DNA = realloc(fs->DNA, fs->DNA_alloc * sizeof(char));
   }

   //printf("Adding DNA: %s\tto: %s\n", DNA, fs->DNA);
   memcpy(fs->DNA + fs->DNA_len, DNA, count*sizeof(char));
   *(fs->DNA + fs->DNA_len + count * sizeof(char)) = '\0';
   fs->DNA_len += count;
   //printf("Result: %s\n", fs->DNA);
}

int main(void)
{
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("Failed to open data file");
      return 1;
   }

   struct base_tally *first_base_tally = new_tally();
   struct base_tally *tally = first_base_tally;

   int fasta_len = 0;

   char *current_line = NULL;
   size_t current_line_alloc = 0;
   int chars_read;
   struct fasta *current_fasta = NULL;
   while ((chars_read = getline(&current_line, &current_line_alloc, f)) > -1)
   {
      //puts("Start of while loop");
      if (*current_line == '>')
      {
         //current_line is a new entry, so deal with the current entry first, then create the new FASTA
         if (current_fasta != NULL)
         {
            int fasta_ix;
            for (fasta_ix=0; fasta_ix<current_fasta->DNA_len; fasta_ix++)
            {
               tally_add_base(tally, (current_fasta->DNA)[fasta_ix]);
               if (fasta_ix < current_fasta->DNA_len -1)
               {
                  tally = next_tally(tally);
               }
            }

            //printf("Pretending to deal with FASTA: %s (count=%d)\n", current_fasta->name, current_fasta->DNA_len);
            //printf("DNA: %s\n", current_fasta->DNA);
            free_fasta(current_fasta);
            tally = first_base_tally;
         }

         //puts("allocating fasta");
         current_fasta = malloc(sizeof(struct fasta));
         current_fasta->name = malloc(chars_read * sizeof(char));
         if (sscanf(current_line, ">%s", current_fasta->name) != 1)
         {
            printf("failed to read FASTA name: %s", current_line);
            fclose(f);
            free(current_line);
            return 1;
         }
         
         current_fasta->DNA_len = 0;
         current_fasta->DNA_alloc = 10;
         current_fasta->DNA = malloc(10 * sizeof(char));
      }
      else
      {
         if (current_fasta != NULL)
         {
            //ignore trailing newline
            fasta_add_DNA(current_fasta, current_line, chars_read-1);
         }
         else
         {
            printf("Line started with something other than '>' but current_fasta is NULL");
         }
      }
   }
   if (current_fasta != NULL)
   {
      int fasta_ix;
      for (fasta_ix=0; fasta_ix<current_fasta->DNA_len; fasta_ix++)
      {
         tally_add_base(tally, (current_fasta->DNA)[fasta_ix]);
         if (fasta_ix < current_fasta->DNA_len -1)
         {
            tally = next_tally(tally);
         }
      }
      free_fasta(current_fasta);
   }

   free(current_line);
   fclose(f);
   
   for (tally = first_base_tally; tally != NULL; tally = tally->next)
   {
      putchar(tally_consensus(tally));
   }
   
   print_profile(first_base_tally);

   struct base_tally *tmp_tally;
   tally = first_base_tally;
   while (tally != NULL)
   {
      tmp_tally = tally->next;
      free(tally);
      tally = tmp_tally;
   }
}
