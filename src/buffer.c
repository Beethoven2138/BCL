/*  BCL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    BCL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with BCL.  If not, see <http://www.gnu.org/licenses/>.*/

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
	node->xPos = 0;

	int c;

	while (1)
	{
		c = fgetc(fp);

		if (!feof(fp) && c != 10)
		{
		        //if (node->length < buffer->text_win->w - 5)
				add_char_to_line(node, (char)c, node->length);
				

				/*else
			{
				node->add_new_line = false;
				node->next = (buffer_node*)malloc(sizeof(buffer_node));
				node->next->lineno = node->lineno;
				node = node->next;
				node->next = NULL;
				node->head = NULL;
				node->length = 0;
				node->add_new_line = true;
				add_char_to_line(node, (char)c, 0);
				}*/
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
			node->xPos = 0;

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
		line->head->value = value;
		goto exit;
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



void free_buffer(struct text_buffer *buffer_to_free)
{
	struct buffer_node *node = buffer_to_free->head;

	if (node != NULL)
	{
		struct buffer_node *tmp_node;

		//Free each line
		for (int j = 0; j < buffer_to_free->node_count; j++)
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

	destroy_win(buffer_to_free->text_win->win);
	destroy_win(buffer_to_free->command_win->win);

	buffer_to_free->file_name = 0;
	buffer_to_free->file_buffer_name = 0;
	buffer_to_free->modified = false;
	buffer_to_free->x = 0;
	buffer_to_free->y = 0;
	buffer_to_free->xPos = 0;
	buffer_to_free->yPos = 0;
	buffer_to_free->head = NULL;
	buffer_to_free->tail = NULL;
	buffer_to_free->node_count = 0;
	buffer_to_free->current_line = NULL;
	buffer_to_free->edit_start = NULL;
	buffer_to_free->edit_end = NULL;
	buffer_to_free->text_win = NULL;
	buffer_to_free->command_win = NULL;
	//buffer_to_free->title_win = NULL;
	//destroy_win(buffer_to_free->title_win->win);
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

	for (int i = 0; i < length && letter != NULL; i++)
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
		line->length--;
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
        size_t x = 0;
	size_t xPos = 0;
	size_t y = 0;

	bool passed_line = false;

	struct buffer_node *line = buffer->edit_start;

	struct character *value;

	buffer->xPos = buffer->x;

	int i = 0;
	int j = 0;

	struct character *tmp = buffer->current_line->head;

	for (tmp; tmp != NULL; tmp = tmp->next, j++)
	{
		if (tmp->value == 9)
			i += 8;
		else
			i++;
		if (j == buffer->x)
			break;
	}
	
	int x_off = /*buffer->x*/i - (buffer->text_win->w - 6);

	if (x_off < 0)
		x_off = 0;

	for (line; line != buffer->edit_end->next && y < buffer->edit_end->lineno; line = line->next)
	{
		x = 0;
		xPos = 0;

		if (buffer->y == line->lineno - 1)
			passed_line = true;

		if (line->lineno < 10)		
			mvwprintw(buffer->text_win->win, y+1, x+1, "0%d ", line->lineno);

		else
			mvwprintw(buffer->text_win->win, y+1, x+1, "%d ", line->lineno);

		value = line->head;

		if (value != NULL)
		{
			for (size_t i = 0; i < x_off; i++)
			{
				value = value->next;

				if (value == NULL)
					break;
			}
		}
		
		for (value; value != NULL; value = value->next)
		{
			if (value->value == 9)
			{
				for (int i = 0; i < 7; i++)
					mvwprintw(buffer->text_win->win, y+1, x+4, " ");
				x += 8;

				if (line == buffer->current_line && xPos < buffer->xPos)
				{
					buffer->xPos += 7;
				}
			}

			else
			{
				mvwprintw(buffer->text_win->win, y+1, x+4, "%c", value->value);
				x++;
				xPos++;
			}
		}
		y++;
	}
	move(buffer->yPos + 2, buffer->xPos + 4 - x_off);

	box(buffer->text_win->win, 0, 0);
	box(buffer->command_win->win, 0, 0);
	mvwprintw(buffer->command_win->win, 1, 1, "F2=quit and save to backup, F3=quit, F4=GOTO, F5=Save, F6=Shell Command, F7=R/W file");
	//mvwprintw(buffer->text_win->win, 0, 0, "Buggy collection of linked lists Version 1.2   AUTHOR: SAXON SUPPLE");
	wrefresh(buffer->text_win->win);
	wrefresh(buffer->command_win->win);
	//wrefresh(buffer->title_win->win);
}

void print_line(struct text_buffer *buffer, struct buffer_node *line)
{
	for (int x = 1; x < buffer->text_win->w; x++)
		mvwprintw(buffer->text_win->win, buffer->yPos + 1, 4, " ");

	char tmp[line->length + 1];

	get_line(line, tmp, line->length);
	
	mvwprintw(buffer->text_win->win, buffer->yPos + 1, 4, tmp);
	wrefresh(buffer->text_win->win);
        move(buffer->yPos + 2, buffer->xPos + 4);
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
	setlocale(LC_ALL, "en_GB.UTF-8");
	start_color();

	init_pair(1,COLOR_BLACK, COLOR_WHITE);
	
	buffer->x = 0;
	buffer->y = 0;
	buffer->xPos = 0;
	buffer->yPos = 0;
	buffer->modified = false;
	buffer->text_win = (struct window_data*)malloc(sizeof(struct window_data));
	buffer->command_win = (struct window_data*)malloc(sizeof(struct window_data));
	//buffer->title_win = (struct window_data*)malloc(sizeof(struct window_data));
	buffer->text_win->x = 0;
	buffer->text_win->y = 1;
	buffer->text_win->w = COLS;
	buffer->text_win->h = LINES - 3;

	buffer->command_win->x = 0;
	buffer->command_win->y = LINES - 3;
	buffer->command_win->w = COLS;
	buffer->command_win->h = 3;

	/*buffer->title_win->x = 0;
	buffer->title_win->y = 0;
	buffer->title_win->h = 1;
	buffer->title_win->w = COLS;*/

	buffer->text_win->win = create_new_win(buffer->text_win->x, buffer->text_win->y, buffer->text_win->w,
					       buffer->text_win->h);

	buffer->command_win->win = create_new_win(buffer->command_win->x, buffer->command_win->y,
						  buffer->command_win->w, buffer->command_win->h);
	/*buffer->title_win->win = simple_create_new_win(buffer->text_win->x, buffer->text_win->y,
	  buffer->text_win->w, buffer->text_win->h);*/

//mvwprintw(buffer->text_win->win, 0, 0, "Buggy collection of linked lists Version 1.2   AUTHOR: SAXON SUPPLE");
	mvwprintw(buffer->command_win->win, 1, 1, "F2=quit and save to backup, F3=quit, F4=GOTO, F5=Save, F6=Shell Command, F7=R/W file");
	wrefresh(buffer->command_win->win);
	wrefresh(buffer->text_win->win);
	attron(COLOR_PAIR(1));
	for (int i = 0; i < COLS; i++)
		mvprintw(0, i, " ");
	mvprintw(0, 0, " Buggy Collection of Linked-lists Version 1.2   AUTHOR: SAXON SUPPLE");
	attroff(COLOR_PAIR(1));
	refresh();
}

void set_edit_buffer(struct text_buffer *buffer, int line)
{
	buffer->edit_start = buffer->head;

	for (line; line > 0; line--)
	{
		buffer->edit_start = buffer->edit_start->next;
	}

	buffer->edit_end = buffer->edit_start;

        for (int i = 1; i < buffer->text_win->h; i++)
	{
		if (buffer->edit_end->next == NULL)
			break;
		else
			buffer->edit_end = buffer->edit_end->next;
	}
}

void update_environment(struct text_buffer *buffer)
{
	buffer->text_win->w = COLS;
	buffer->text_win->h = LINES - 3;

	buffer->command_win->y = LINES - 3;
	buffer->command_win->w = COLS;

	buffer->command_win->x = 0;
	buffer->command_win->y = LINES - 3;
	buffer->command_win->w = COLS;
	buffer->command_win->h = 3;
}

static int longest_line_length(struct text_buffer buffer)
{
	int i = 0;

	struct buffer_node *tmp = buffer.head;
	struct character *value;

	for (tmp; tmp != NULL; tmp = tmp->next)
	{
		int x = 0;
		for (value = tmp->head; value != NULL; value = value->next)
		{
			if (value->value == 9)
				x += 8;
			else
				x++;
		}

		if (x > i)
			i = x;
	}

	return i;
}


/*TODO:
  Fix read_state
  Improve print_line
  Add multiple buffers
  Add undo and redo
  Add syntax highlighting
 */
