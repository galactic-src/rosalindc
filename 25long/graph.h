#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>

#include "str.h"

struct directed_edge;
struct directed_edge
{
   void *start;
   void *end;
   int overlap;
   struct directed_edge *next;
};

struct directed_edge *new_directed_edge(void *start, void *end);

struct directed_graph
{
   struct directed_edge *head;
   struct directed_edge *current;
};

void free_graph(struct directed_graph *graph);
void log_graph(struct directed_graph *g, void (log_func)(void *start, void *end));

#endif
