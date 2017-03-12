#include <fileops.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* create_file_buffer(char *file_name)
{
	printf("In create_file_buffer\n");
	FILE *fp = fopen(file_name, "r");
	if(fp != 0)
	{
		char* file_buffer_name = (char*)malloc(strlen(file_name)*sizeof(char) + 2*sizeof(char));
	        strcpy(file_buffer_name, file_name);
		
	        strcat(file_buffer_name, "~");
		
		FILE *file_buffer = fopen(file_buffer_name, "w+");
		fclose(fp);
		fclose(file_buffer);
		free(file_buffer_name);
		return file_buffer_name;
	}

	printf("ERROR!\n");
	return 0;
}

int copy_file(char *src_file_name, char *dest_file_name)
{
	FILE *src = fopen(src_file_name, "r");
	if(src == 0)
		return 0;

	FILE *dest = fopen(dest_file_name, "w+");
	if(dest == 0)
		return 0;

	int c;

	while (1)
	{
		c = fgetc(src);

		if(feof(src))
			break;
		
		fputc(c, dest);
	}

	fprintf(dest, "%s", "^z");

	fclose(src);
	fclose(dest);

	return 1;
}

int buffer_to_file(char *file_name, char *permissions, struct text_buffer *buffer)
{
	
	FILE *fp = fopen(file_name, "w+");

	if (fp == 0)
		return 0;

	struct buffer_node *node = buffer->head;
	
	for (node; node->next != 0; node = node->next)
	{
		char *line = (char*)malloc(sizeof(char) * (node->length+1));
		get_line(node, line, node->length);
		printf("%s", line);
		fprintf(fp, line);
		fputc(10, fp);
		free(line);
	}
	fclose(fp);

	return 1;
}
