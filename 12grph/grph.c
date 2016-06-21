#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// in the problem, this is parameter k
#define MATCH_LEN 3

struct char_array
{
   size_t len; //include terminating null
   int alloc;
   char *data;
};

void init_char_array(struct char_array *ca)
{
   ca->len = 1;
   ca->alloc = 10;
   ca->data = malloc(10 * sizeof(char));
   ca->data[0] = '\0';
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

struct fasta
{
   char *name;
   struct char_array dna;
};

struct fasta_node;
struct fasta_node
{
   struct fasta_node *next;
   struct fasta fasta;
};

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

struct fasta_list
{
   struct fasta_node *head;
   struct fasta_node *current_node;
};

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

struct directed_edge;
struct directed_edge
{
   char *start;
   char *end;
   struct directed_edge *next;
};

struct directed_edge *new_directed_edge(char *start, char *end)
{
   struct directed_edge *edge = malloc(sizeof(struct directed_edge));
   edge->start = copystr(start);
   edge->end = copystr(end);
   edge->next = NULL;
}

void free_directed_edge(struct directed_edge *edge)
{
   free(edge->start);
   free(edge->end);
   free(edge);
}

struct directed_graph
{
   struct directed_edge *head;
   struct directed_edge *current;
};

void free_graph(struct directed_graph *graph)
{
   struct directed_edge *tmp;
   struct directed_edge *edge = graph->head;
   while (edge != NULL)
   {
      tmp = edge->next;
      free_directed_edge(edge);
      edge = tmp;
   }
}

void log_graph(struct directed_graph *g)
{
   struct directed_edge *e = g->head;
   while (e != NULL)
   {
      printf("%s %s\n", e->start, e->end);
      e = e->next;
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

   struct fasta_list fastas;
   fastas.head = NULL;
   fastas.current_node = NULL;

   ssize_t chars_read;
   char *current_line = NULL;
   size_t current_line_alloc = 0;
   while ((chars_read = getline(&current_line, &current_line_alloc, f)) > -1)
   {
      //printf("READ FROM FILE: %s\n", current_line);
      if (*current_line == '>')
      {
         // fasta name - skip '>' character
         char *name = malloc((chars_read-1)*sizeof(char));
         if (sscanf(current_line, ">%s\n", name) != 1)
         {
            puts("Failed to read line name");
            return 1;
         }
         //printf("%s\n", name);
         struct fasta_node *fn = new_fasta_node(name); 
         if (fastas.head == NULL)
         {
            // first entry
            fastas.current_node = fastas.head = fn;
         }
         else if (fastas.current_node->fasta.dna.len == 0)
         {
            puts("Found title line but current fasta is empty");
            return 1;
         }
         else
         {
            //printf("%s: %s", fastas.current_node->fasta.name, fastas.current_node->fasta.dna.data);
            fastas.current_node = fastas.current_node->next = fn;
         }
      }
      else
      {
         //printf("Adding chars to %s: %s", fastas.current_node->fasta.name, current_line);
         if (current_line[chars_read] == '\0'
           && current_line[chars_read-1] == '\n')
         {
            current_line[chars_read-1] = '\0';
         }
         else
         {
            puts("Failed end of dna string newline replacement");
            return 1;
         }
         append_chars(&(fastas.current_node->fasta.dna), current_line);
      }
   }
   free(current_line);

   struct directed_graph graph;
   graph.head = NULL;
   graph.current = NULL;

   char *c1, *c2;
   struct fasta_node *edge_start = fastas.head;
   struct fasta_node *edge_end;
   while (edge_start != NULL)
   {
      edge_end = fastas.head;
      while (edge_end != NULL)
      {
         if (edge_start != edge_end)
         {
            //printf("Start dna: %s, len=%lu\n", edge_start->fasta.dna.data, edge_start->fasta.dna.len);
            //printf("End dna: %s, len=%lu\n", edge_end->fasta.dna.data, edge_end->fasta.dna.len);
            //printf("End of Start dna: %s\n", edge_start->fasta.dna.data + edge_start->fasta.dna.len -1 - MATCH_LEN);
            
            c1 = edge_start->fasta.dna.data + edge_start->fasta.dna.len - 1 - MATCH_LEN;
            c2 = edge_end->fasta.dna.data;
            int i;
            int match = 1;
            for(i=0; i<MATCH_LEN; i++)
            {
               //printf("Compare %c with %c\n", *c1, *c2);
               if (*c1 != *c2)
               {
                  match = 0;
                  break;
               }
               c1++;
               c2++;
            }

            if (match)
            {
               struct directed_edge *e = new_directed_edge(edge_start->fasta.name, edge_end->fasta.name);
               //printf("Adding %s -> %s\n", e->start, e->end);
               if (graph.head == NULL)
               {
                  graph.head = graph.current = e;
               }
               else
               {
                  graph.current = graph.current->next = e;
               }
            }
         }
         edge_end = edge_end->next;
      }
      edge_start = edge_start->next;
   }

   free_fasta_nodes(&fastas);

   log_graph(&graph);

   free_graph(&graph);

   fclose(f);
}
