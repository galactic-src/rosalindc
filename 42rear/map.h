#ifndef MAP_H
#define MAP_H

#define BUCKETS 400

struct hm_node
{
   int *ints;
   struct hm_node *next;
};

struct hashmap
{
   struct hm_node nodes[400];
};


struct hashmap *new_map();
int hash_ints(int *ints);
void hm_add(struct hashmap *map, int *ints);
int hm_contains(struct hashmap *hm, int *ints);

void hm_free_node_list(struct hm_node *bucket);
void hm_free(struct hashmap *hm);

#endif /*MAP_H*/
