#include <fileops.h> 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* create_file_buffer(char *file_name)
{
	FILE *fp = fopen(file_name, "r");
	if(fp != NULL)
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

	return 0;
}

int copy_file(char *src_file_name, char *dest_file_name)
{
	FILE *src = fopen(src_file_name, "r");
	if(src == NULL)
		return 0;

	FILE *dest = fopen(dest_file_name, "w+");
	if(dest == NULL)
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

int buffer_to_file(char *file_name, struct text_buffer *buffer)
{
	
	FILE *fp = fopen(file_name, "w+");

	if (fp == NULL)
		return 0;

	struct buffer_node *node = buffer->head;
	
	for (node; node != NULL; node = node->next)
	{
		char *line = (char*)malloc(sizeof(char) * (node->length+1));
		get_line(node, line, node->length);
		fprintf(fp, "%s", line);
		fputc(10, fp);
		free(line);
	}
	fclose(fp);

	return 1;
}

void* auto_save(void *file_buffer_name)
{
	while (1)
	{
		buffer_to_file(file_buffer_name, &buffer);
		sleep (5);
	}
	
	return 0;
}
