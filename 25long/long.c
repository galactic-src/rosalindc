#include <string.h>

#include "fasta.h"
#include "graph.h"

//gcc long.c fasta.c graph.c str.c -o long

/**
 * General strategy:
 * 1. read fastas
 * 2. for every combination, check whether they overlap by at least half
 * 3. for each combination that do overlap by at least half, add to a list of combinations,
      sorted by which
 * 4. work down the list, combining strings.
      a. Combine strings. 
      b. Where the start of the string was the start of another pair, remove the pair from the list
      c. Where the start of the string was the end of another pair, update to point at the merged string
      d. Where the end of the string was the start of another pair, remove the pair
      e. Where the end of the string was the end of another pair, update to point at the merged string
 * 5. Repeat until list is empty and there is (hopefully) a single merged string. Otherwise something went wrong.. 
 */

struct directed_graph *graph_fasta_overlaps(struct fasta_list *fastas)
{
   struct directed_graph *graph = malloc(sizeof(struct directed_graph));
   graph->head = NULL;
   graph->current = NULL;

   struct fasta_node *fn_start = fastas->head;
   struct fasta_node *fn_end;
   while (fn_start != NULL)
   {
      //printf("start %s\n", fn_start->fasta.name);
      int half_start_len = (fn_start->fasta.dna.len+1)/2;

      fn_end = fastas->head;
      while (fn_end != NULL)
      {
         //printf("end %s\n", fn_end->fasta.name);
         if (fn_start == fn_end)
         {
            //puts("end == start; skipping end");
            fn_end = fn_end->next;
            continue;
         }
         
         int half_end_len = (fn_end->fasta.dna.len+1)/2;
         int min_overlap = half_start_len > half_end_len? half_start_len : half_end_len;

         int overlap = get_overlap(&(fn_start->fasta.dna), &(fn_end->fasta.dna), min_overlap);
         if (overlap > 0)
         {
            //printf("adding %s, %s, %d\n", fn_start->fasta.dna.data, fn_end->fasta.dna.data, overlap);
            struct directed_edge *new_edge = new_directed_edge(&(fn_start->fasta), &(fn_end->fasta));
            new_edge->overlap = overlap;
            if (graph->head == NULL)
            {
               graph->head = new_edge;
               graph->current = new_edge;
            }
            else
            {
               struct directed_edge *de = graph->head;
               while(de->next != NULL && de->next->overlap > overlap)
               {
                  de = de->next;
               }

               new_edge->next = de->next;
               de->next = new_edge;
            }
         }

         fn_end = fn_end->next;
      }

      fn_start = fn_start->next;
   }
   return graph;
}

struct char_array *merge_graphed_fragments(struct directed_graph *graph)
{
   struct directed_edge *to_merge;
   struct directed_edge *prev;
   struct directed_edge *de;
   graph->current = graph->head;
   while (graph->current !=NULL)
   {
      to_merge = graph->current;

      prev = to_merge;
      de = to_merge->next;
      while (de != NULL)
      {
         //b. Where the start of the string was the start of another pair, remove the pair from the list
         //e. Where the end of the string was the end of another pair, remove
         //f. if the two strings are the same two, reversed, remove
         if (to_merge->start == de->start
             || to_merge->end == de->end
             || (to_merge->start == de->end && to_merge->end == de->start))
         {
            prev->next = de->next;
            free(de);
            de = prev->next;
         }      
         //c. Where the start of the string was the end of another pair, update to point at the merged string.
         //   Nothing to do since we are keeping the start to hold the merged string.
         //else if (to_merge->start == de->end) {}
         //d. Where the end of the string was the start of another pair, update the pair
         else if (to_merge->end == de->start)
         {
            de->start = to_merge->start;
         }

         prev = de;
         de = de->next;
      }

      //add the tail of end on to start to merge the two
      append_chars(&((struct fasta *)to_merge->start)->dna, ((struct fasta *)to_merge->end)->dna.data + to_merge->overlap);
      //printf("after: %s\n", ((struct fasta *)to_merge->start)->dna.data);
      
      if (to_merge->next == NULL)
      {
         struct char_array *ret = &((struct fasta *)to_merge->start)->dna;
         // copy, so that we can free the graph but keep the result
         return attach_str(clone_str(ret->data));
      }
      else
      {
         graph->current = to_merge->next;
      }
   }
}

int main(void)
{
   struct fasta_list *fragments = parse_fasta_file("data");
   //log_fasta_list(fragments);

   struct directed_graph *overlaps = graph_fasta_overlaps(fragments);
   
   struct char_array *genome = merge_graphed_fragments(overlaps);

   free_fasta_list(fragments);
   struct directed_edge *de = overlaps->head;
   struct directed_edge *next_de;
   while (de != NULL)
   {
      printf("free after fragments\n");
      next_de = de->next;
      free(de);
      de = next_de;
   }
   free(overlaps);

   printf("%s\n", genome->data);

   free_char_array(genome);
   return 0;
}
