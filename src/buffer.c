#include <buffer.h>

#include <stdio.h>
#include <stdlib.h>

int file_to_buffer(char *file_name, struct text_buffer *buffer)
{
	FILE *fp = fopen(file_name, "r");

	if (fp == 0 || buffer == 0)
		return 0;

	struct buffer_node *node;

	//Create the first line in the buffer
	buffer->head = (buffer_node*)malloc(sizeof(buffer_node));
	buffer->tail = buffer->head;
	node = buffer->head;

	buffer->node_count = 1;

	node->length = 0;

	int c;

	while(1)
	{
		c = fgetc(fp);

		if (!feof(fp) && c != 10)
		{
			//Create a function to add a character to the character linked list
			add_char_to_line(node, (char)c, node->length);
		}

		//New Line character
		else if (!feof(fp))
		{
			//Create a new line
			node->next = (buffer_node*)malloc(sizeof(buffer_node));
			node = node->next;

			buffer->tail = node;
			buffer->node_count++;
		}
		
		else
			goto exit;
	}

exit:
	fclose(fp);
	return 1;
}

void add_char_to_line(struct buffer_node *line, char value, int position)
{
	printf ("%c", value);
	
	unsigned int count = 0;

        struct character *node = line->head;

	if (node == 0)
	{
		//Make first character node in the line
		line->head = (character*)malloc(sizeof(character));
		node = line->head;
	}

	for (node; node != 0; node = node->next)
	{
		//If the character is to be placed at the end of the line
		if (node->next == 0 && position == count + 1)
		{
			node->next = (character*)malloc(sizeof(character));
		}

		else if (count == position)
		{
			struct character *tmp = (character*)malloc(sizeof(character));

			//This will occur on the itteration after the previous if statement
			if (node->next == 0)
				node->next = (character*)malloc(sizeof(character));
			
		        tmp->next = node->next;
			node->next = tmp;
			node->next->value = value;

			break;
		}
		
		count++;
	}
	
	line->length++;
}

void free_buffer(struct text_buffer *buffer)
{
	struct buffer_node *node = buffer->head;

	if (node != 0)
	{
		struct buffer_node *tmp_node;

		//Free each line
		for (int j = 0; j < buffer->node_count; j++)
		{
			struct character *letter = node->head;

			//Free each letter in a line
			for (int i = 0; i < node->length; i++)
			{
				struct character *tmp;
				tmp = letter;
				letter = letter->next;
				free(tmp);
			}

			tmp_node = node;
			node = node->next;
			free(tmp_node);
		}
		
	}

//	free(buffer);
}


char get_letter(struct buffer_node *node, unsigned int position)
{
	unsigned int count = 0;

	struct character *letter = node->head;
	
	for (letter; letter != 0; letter = letter->next)
	{
		if (count == position)
		{
			return letter->value;
		}
		
		count++;
	}

	return 0;
}

void get_line(struct buffer_node *node, char *contents, unsigned int length)
{
	struct character *letter;

	letter = node->head;
	
	unsigned int count = 0;
	
	for (letter; letter != 0; letter = letter->next)
	{
		contents[count] = letter->value;
		count++;
	}
}
