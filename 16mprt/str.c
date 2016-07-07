#include <stdlib.h>
#include <string.h>

#include "str.h"

// length of string, including terminating null
size_t str_total_len(const char *s)
{
   return strlen(s) + 1;
}

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
   memcpy(ca->data + ca->len - 1, src, srclen); //includes terminating NUL
   ca->len = ca->len - 1 + srclen;
}

char *detach(struct char_array *ca)
{
   char *str = ca->data;
   free(ca);
   return str;
}
