#include <buffer.h>
#include <ansi.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

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
			node->next = 0;
			node->head = 0;

			buffer->tail = node;
			buffer->node_count++;
		}
		
		else
			goto exit;
	}

exit:
	fclose(fp);
	node->next = 0;
	return 1;
}

void add_char_to_line(struct buffer_node *line, char value, int position)
{
	if (line->head == 0)
	{
		line->head = (character*)malloc(sizeof(character));
		line->head->next = 0;
		line->head->value = 0;
	}
	
	struct character *letter = line->head;
	
	struct character *tmp = (character*)malloc(sizeof(character));
	
	//Start of line
	if (position == 0)
	{
	        tmp->next = letter;
		line->head = tmp;
		tmp->value = value;
		goto exit;
	}

	for (unsigned int count = 1; count <= position; count++)
	{
		//Append at end of the line
		if (letter->next == 0)
		{
			letter->next = (character*)malloc(sizeof(character));
			letter->next->value = value;
			letter->next->next = 0;
			goto exit;
		}

		if (count == position)
		{
			tmp->next = letter->next;
			letter->next = tmp;
			tmp->value = value;
			goto exit;
		}

		letter = letter->next;
	}

exit:
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

	//free(buffer);
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

	for (int i = 0; i < length; i++)
	{
		contents[i] = letter->value;

		letter = letter->next;
	}

	contents[length] = 0;
}

void delete_character(struct buffer_node *line, unsigned int position)
{
	struct character *node = line->head;

	if (position == 0)
	{
		line->head = node->next;
		free(node);
		return;
	}
	
	unsigned int count = 1;
	
	for (node; node != 0; node = node->next)
	{
		if (count == position)
		{
			struct character *tmp = node->next;
			node->next = tmp->next;
			free(tmp);
			goto exit;
		}

		count++;
	}
exit:
	line->length--;
}

void print_buffer(struct text_buffer *buffer)
{
	system ("/bin/stty cooked");

	clear();

	move_cursor(0,0);

	struct buffer_node *line = buffer->head;

	unsigned int count = 1;
	
	while (count < w.ws_row)
	{
		textcolor(BRIGHT, BLACK, WHITE);

		if (count < 10)
		{
			printf("0%d", count);
		}

		else
		{
			printf ("%d", count);
		}

		textcolor(RESET, WHITE, BLACK);

		unsigned int length = 0;

		//This code is for adding the cursor at the end of the line. The problem is that at the beginning line == 0 meaning that the cursor will show at the end of the line.
		if (count == cursor.line+1 && line != 0)
		{
			bool c_printed = 0;

			for (struct character *node = line->head; node != 0; node = node->next)
			{
				if (++length > w.ws_col)
				{
					printf("\n  ");
					length = 0;
				}

				if (node->value == 0)
					break;

				if (length != cursor.column)
					printf("%c", node->value);
				else
				{
				        textcolor(RESET, BLACK, WHITE);
					printf("%c", node->value);
					textcolor(RESET, WHITE, BLACK);
					c_printed = true;
				}
			}

			if (!c_printed)
			{
				textcolor(RESET, BLACK, WHITE);
				printf(" ");
				textcolor(RESET, WHITE, BLACK);
			}
			line = line->next;
		}

		else if (line != 0)
		{
			for (struct character *node = line->head; node != 0; node = node->next)
			{
				if (++length > w.ws_col)
				{
					printf("\n  ");
					length = 0;
				}

				if (node->value == 0)
					break;

				printf("%c", node->value);
			}
			line = line->next;

		}
		
		printf("\n");

		count++;
	}

	move_cursor(cursor.line, cursor.column);

	system ("/bin/stty raw");
}

void get_line_node(struct text_buffer *buffer, struct buffer_node **line, int num)
{
	*line = buffer->head;

	for (num; num > 0 && (*line)->next != 0; num--)
	{
		*line = (*line)->next;
	}
}

int get_line_length(struct buffer_node *line)
{
	int length = 1;

	struct buffer_node *node = line;
	
	for (node; node != 0; node = node->next)
		length++;

	return length;
}

void get_prev_line(struct text_buffer buffer, struct buffer_node **line)
{
	struct buffer_node *tmp = buffer.head;

	for (tmp; tmp->next != *line; tmp = tmp->next);

	*line = tmp;
}
