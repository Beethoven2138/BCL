#include <buffer.h>

#include <stdio.h>
#include <stdlib.h>

int file_to_buffer(char *file_name, struct text_buffer *buffer, struct buffer_node *node)
{
	FILE *fp = fopen(file_name, "r");

	if (fp == 0)
		return 0;
	
	buffer->head = node;

	int c;

	while(1)
	{
		for (int i = 0; i < BUFFER_NODE_LENGTH; i++)
		{
			c = fgetc(fp);
			if (!feof(fp))
			{
				node->text[i] = (char)c;
				printf("%c", node->text[i]);
			}
		
			else
				goto exit;//TODO exit, means end of file
		}
		node->next = (buffer_node*)malloc(sizeof(buffer_node));
		node->next->prev = node;
		node = node->next;

		buffer->tail = node;
	}

exit:
	fclose(fp);
	return 1;
}
