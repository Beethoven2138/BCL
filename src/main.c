/*  text_editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    text_editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with text_editor.  If not, see <http://www.gnu.org/licenses/>.*/

//AUTHOR: SAXON SUPPLE

//TODO: make everything relative to edit_start

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <ncurses.h>

#include <fileops.h>
#include <buffer.h>
#include <tui.h>

#define FILENAME "/home/saxon/Programming/text_editorV1.1/text_editor.txt"
#define FILEBUFFERNAME "/home/saxon/Programming/text_editorV1.1/text_editor.txt~"

//valgrind --leak-check=full ./text_editor

struct text_buffer buffer;

int main(int argc, char *argv[])
{
	char *file_buffer_name;
	char *file_name;

	//If the user entered he address of the file that he wants to open
	if (argc == 2)
	{
		file_buffer_name = create_file_buffer(argv[1]);
		if (file_buffer_name == 0)
			goto exit;
	        file_name = argv[1];
	}

	//Open the default one instead
	else
	{
		printf ("%d", argc);
		file_buffer_name = create_file_buffer(FILENAME);
		file_name = FILENAME;
	}

	if (file_buffer_name == 0)
		return 0;

	//Add the contents of the file to a buffer

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	refresh();

	init_buffer(&buffer);

	if (file_to_buffer(file_name, &buffer) == 0)
		goto exit;

	set_edit_buffer(&buffer, 0);
	
        int ch;

	bool insert_mode = false;

	//pthread_t auto_save_thread;
	//pthread_create(&auto_save_thread, 0, auto_save, (void*) &file_buffer_name);

	print_buffer(&buffer);

	//Used to know which line to add a char to
	struct buffer_node *current_line = buffer.head;

	while (1)
	{
		//Get the char that the user enters
		ch = getch();

		//Quit
		if (ch == KEY_F(2))
		{
			buffer_to_file(file_buffer_name, &buffer);

			free_buffer(&buffer);

			endwin();

			goto exit;
		}

		//Delete key
		else if (ch == KEY_BACKSPACE || ch == KEY_DL || ch == 127)
		{
			if (buffer.x > 0)
			{
				buffer.x--;
				delete_character(current_line, buffer.x);
				if (buffer.x == 0)
				{
					current_line->head = NULL;
					current_line->length = 0;
				}
			}
			
			else if (buffer.y > 0)
			{
				buffer.y--;
				get_prev_line(buffer, &current_line);
				buffer.x = current_line->length;

				if (current_line->length == 0)
				{
					current_line->head = current_line->next->head;
				}

				else if (current_line->next->head != NULL)
				{
					struct character *tmp;
					tmp = get_letter(current_line, current_line->length-1);
					tmp->next = current_line->next->head;
				}

				current_line->length += current_line->next->length;

				if (current_line->next->next != NULL)
				{
				
					delete_line(&buffer, current_line);

					struct buffer_node* tmp_line = current_line->next;

					for (tmp_line; tmp_line != 0; tmp_line = tmp_line->next)
						tmp_line->lineno--;
				}

				else
				{
					free(current_line->next);
					current_line->next = NULL;
					buffer.tail = NULL;
					buffer.node_count--;
				        
				}
			}

			set_edit_buffer(&buffer, 0);
		}//Only issues when last line

		//Forward arrow
		else if (ch == KEY_RIGHT)
		{
			buffer.x++;

			if (buffer.x > current_line->length && current_line->next != 0)
			{
				current_line = current_line->next;
				buffer.x = 0;
				buffer.y++;
			}

			else if (buffer.x > current_line->length)
				buffer.x = current_line->length;
		}

		//Backward arrow
		else if (ch == KEY_LEFT)
		{
			buffer.x--;
			
			if (buffer.x < 0 && buffer.y > 0)
			{
				//TODO Get hold of the previous line
				get_prev_line(buffer, &current_line);
			        buffer.x = current_line->length;
			        buffer.y--;
			}

		        if (buffer.x < 0)
				buffer.x = 0;
		}

		//Up arrow
		else if (ch == KEY_UP)
		{
			if (buffer.y > 0)
			{
				buffer.y--;
				get_prev_line(buffer, &current_line);
				if (buffer.x > current_line->length)
				{
					if (current_line->length != 0)
						buffer.x = current_line->length;
					else
						buffer.x = 0;
				}
			}
		}

		//Down arrow
		else if (ch == KEY_DOWN)
		{
			if (current_line->next != 0)
			{
				buffer.y++;

				current_line = current_line->next;

				if (buffer.y < buffer.node_count)
				{
					set_edit_buffer(&buffer, 0);
				}

				if (buffer.x > current_line->length)
				{
					if (current_line->length == 1)
						buffer.x = 0;
					else
						buffer.x = current_line->length;
				}
			}
		}

		//Enter key
		else if (ch == 10 || ch == KEY_ENTER)
		{
			struct buffer_node *new_line = (buffer_node*)malloc(sizeof(buffer_node));

			//Create new line
			new_line->next = current_line->next;
			current_line->next = new_line;

			if (buffer.x != 0)
			{
				struct character *tmp_ch;
				tmp_ch = get_letter(current_line, buffer.x-1);

				struct character *tmp_next = tmp_ch->next;

				tmp_ch->next = NULL;
				
				//Set Lengths of the lines
				new_line->length = current_line->length - buffer.x;

				//Set the head for the new line
				new_line->head = tmp_next;
			}

			else
			{
				new_line->head = current_line->head;
				new_line->length = current_line->length;
				current_line->head = NULL;
			}

			new_line->lineno = current_line->lineno;
			current_line->length = buffer.x;
			current_line = new_line;

			struct buffer_node *tmp = current_line;

			size_t i = tmp->lineno;

			for (tmp; tmp != NULL; tmp = tmp->next)
			{
				i++;
				tmp->lineno = i;
			}

			buffer.node_count++;
			buffer.y++;
			buffer.x = 0;

			set_edit_buffer(&buffer, 0);
		}

		//Save file
		else if ((ch == 's' || ch == 'S') && insert_mode)
		{
			//pthread_join(auto_save_thread, 0);
			buffer_to_file(file_name, &buffer);
			free_buffer(&buffer);
			endwin();
			break;
		}

		//Escape key
		else if (ch == 0x1B)
		{
		        insert_mode = !insert_mode;
		}

		else if (!insert_mode)
		{
			if (buffer.x > current_line->length)
				buffer.x = current_line->length;

		        else
				add_char_to_line(current_line, ch, buffer.x);

		        buffer.x++;

			if (buffer.x >= current_line->length - 1 && current_line->next == NULL)
			{
				current_line->next = malloc(sizeof(buffer_node));
				current_line->next->next = NULL;
				current_line->next->length = 0;
				current_line->next->lineno = current_line->lineno + 1;
				current_line->next->head = NULL;
				buffer.tail = NULL;
				buffer.node_count++;

				set_edit_buffer(&buffer, 0);
			}
		}

		/*if (buffer.x > 1 && (((buffer.x + 1) % (buffer.text_win->h - 2)) == 0))
		{
			set_edit_buffer(&buffer, (buffer.x+1)-((buffer.text_win->h)/2));
			}*/

		wclear(buffer.text_win->win);
		print_buffer(&buffer);
	}
	
exit:
        return 0;
}


//Change edit nodes when y % buffer height == 0
