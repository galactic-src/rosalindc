#include <stdio.h>
#include <stdlib.h>

// each time you add a leaf, you effectively replace 1 leaf with 2 leaves by adding an internal node.
// trivial, just subtract 2 from the supplied number.

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      puts("Expected usage: ./inod 200");
      exit(1);
   }

   int leaves = atoi(argv[1]);
   int internal_nodes = leaves - 2;
   printf("%d\n", internal_nodes);

   return 0;
}
