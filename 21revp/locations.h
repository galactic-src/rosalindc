#ifndef LOCATIONS_H
#define LOCATIONS_H

struct locations_list
{
   int len;
   int alloc;
   int *locations;
};

struct locations_list *new_locations_list();
void clear_locations_list(struct locations_list *ll);
void free_locations_list(struct locations_list *ll);
void locations_list_add(struct locations_list *ll, int location);
void log_locations(struct locations_list *ll);



#endif
