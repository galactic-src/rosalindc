#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BP 1000
#define BUF_LEN (MAX_BP + 1)
#define NAME_BUF_LEN 150

int main(void)
{

	char name[NAME_BUF_LEN];
	double gc_content = 0.0;

	FILE *f = fopen("data", "r");
	if (f == NULL)
	{
		printf("Failed to open file\n");
		return 1;
	}

	int GC_count = 0;
	int AT_count = 0;
	char current_line_name[NAME_BUF_LEN];
   double line_gc_content;

	ssize_t chars_read = 0;
	size_t buf_len = 0;
	char *line_buf = NULL;
	while ((chars_read = getline(&line_buf, &buf_len, f)) > -1)
	{
		if (chars_read < 1)
		{
			printf("error reading line, got %lu\n", chars_read);
			if (line_buf != NULL)
			{
				free(line_buf);
			}
			fclose(f);
			return 1;
		}
      else
      {
		   //printf("chars_read = %lu\n", chars_read);
   		//printf("READ: %s", line_buf);
      }

      int i;
		switch(*line_buf)
		{
			case '>':
      		line_gc_content = 100.0 * (double)GC_count/((double)AT_count + (double)GC_count);
            //printf("GC:%d, AT:%d\n", GC_count, AT_count);
	      	if (line_gc_content > gc_content)
		      {
		   	   memcpy(name, current_line_name, NAME_BUF_LEN);
	   		   gc_content = line_gc_content;
   		   }

            sscanf(line_buf, ">%s", current_line_name);
            //puts(current_line_name);
      		GC_count=0;
      		AT_count=0;
			case 'A':
			case 'T':
			case 'C':
			case 'G':
            //line of bases - iterate over it later
	         
	      	//calculate GC content
	      	for (i=0; i<chars_read; i++)
	      	{
	      		//putchar('.');
	      		switch(line_buf[i])
	      		{
	      			case 'A':
	      			case 'T':
	      				AT_count++;
	      				//putchar('#');
	      				break;
	      			case 'C':
	      			case 'G':
	      				GC_count++;
	      				//putchar('~');
	      				break;
	      			default:
	      				//putchar(line_buf[i]);
	      				break;
	      		}
	      	}			
            break;
			case '\0':
				//blank line - ignore
				break;
			default:
				printf("Unexpected char at start of line: '%c'", *line_buf);
				if (line_buf != NULL)
				{
					free(line_buf);
				}
				fclose(f);
            return 1;
      }
   }

   line_gc_content = 100.0 * (double)GC_count/((double)AT_count + (double)GC_count);
   //printf("GC:%d, AT:%d\n", GC_count, AT_count);
 	if (line_gc_content > gc_content)
   {
  	   memcpy(name, current_line_name, NAME_BUF_LEN);
	   gc_content = line_gc_content;
   }
	
   free(line_buf);
	fclose(f);

	//puts("Result:");
	puts(name);
	printf("%f\n", gc_content);

   return 0;
}
