#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fasta.h"

void init_char_array(struct char_array *ca)
{
   ca->len = 1;
   ca->alloc = 10;
   ca->data = malloc(10 * sizeof(char));
   ca->data[0] = '\0';
}

struct char_array *attach_str(char *src)
{
   struct char_array *ca = malloc(sizeof(struct char_array));
   ca->alloc = ca->len = str_total_len(src);
   ca->data = src;
   return ca;
}

void free_char_array(struct char_array *ca)
{
   free(ca->data);
   free(ca);
}

// length of string, including terminating null
size_t str_total_len(const char *s)
{
   return strlen(s) + 1;
}

void ensure_len(struct char_array *ca, int newlen)
{
   while (ca->alloc < newlen)
   {
      ca->alloc *= 2;
      ca->data = realloc(ca->data, ca->alloc);
   }
}

void append_chars(struct char_array *ca, char *src)
{
   int srclen = str_total_len(src);
   ensure_len(ca, ca->len + srclen);
   memcpy(ca->data + ca->len - 1, src, srclen);
   ca->len = ca->len - 1 + srclen;
}

char *copystr(char *src)
{
   size_t len = str_total_len(src);
   char *copy = malloc(len * sizeof(char));
   memcpy(copy, src, len);
   //printf("copied string %s, to %s\n", src, copy);
   return copy;
}

struct fasta_node *new_fasta_node(char *name)
{
   struct fasta_node *fn = malloc(sizeof(struct fasta_node));
   fn->fasta.name = name;
   init_char_array(&(fn->fasta.dna));
   fn->next = NULL;
}

void free_fasta_node(struct fasta_node *fn)
{
   free(fn->fasta.dna.data);
   free(fn->fasta.name);
   free(fn);
}

void free_fasta_nodes(struct fasta_list *fl)
{
   struct fasta_node *node = fl->head;
   struct fasta_node *tmp;
   while(node != NULL)
   {
      tmp = node->next;
      free_fasta_node(node);
      node = tmp;
   }
}
