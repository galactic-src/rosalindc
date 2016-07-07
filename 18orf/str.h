size_t str_total_len(const char *s);

struct char_array
{
   size_t len; //include terminating null
   int alloc;
   char *data;
};

void init_char_array(struct char_array *ca);
void init_char_array_sz(struct char_array *ca, int size);
struct char_array *attach_str(char *src);
void free_char_array(struct char_array *ca);
void ensure_len(struct char_array *ca, int newlen);
void append_chars(struct char_array *ca, char *src);
char *detach(struct char_array *ca);
void reverse_char_array(struct char_array *ca);

struct char_array_list
{
   int alloc;
   int len;
   struct char_array *strs;
};
