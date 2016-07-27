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

struct char_array *substr(struct char_array *src, int start, int len)
{
   if (start < 0 
     || len < 0 
     || start + len >= src->len)
   {
      return NULL;
   }
   else
   {
      struct char_array *new = malloc(sizeof(struct char_array));
      init_char_array_sz(new, len+1);
      memcpy(new->data, src->data + start, len);
      new->data[len] = '\0';
      new->len = len+1;
      return new;
   }
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
      printf("Freeing %d \"%s\"\n", i, list->strs[i]->data);
      free_char_array(list->strs[i]);
   }
   free(list->strs);
   free(list);
}

int get_overlap(struct char_array *start, struct char_array *end, int min_overlap)
{
   //printf("NEW: compare %s(%lu) with %s(%lu), min len %d\n", start->data, start->len, end->data, end->len, min_overlap);

   if (min_overlap < 1)
   {
      min_overlap = 1;
   }

   if (start->len-1 < min_overlap
       || end->len-1 < min_overlap)
   {
      //puts("too short for sufficient overlap");
      return 0;
   }

   // for the for loop, start from check_from and try matching from the next check_len characters
   char *check_from;
   int check_len;
   if (start->len <= end->len)
   {
      //end string at least as long as start string.
      //Start from the start of the start string
      check_from = start->data;
      //check until we would be checking min_overlap characters
      check_len = start->len-min_overlap;
      //printf("second string longer, start from beginning of start data, check_len %d\n", check_len);
   }
   else
   {
      //end string is shorter, we want to start where the end string exactly ends up over the end of the start string
      //start from where 
      check_from = start->data + start->len - end->len;
      check_len = end->len-min_overlap;
      //printf("first string longer, check_len %d\n", check_len);
   }

   int i;
   for (i=0; i<check_len; i++)
   {
      int chars_to_check = check_len-1+min_overlap-i;
      //printf("memcmp %s with %s for %d chars\n", check_from+i, end->data, chars_to_check);
      //printf("compare start %c-%c, end %c-%c\n", *(check_from+i), *(check_from+i+chars_to_check-1), *(end->data), *(end->data+chars_to_check-1));
      if (!memcmp(check_from+i, end->data, chars_to_check))
      {
         return min_overlap+check_len-1-i;
      }
   }

   return 0;
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
   struct char_array *cal_str1 = malloc(sizeof(struct char_array));
   init_char_array(cal_str1);
   append_chars(cal_str1, "Hello.");
   cal_add(cal, cal_str1);
   struct char_array *cal_str2 = malloc(sizeof(struct char_array));
   init_char_array(cal_str2);   
   append_chars(cal_str2, "Goodbye.");
   cal_add(cal, cal_str2);
   printf("logging list, length %d\n", cal->len);
   log_cal(cal);
   free_cal(cal);

   struct char_array *ovlp_str1 = malloc(sizeof(struct char_array));
   init_char_array(ovlp_str1);
   append_chars(ovlp_str1, "ABCABC");
   struct char_array *ovlp_str2 = malloc(sizeof(struct char_array));
   init_char_array(ovlp_str2);
   append_chars(ovlp_str2, "BCA");
   struct char_array *ovlp_str3 = malloc(sizeof(struct char_array));
   init_char_array(ovlp_str3);
   append_chars(ovlp_str3, "BABCA");

   printf("Overlap between %s and %s is %d\n", ovlp_str1->data, ovlp_str2->data, get_overlap(ovlp_str1, ovlp_str2, 0));
   printf("Overlap between %s and %s is %d\n", ovlp_str2->data, ovlp_str1->data, get_overlap(ovlp_str2, ovlp_str1, 0));
   printf("Overlap between %s and %s (min 5) is %d\n", ovlp_str2->data, ovlp_str1->data, get_overlap(ovlp_str2, ovlp_str1, 5));
   printf("Overlap between %s and %s (min 1) is %d\n", ovlp_str2->data, ovlp_str1->data, get_overlap(ovlp_str2, ovlp_str1, 1));
   printf("Overlap between %s and %s (min 0) is %d\n", ovlp_str1->data, ovlp_str3->data, get_overlap(ovlp_str1, ovlp_str3, 0));
   printf("Overlap between %s and %s (min 4) is %d\n", ovlp_str3->data, ovlp_str1->data, get_overlap(ovlp_str3, ovlp_str1, 4));
   printf("Overlap between %s and %s (min 0) is %d\n", ovlp_str2->data, ovlp_str3->data, get_overlap(ovlp_str2, ovlp_str3, 0));
   printf("Overlap between %s and %s (min 0) is %d\n", ovlp_str3->data, ovlp_str2->data, get_overlap(ovlp_str3, ovlp_str2, 0));

   struct char_array *ca_super = clone_ca(ovlp_str1);
   puts(ca_super->data);
   struct char_array *ca_sub = substr(ca_super, 0, 4);
   puts(ca_sub->data);
   free_char_array(ca_sub);
   free_char_array(ca_super);

   free_char_array(ovlp_str1);
   free_char_array(ovlp_str2);
   free_char_array(ovlp_str3);

   return 0;
}*/
