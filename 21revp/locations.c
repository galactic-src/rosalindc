struct locations_list *new_locations_list()
{
   struct locations_list *ll = malloc(sizeof(struct locations_list));
   ll->len = 0;
   ll->alloc = 10;
   ll->locations = malloc(sizeof(int) * 10);
   return ll;
}

void clear_locations_list(struct locations_list *ll)
{
   ll->len = 0;
   //clear any data but leave allocated memory
   bzero(ll->locations, ll->alloc);
}

void free_locations_list(struct locations_list *ll)
{
   free(ll->locations);
   free(ll);
}

void locations_list_add(struct locations_list *ll, int location)
{
   if (ll->len == ll->alloc)
   {
      ll->alloc *= 2;
      ll->locations = realloc(ll->locations, ll->alloc * sizeof(int));
   }
   ll->locations[ll->len++] = location;
}

void log_locations(struct locations_list *ll)
{
   int i;
   for (i=0; i<ll->len; i++)
   {
      printf("%d", ll->locations[i]);
      if (i == ll->len)
      {
         break;
      }
      putchar(' ');
   }
   putchar('\n');
}

