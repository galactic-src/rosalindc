#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fasta.h"

struct fasta_node *new_fasta_node(char *name)
{
   struct fasta_node *fn = malloc(sizeof(struct fasta_node));

   fn->fasta.name = name;
   init_char_array(&(fn->fasta.dna));
   fn->next = NULL;

   return fn;
}

void free_fasta_node(struct fasta_node *fn)
{
   free(fn->fasta.dna.data);
   free(fn->fasta.name);
   free(fn);
}

void log_fasta_node(struct fasta_node *fn)
{
   printf("%s\n%s\n", fn->fasta.name, fn->fasta.dna.data);
}

void free_fasta_list(struct fasta_list *fl)
{
   struct fasta_node *node = fl->head;
   struct fasta_node *tmp;
   while(node != NULL)
   {
      tmp = node->next;
      free_fasta_node(node);
      node = tmp;
   }

   free(fl);
}

struct fasta_list *new_fasta_list()
{
   struct fasta_list *fl = malloc(sizeof(struct fasta_list));
   fl->current_node = NULL; 
   fl->head = NULL;

   return fl;
}

void log_fasta_list(struct fasta_list *fl)
{
   fl->current_node = fl->head;
   while (fl->current_node != NULL)
   {
      log_fasta_node(fl->current_node);
      fl->current_node = fl->current_node->next;
   }
}

static int check_next_char(FILE *f, char cmp)
{
    int c = fgetc(f);
    ungetc(c, f);

    return c == cmp;
}

static int is_title(FILE *stream)
{
   //printf("Checking is title\n");
   return check_next_char(stream, '>');
}

static int is_end(FILE *stream)
{
   return check_next_char(stream, EOF);
}

struct fasta_node *read_fasta_node(FILE *f, size_t *alloc, char**line)
{
   if (!is_title(f))
   {
      //puts("Wasn't title");
      return NULL;
   }
   
   ssize_t chars_read;
   if ((chars_read = getline(line, alloc, f)) == -1)
   {
      puts("Expected title line but getline failed or got EOF");
      exit(1);
   }
   //printf("line (before trim): %s\n", *line);
   (*line)[chars_read-1] = '\0';
   //printf("line (after trim): %s\n", *line);
   //printf("Title: %s\n", *line);
   
   struct fasta_node *fn = new_fasta_node(clone_str(*(line) + 1));
   //printf("Made fasta node for %s\n", fn->fasta.name);
   while (!is_title(f) && !is_end(f))
   {
      if ((chars_read = getline(line, alloc, f)) == -1)
      {
         puts("getline failed getting data line");
         exit(1);
      }
      //printf("Read %lu chars\n", chars_read);
      if (chars_read == 0)
      {
         continue;
      }

      (*line)[chars_read-1] = '\0';
      
      append_chars(&(fn->fasta.dna), *line);
   }

   if (fn->next != NULL)
   {
      printf("node next isn't NULL\n");
   }
   return fn;
}

struct fasta_list *parse_fasta_file(char *file)
{
   FILE *f = fopen(file, "r");
   if (f == NULL)
   {
      printf("Failed to open file \"%s\"\n", file);
      exit(1);
   }

   struct fasta_list *fl = new_fasta_list();
   
   struct fasta_node *current = NULL;
   struct fasta_node *next;
   char *line_buf = NULL;
   size_t line_buf_alloc = 0;
   while ((next = read_fasta_node(f, &line_buf_alloc, &line_buf)) != NULL)
   {
      //printf("Read fasta node\n");
      if (current == NULL)
      {
         //printf("first fasta node\n");
         fl->head = next;
         fl->current_node = next;
         current = next;
      }
      else
      {
         //printf("adding to existing fasta node\n");
         current->next = next;
         current = next;
      }
   }

   fclose(f);
   free(line_buf);

   return fl;
}

int fl_len(struct fasta_list *fl)
{
   int len=0;
   struct fasta_node *n = fl->head;
   while (n != NULL)
   {
      ++len;
      n = n->next;
   }
   return len;
}
