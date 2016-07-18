#ifndef FASTA_H
#define FASTA_H

#include "str.h"

struct fasta
{
   char *name;
   struct char_array dna;
};

struct fasta_node;
struct fasta_node
{
   struct fasta fasta;
   struct fasta_node *next;
};

struct fasta_node *new_fasta_node(char *name);
void free_fasta_node(struct fasta_node *fn);
void log_fasta_node(struct fasta_node *fn);

struct fasta_list
{
   struct fasta_node *head;
   struct fasta_node *current_node;
};

struct fasta_list *new_fasta_list();
void free_fasta_list(struct fasta_list *fl);
void log_fasta_list(struct fasta_list *fl);

struct fasta_node *read_fasta_node(FILE *f, size_t *alloc, char**line);
struct fasta_list *parse_fasta_file(char *file);

#endif
