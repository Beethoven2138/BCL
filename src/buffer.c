#include <buffer.h>

int file_to_buffer(char *file_name, struct text_buffer *buffer)
{
	FILE *fp = fopen(file_name, "r");

	if (fp == NULL || buffer == NULL)
		return 0;

	struct buffer_node *node;

	//Create the first line in the buffer
	buffer->head = (buffer_node*)malloc(sizeof(buffer_node));
	buffer->tail = buffer->head;
	node = buffer->head;

	buffer->node_count = 1;

	node->length = 0;
	node->lineno = 1;

	int c;

	while (1)
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
			node->next->lineno = node->lineno+1;
			node = node->next;
			node->next = NULL;
			node->head = NULL;
			node->length = 0;


			buffer->tail = node;
			buffer->node_count++;
		}

		else
			goto exit;
	}

exit:
	fclose(fp);
	node->next = NULL;
	return 1;
}

void add_char_to_line(struct buffer_node *line, char value, int position)
{
	if (line->head == NULL)
	{
		line->head = (character*)malloc(sizeof(character));
		line->head->next = NULL;
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
		if (letter->next == NULL)
		{
			letter->next = (character*)malloc(sizeof(character));
			letter->next->value = value;
			letter->next->next = NULL;
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

	if (node != NULL)
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

	destroy_win(buffer->text_win->win);
	destroy_win(buffer->command_win->win);
	//free(buffer);
}


struct character* get_letter(struct buffer_node *node, size_t position)
{
        size_t count = 0;

	struct character *letter = node->head;
	
	for (letter; letter != NULL; letter = letter->next)
	{
		if (count == position)
		{
			return letter;
		}

		count++;
	}

	return 0;
}

void get_line(struct buffer_node *node, char *contents, size_t length)
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

        size_t count = 1;

	for (node; node != NULL; node = node->next)
	{
		if (count == position)
		{
		        struct character *tmp = node->next;
			node->next = tmp->next;
			free(tmp);
			line->length--;
			return;
		}

		count++;
	}
}

void print_buffer(struct text_buffer *buffer)
{
	int x = 0;
	int y = 0;

	struct buffer_node *line = buffer->edit_start;

	struct character *value;

	move(0,0);
	printw("Text editor Version 1.1.   AUTHOR: SAXON SUPPLE");
	refresh();
	move(0,0);

	for (line; line != buffer->edit_end->next; line = line->next)
	{
		mvwprintw(buffer->text_win->win, y+1, x+1, "0%d ", line->lineno);

		value = line->head;

		for (value; value != NULL; value = value->next)
		{
			mvwprintw(buffer->text_win->win, y+1, x+4, "%c", value->value);
			x++;
		}

		y++;
		x = 0;
	}

	move(buffer->y+2, buffer->x+4);

	wrefresh(buffer->text_win->win);
}

void get_line_node(struct text_buffer *buffer, struct buffer_node **line, int num)
{
	*line = buffer->head;

	for (num; num > 0 && (*line)->next != NULL; num--)
	{
		*line = (*line)->next;
	}
}

void get_prev_line(struct text_buffer buffer, struct buffer_node **line)
{
	struct buffer_node *tmp = buffer.head;

	for (tmp; tmp->next != *line; tmp = tmp->next);

	*line = tmp;
}

void delete_line(struct text_buffer *buffer, struct buffer_node *prev)
{
	if (prev->next->next == NULL)
	{
		free(prev->next);
		buffer->tail = prev;
		return;
	}

	struct buffer_node *tmp = prev->next->next;

	free(prev->next);

	if (prev->next == buffer->tail)
		buffer->tail = prev;
	
        prev->next = tmp;

	buffer->node_count--;
}

void init_buffer(struct text_buffer *buffer)
{
	buffer->x = 0;
	buffer->y = 0;
	buffer->text_win = (struct window_data*)malloc(sizeof(struct window_data));
	buffer->command_win = (struct window_data*)malloc(sizeof(struct window_data));
	buffer->text_win->x = 0;
	buffer->text_win->y = 1;
	buffer->text_win->w = COLS;
	buffer->text_win->h = LINES - 1;

	buffer->command_win->x = 0;
	buffer->command_win->y = LINES - 1;
	buffer->command_win->w = COLS;
	buffer->command_win->h = 1;

	buffer->text_win->win = create_new_win(buffer->text_win->x, buffer->text_win->y, buffer->text_win->w,
					       buffer->text_win->h);

	buffer->command_win->win = create_new_win(buffer->command_win->x, buffer->command_win->y,
						  buffer->command_win->w, buffer->command_win->h);
}

void set_edit_buffer(struct text_buffer *buffer, int line)
{
	buffer->edit_start = buffer->head;

	for (line; line > 0; line--)
	{
		buffer->edit_start = buffer->edit_start->next;
	}

	buffer->edit_end = buffer->edit_start;

        for (int i = 1; i < LINES; i++)
	{
		if (buffer->edit_end->next == NULL)
			break;
		else
			buffer->edit_end = buffer->edit_end->next;
	}
}
