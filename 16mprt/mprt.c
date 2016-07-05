#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/time.h>

#include <netdb.h>
#include <sys/socket.h>
//#include <arpa/inet.h> 

#include "str.h"

struct locations_list
{
   int len;
   int alloc;
   int *locations;
};

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

/**
 * [XY] = X or Y
 * {Z} = not Z
 */
//char *gly_motif = "N{P}[ST]{P}";

// sub in the protein UniProt ID
char *get_template_close = "GET /uniprot/%s.fasta HTTP/1.1\r\nHost: www.uniprot.org\r\nConnection: close\r\n\r\n";
//char *get_template_keep_alive = "GET /uniprot/%s.fasta HTTP/1.1\r\nHost: www.uniprot.org\r\nConnection: keep-alive\r\n\r\n";

int is_gly(char *c)
{
   return ((c[0] == 'N') && (c[1] != 'P') && (c[2] == 'S' || c[2] == 'T') && (c[3] != 'P'));
}

void find_gly_motifs(struct locations_list *ll, char *sequence)
{
   clear_locations_list(ll);
   size_t string_length = strlen(sequence);
   for (int i=0; i<(string_length-3); i++)
   {
      if (is_gly(sequence + i))
      {
         locations_list_add(ll, i+1); // 1-indexed
      }
   }
}

/*void test_find_gly_motifs(struct locations_list *ll)
{
   char *test1 = "NNSN"; //expect 0
   char *test2 = "NNTXNNTX"; // expect 0, 4
   char *test3 = "NPSNNNTP"; // expect none
   char *test4 = "XXXXXXNSTSXXXXXNSTSXXNSTS"; // expect 3 matches
   char *test5 = "NNTNNTN"; // expect 0,3 (overlap)

   find_gly_motifs(ll, test1);
   printf("Tested %s:\n", test1);
   log_locations(ll);

   find_gly_motifs(ll, test2);
   printf("Tested %s:\n", test2);
   log_locations(ll);

   find_gly_motifs(ll, test3);
   printf("Tested %s:\n", test3);
   log_locations(ll);

   find_gly_motifs(ll, test4);
   printf("Tested %s:\n", test4);
   log_locations(ll);

   find_gly_motifs(ll, test5);
   printf("Tested %s:\n", test5);
   log_locations(ll);
}*/

struct char_array *send_prot_request(struct sockaddr_in *uniprot, char *request)
{   
   char buf[1000];

   /**
    * Make a socket
    */
   int tcpSocket = socket(AF_INET, SOCK_STREAM, 0);
   if (tcpSocket < 0)
   {
      printf("Error opening socket\n");
      exit(1);
   }
      
   /**
    * 2b. Connect to the server
    */
   if (connect(tcpSocket, (struct sockaddr *)uniprot, sizeof *uniprot) < 0)
   {
      printf("Error Connecting: %s\n", strerror(errno));
      printf("errno: %s", strerror(errno)); 
      exit(1);
   }

   /**
    * 3c. Send the request
    */
   //printf("Sending request: %s\n", request);
   if (send(tcpSocket, request, strlen(request), 0) < 0)
   {
      printf("Error with send: %s\n", strerror(errno));
      exit(1);
   }

   /**
    * 3d. Read the response
    */
   struct char_array *response = malloc(sizeof(struct char_array));
   init_char_array(response);
   ssize_t response_chars;
   while ((response_chars = recv(tcpSocket, buf, 999, 0)) != 0) // 0 signifies end of response
   {
      if (response_chars == -1)
      {
         printf("error calling recv: %s\n", strerror(errno));
         exit(1);
      }
      //printf("got %lu chars\n", response_chars);
      buf[response_chars] = '\0';
      append_chars(response, buf);
   }    

   close(tcpSocket);

   return response;
}

