#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum header_label
{
   HDR_SERVER,
   HDR_VARY,
   HDR_X_HOSTED_BY,
   HDR_CONTENT_TYPE,
   HDR_DATE,
   HDR_X_UNIPROT_RELEASE,
   HDR_X_TOTAL_RESULTS,
   HDR_EXPIRES,
   HDR_ACCESS_CONTROL_ALLOW_ORIGIN,
   HDR_CONNECTION,
   HDR_LAST_MODIFIED,
   HDR_ACCESS_CONTROL_ALLOW_HEADERS,
   HDR_CONTENT_LENGTH
};

enum http_ver
{
   HTTP1_0,
   HTTP1_1,
   HTTP2_0
};

struct header
{
   enum header_label label;
   char *value;
};

struct header_list
{
   int hdr_count;
   int hdr_alloc;
   struct header *headers;
};

struct get_response
{
   char *full_response;
   char *err;
   enum http_ver version;
   int code;
   char *desc;
   struct header_list headers;
   char *body;
};

struct get_response *parse_response(char *response)
{
   struct get_response *r = malloc(sizeof(struct get_response));
   bzero(r, sizeof(struct get_response));

   r->full_response = malloc(strlen(response)+1);
   memcpy(r->full_response, response, strlen(response)+1);
   r->err = NULL;

   int http_ver_1, http_ver_2;
   sscanf(response, "HTTP/%d.%d %d %ms\r\n", &http_ver_1, &http_ver_2, &r->code, &r->desc);
   if (http_ver_1 == 1 && http_ver_2 == 0)
   {
      //puts("HTTP1.0");
      r->version = HTTP1_0;
   }
   else if (http_ver_1 == 1 && http_ver_2 == 1)
   {
      //puts("HTTP1.1");
      r->version = HTTP1_1;
   }
   else if (http_ver_1 == 2 && http_ver_2 == 0)
   {
      //puts("HTTP2.0");
      r->version = HTTP2_0;
   }
   else
   {
      r->err = "Unrecognised HTTP version";
   }
   
   if (r->err) {return r;}

   return r;
}

char *example_response = "HTTP/1.1 200 OK\r\n"
"Server: Apache-Coyote/1.1\r\n"
"Vary: User-Agent\r\n"
"X-Hosted-By: European Bioinformatics Institute\r\n"
"Content-Type: text/plain\r\n"
"Date: Mon, 27 Jun 2016 17:22:00 GMT\r\n"
"X-UniProt-Release: 2016_06\r\n"
"X-Total-Results: 1\r\n"
"Expires: Mon, 04 Jul 2016 17:22:00 GMT\r\n"
"Access-Control-Allow-Origin: *\r\n"
"Connection: close\r\n"
"Last-Modified: Wed, 17 Feb 2016 00:00:00 GMT\r\n"
"Access-Control-Allow-Headers: origin, x-requested-with, content-type\r\n"
"Content-Length: 305\r\n"
"\r\n"
">sp|A2Z669|CSPLT_ORYSI CASP-like protein 5A2 OS=Oryza sativa subsp. indica GN=OsI_33147 PE=3 SV=1\n"
"MRASRPVVHPVEAPPPAALAVAAAAVAVEAGVGAGGGAAAHGGENAQPRGVRMKDPPGAP\n"
"GTPGGLGLRLVQAFFAAAALAVMASTDDFPSVSAFCYLVAAAILQCLWSLSLAVVDIYAL\n"
"LVKRSLRNPQAVCIFTIGDGITGTLTLGAACASAGITVLIGNDLNICANNHCASFETATA\n"
"MAFISWFALAPSCVLNFWSMASR";

int main(void)
{
   struct get_response *r = parse_response(example_response);
   if (r->err != NULL) 
   {
      puts(r->err);
      exit(1);
   }
   else
   {
      puts("no errors recorded");
   }

   printf("Response: %d %s\n", r->code, r->desc);

   return 0;
}
