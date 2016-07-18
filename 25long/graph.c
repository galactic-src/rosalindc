#include "graph.h"

struct directed_edge *new_directed_edge(void *start, void *end)
{
   struct directed_edge *edge = malloc(sizeof(struct directed_edge));
   edge->start = start;
   edge->end = end;
   edge->overlap = 0;
   edge->next = NULL;
}

void free_graph(struct directed_graph *graph)
{
   struct directed_edge *tmp;
   struct directed_edge *edge = graph->head;
   while (edge != NULL)
   {
      tmp = edge->next;
      free(edge);
      edge = tmp;
   }
}

void log_graph(struct directed_graph *g, void (log_func)(void *start, void *end))
{
   struct directed_edge *e = g->head;
   while (e != NULL)
   {
      log_func(e->start, e->end);
      e = e->next;
   }
}
