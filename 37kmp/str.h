#ifndef STR_H
#define STR_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct char_array
{
   size_t len; //include terminating null
   int alloc;
   char *data;
};

size_t str_total_len(const char *s);

void init_char_array(struct char_array *ca);
void init_char_array_sz(struct char_array *ca, int size);
struct char_array *attach_str(char *src);
void free_char_array(struct char_array *ca);
void ensure_len(struct char_array *ca, int newlen);
void append_chars(struct char_array *ca, char *src);
void add_char(struct char_array *ca, char c);

char *detach(struct char_array *ca);
void reverse_char_array(struct char_array *ca);
char *clone_str(char *src);
struct char_array *substr(struct char_array *src, int start, int len);
struct char_array *clone_ca(struct char_array *src);
void remove_all(struct char_array *ca, char *to_remove);


struct char_array_list
{
   int alloc;
   int len;
   struct char_array **strs;
};
struct char_array_list *new_char_array_list();
void cal_add(struct char_array_list *list, struct char_array *to_add);
void log_cal(struct char_array_list *list);
void free_cal(struct char_array_list *list);

int get_overlap(struct char_array *start, struct char_array *end, int min_overlap);

#endif
