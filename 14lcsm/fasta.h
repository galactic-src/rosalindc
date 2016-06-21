struct char_array
{
   size_t len; //include terminating null
   int alloc;
   char *data;
};

void init_char_array(struct char_array *ca);
void free_char_array(struct char_array *ca);
size_t str_total_len(const char *s);
void ensure_len(struct char_array *ca, int newlen);
void append_chars(struct char_array *ca, char *src);
char *copystr(char *src);
struct char_array *attach_str(char *src);

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

struct fasta_node *new_fasta_node(char *name);
void free_fasta_node(struct fasta_node *fn);

struct fasta_list
{
   struct fasta_node *head;
   struct fasta_node *current_node;
};

void free_fasta_nodes(struct fasta_list *fl);

