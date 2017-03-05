#include <buffer.h>

#include <stdio.h>
#include <stdlib.h>

int file_to_buffer(char *file_name, struct text_buffer *buffer, struct buffer_node *node)
{
	FILE *fp = fopen(file_name, "r");

	if (fp == 0)
		return 0;
	
	buffer->head = node;
	buffer->tail = node;

	int c;

	while(1)
	{
		for (int i = 0; i < BUFFER_NODE_LENGTH-1; i++)
		{
			c = fgetc(fp);
			if (!feof(fp))
			{
				node->text[i] = (char)c;
			}
		
			else
				goto exit;//TODO exit, means end of file
		}
		//printf("%s\n", node->text);
		node->next = (buffer_node*)malloc(sizeof(buffer_node));
		node->next->prev = node;
		node = node->next;

		buffer->tail = node;
	}

exit:
	fclose(fp);
	return 1;
}

int append_char_to_buffer(char c, struct text_buffer *buffer)
{
	if (buffer->tail == 0)
		return 0;

	struct buffer_node *node = buffer->tail;

	if (node->text[BUFFER_NODE_LENGTH-2] != 0)
	{
		node->next = (buffer_node*)malloc(sizeof(struct buffer_node));
		node->next->prev = node;
		node = node->next;
		buffer->tail = node;
	}
	
	for (int i = 0; i < BUFFER_NODE_LENGTH-1; i++)
	{
		if (node->text[i] == 0)
		{
			node->text[i] = c;
			break;
		}
	}
	
	return 1;
}

// TODO: Get rid of the retarded segmentation fault
void free_buffer(struct text_buffer *buffer)
{
        struct buffer_node *tmp;
	struct buffer_node *head = buffer->head;

	while (head != 0)
	{
		tmp = head;
		
		head = head->next;
		free(tmp);
	}

	buffer->head = 0;
	buffer->tail = 0;
}
