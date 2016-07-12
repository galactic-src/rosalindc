#include "str.h"

// length of string, including terminating null
size_t str_total_len(const char *s)
{
   return strlen(s) + 1;
}

void init_char_array(struct char_array *ca)
{
   init_char_array_sz(ca, 10);
}

void init_char_array_sz(struct char_array *ca, int size)
{
   ca->len = 1;
   ca->alloc = size;
   ca->data = malloc(size);
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
   memcpy(ca->data + ca->len - 1, src, srclen);
   ca->len = ca->len - 1 + srclen;
}

void add_char(struct char_array *ca, char c)
{
   //printf("adding %c, last two chars are %c and 0x%x\n", c, ca->data[ca->len-2], ca->data[ca->len-1]);
   ensure_len(ca, ca->len+1);
   ca->data[ca->len-1] = c;
   //printf("%lu\n", ca->len);
   if (c != '\0')
   {
      ca->len++;
      //printf("%lu\n", ca->len);
      ca->data[ca->len-1] = '\0';
   }
}

char *detach(struct char_array *ca)
{
   char *str = ca->data;
   free(ca);
   return str;
}

void reverse_char_array(struct char_array *ca)
{
   char *new_data = malloc(ca->len);
   new_data[ca->len-1] = '\0';
   int dest_ix=ca->len-2, src_ix=0;
   while(dest_ix >= 0)
   {
      new_data[dest_ix--] = ca->data[src_ix++];
   }
   free(ca->data);
   ca->data = new_data;
}

char *clone_str(char *src)
{
   size_t len = str_total_len(src);
   char *copy = malloc(len * sizeof(char));
   memcpy(copy, src, len);
   return copy;
}

struct char_array *clone_ca(struct char_array *src)
{
   //printf("Cloning data: %s\n", src->data);
   struct char_array *clone = malloc(sizeof(struct char_array));
   init_char_array(clone);
   append_chars(clone, src->data);
   return clone;
}

void remove_all(struct char_array *ca, char *to_remove)
{
   if (strstr(ca->data, to_remove) == NULL)
   {
      return;
   }

   size_t remove_len = strlen(to_remove);

   char *remove_ix;
   while ((remove_ix = strstr(ca->data, to_remove)) != NULL)
   {
      //printf("removing...\n");
      char *remove_end = remove_ix+remove_len;
      memmove(remove_ix, remove_end, strlen(remove_end)+1);
      ca->len -= remove_len;
   }
}

struct char_array_list *new_char_array_list()
{
   struct char_array_list *list = malloc(sizeof(struct char_array_list));
   list->alloc = 10;
   list->len = 0;
   list->strs = malloc(10*sizeof(void*));
   return list;
}

void cal_ensure_size(struct char_array_list *list, int size)
{
   while (list->alloc < size)
   {
      list->alloc *= 2;
      list->strs = realloc(list->strs, list->alloc);
   }
}

void cal_add(struct char_array_list *list, struct char_array *to_add)
{
   cal_ensure_size(list, list->len+1);
   list->strs[list->len] = to_add;
   list->len++;
}

void log_cal(struct char_array_list *list)
{
   int i;
   for(i=0; i<list->len; i++)
   {
      struct char_array *ca = list->strs[i];
      printf("%s\n", ca->data);
   }
}

void free_cal(struct char_array_list *list)
{
   int i;
   for (i=0; i<list->len; i++)
   {
      free_char_array(list->strs[i]);
   }
   free(list->strs);
   free(list);
}

/*int main(void)
{
   struct char_array *ca = malloc(sizeof(struct char_array));
   init_char_array(ca);
   printf("empty: %s\n", ca->data);   
   append_chars(ca, "some data");
   printf("with some data: %s\n", ca->data);
   printf("last two chars are %c, 0x%x\n", ca->data[ca->len-2], ca->data[ca->len-1]);
   add_char(ca, '!');
   printf("with exclamation: %s\n", ca->data);
   
   reverse_char_array(ca);
   printf("reversed: %s\n", ca->data);

   append_chars(ca, "ABCBCDEDEFBCDBC");
   printf("str: %s, len: %lu\n", ca->data, ca->len);
   remove_all(ca, "F");
   printf("removed F: %s, len: %lu\n", ca->data, ca->len);
   remove_all(ca, "BC");
   printf("removed BC: %s, len: %lu\n", ca->data, ca->len);
   free_char_array(ca);

   struct char_array_list *cal = new_char_array_list();
   cal_add(cal, attach_str("Hello."));
   cal_add(cal, attach_str("Goodbye."));
   printf("logging list, length %d\n", cal->len);
   log_cal(cal);
   free_cal(cal);   

   return 0;
}*/