void parse_response(char *prt_name, char *response, struct sockaddr_in *uniprot, char *request_buf)
{
   struct locations_list *ll = new_locations_list();

   int free_response = 0;
   char *foundIx = strstr(response, "302 Found");
   if (foundIx != NULL)
   {
      char *new_prot_code;
      char *addr_header = strstr(response, "Location: ");
      if (sscanf(addr_header, "Location: http://www.uniprot.org/uniprot/%m[^.]", &new_prot_code) != 1) 
      {
         printf("failed to read redirect address");
         exit(1);
      }

      sprintf(request_buf, get_template_close, new_prot_code);
      
      //printf("Redirected request: %s\n", request_buf);
      struct char_array *response2 = send_prot_request(uniprot, request_buf);
      free_response = 1;
      //printf("Redirected response:\n%s\n", response2->data);
      
      response = detach(response2);

      free(new_prot_code);
   }

   struct char_array *amino_acids = malloc(sizeof(struct char_array));
   init_char_array(amino_acids);

   char line[500];
   char *fasta = strstr(response, "\r\n\r\n") + 4;

   fasta = strstr(fasta, "\n") + 1; // skip FASTA title line 
   while (strlen(fasta) > 0)
   {
      if (sscanf(fasta, "%s", line) != 1)
      {
         printf("failed to sscanf a fasta line, current fasta=\"%s\"", fasta);
         exit(1);
      }
      append_chars(amino_acids, line);
      fasta = strstr(fasta, "\n");
      if (fasta == NULL)
      {
         break;
      } else {
         fasta++;
      }
   }

   //printf("%s\n\n", amino_acids->data);
   
   find_gly_motifs(ll, amino_acids->data);
   if (ll->len > 0)
   {
      printf("%s\n", prt_name);
      log_locations(ll);
   }

   free_char_array(amino_acids);
   if (free_response)
   {
      free(response);
   }
   free_locations_list(ll);
}

int main(void)
{
   /**
    * 1. Find the IP address to send to
    */
   char *hostname = "www.uniprot.org";
   struct hostent *server = gethostbyname(hostname);
   if (server == NULL)
   {
      printf("gethostbyname(\"%s\") failed\n", hostname);
      exit(1);
   }
   /*else
   {
      printf("\n%s = ", server->h_name);
      unsigned int j = 0;
      while (server -> h_addr_list[j] != NULL)
      {
         printf("%s\n", inet_ntoa(*(struct in_addr*)(server -> h_addr_list[j])));
         j++;
      }
   }*/

   /**
    * 2. create structure for creating TCP connection
    */
   struct sockaddr_in uniprot;
   // clear memory - this ensures that the sin_zero field at the end of the struct is zero (there are rumours this may need zeroing..)
   bzero(&uniprot, sizeof uniprot);
   // set address type
   uniprot.sin_family = AF_INET;
   // set destination address (h_addr just uses the first entry from the h_addr_list)
   bcopy((char *)server->h_addr, (char *)&uniprot.sin_addr.s_addr, server->h_length);
   // set the port (80 for HTTP) and change the bytes to network order
   uniprot.sin_port = htons(80);

   /**
    * 3. Open data file and send a uniprot request for each line.
    */
   FILE *f = fopen("data", "r");
   if (f == NULL)
   {
      puts("failed to open data file");
      exit(1);
   }

   //request template is 89 chars (+1 for NUL, -2 for %s). 150 should be plenty for a request, which also includes the protein id.
   char request[150];

   char *file_line = NULL;
   size_t line_alloc = 0;
   ssize_t chars_read;
   struct char_array *response;
   while ((chars_read = getline(&file_line, &line_alloc, f)) > -1)
   {
      file_line[chars_read-1] = '\0'; // trim the newline character from the end
      sprintf(request, get_template_close, file_line);

      response = send_prot_request(&uniprot, request);

      parse_response(file_line, response->data, &uniprot, request);
      free_char_array(response);
   }

   free(file_line);

   fclose(f);   
}
