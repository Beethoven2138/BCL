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

//AUTHOR: SAXON SUPPLE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <ncurses.h>
#include <unistd.h>
#include <signal.h>

#include <fileops.h>
#include <buffer.h>
#include <tui.h>
#include <command.h>

//valgrind --leak-check=full ./bcl

//TODO: Get rid of the retarded xPos and yPos in text_buffer as not needed for finding cursor pos

struct text_buffer buffer;

void sigint_handler(void);

int main(int argc, char *argv[])
{
	signal(SIGINT, sigint_handler);

	//If the user entered the address of the file that he wants to open
	if (argc == 2)
	{
	        buffer.file_name = argv[1];
		buffer.file_buffer_name = create_file_buffer(/*argv[1]*/buffer.file_name);
		if (buffer.file_buffer_name == 0)
		        exit(1);
	}

	else
	        exit(1);

	if (buffer.file_buffer_name == NULL)
	        exit(1);

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	noecho();
	refresh();

	init_buffer(&buffer);

	if (file_to_buffer(buffer.file_name, &buffer) == 0)
	        exit(1);

	set_edit_buffer(&buffer, 0);

        int ch;

	bool insert_mode = false;

	//pthread_t auto_save_thread;
	//pthread_create(&auto_save_thread, 0, auto_save, (void*) &file_buffer_name);
        buffer.current_line = buffer.head;

	print_buffer(&buffer);

	size_t chars_entered = 0;

	while (1)
	{
		//Get the char that the user enters
		ch = getch();

		//Quit
		if (ch == KEY_F(2))
		{
			buffer_to_file(buffer.file_buffer_name, &buffer);

			free_buffer(&buffer);

			endwin();

		        exit(0);
		}

		//Save file
		else if (ch == KEY_F(5))
		{
			//pthread_join(auto_save_thread, 0);
			buffer_to_file(buffer.file_name, &buffer);
			buffer.modified = false;
		}

		else if (ch == KEY_F(3))
		{
			if (quit_state(buffer.modified, &buffer))
			{
				char str[100];
				sprintf(str, "rm %s", buffer.file_buffer_name);
				system(str);
				free_buffer(&buffer);
				endwin();
				break;
			}
		}

		else if (ch == KEY_F(4))
			goto_line(&buffer);

		else if (ch == KEY_F(6))
			shell_state(&buffer);

		else if (ch == KEY_F(7))
			read_state(&buffer);

		else if (ch == KEY_HOME)
		{
			buffer.x = 0;
			buffer.xPos = 0;
		}

		else if (ch == KEY_END)
		{
			buffer.x = buffer.current_line->length;
			buffer.xPos = buffer.current_line->length;
		}

		//Delete key
		else if (ch == KEY_BACKSPACE || ch == KEY_DL || ch == 127 || ch == 8)
		{
			buffer.modified = true;
			if (buffer.x > 0)
			{
				buffer.x--;
				buffer.xPos--;
				delete_character(buffer.current_line, buffer.x);
				/*if (buffer.x == 0)
				{
					current_line->head = NULL;
					current_line->length = 0;
				}*/
			}

			else if (buffer.y > 0)
			{
				buffer.y--;
				buffer.yPos--;
				get_prev_line(buffer, &(buffer.current_line));
				buffer.x = buffer.current_line->length;
				buffer.xPos = buffer.current_line->length;

				if (buffer.current_line->length == 0)
				{
					buffer.current_line->head = buffer.current_line->next->head;
				}

				else if (buffer.current_line->next->head != NULL)
				{
					struct character *tmp;
					tmp = get_letter(buffer.current_line, buffer.current_line->length-1);
					tmp->next = buffer.current_line->next->head;
				}

				buffer.current_line->length += buffer.current_line->next->length;

				if (buffer.current_line->next->next != NULL)
				{
					delete_line(&buffer, buffer.current_line);

					struct buffer_node* tmp_line = buffer.current_line->next;

					for (tmp_line; tmp_line != 0; tmp_line = tmp_line->next)
						tmp_line->lineno--;
				}

				else
				{
					free(buffer.current_line->next);
					buffer.current_line->next = NULL;
					buffer.tail = buffer.current_line;
					buffer.node_count--;
				}

				if (buffer.yPos == 0 && buffer.y > (buffer.text_win->h/2)-1)
				{
					set_edit_buffer(&buffer, buffer.y - buffer.text_win->h/2);
					buffer.yPos = (buffer.text_win->h-1)/2;
				}
			}

			set_edit_buffer(&buffer, buffer.edit_start->lineno-1);
		}

		//Forward arrow
		else if (ch == KEY_RIGHT)
		{
			buffer.x++;
			buffer.xPos++;

			if (buffer.x > buffer.current_line->length && buffer.current_line->next != 0)
			{
				buffer.current_line = buffer.current_line->next;
				buffer.x = 0;
				buffer.xPos = 0;
				buffer.y++;
				buffer.yPos++;
			}

			else if (buffer.x > buffer.current_line->length)
			{
				buffer.x = buffer.current_line->length;
				buffer.xPos = buffer.current_line->length;
			}
		}

		//Backward arrow
		else if (ch == KEY_LEFT)
		{
			buffer.x--;
			buffer.xPos--;

			if (buffer.x < 0 && buffer.y > 0)
			{
				//TODO Get hold of the previous line
				get_prev_line(buffer, &(buffer.current_line));
			        buffer.x = buffer.current_line->length;
				buffer.xPos = buffer.current_line->length;
			        buffer.y--;
				buffer.yPos--;
			}

		        if (buffer.x < 0)
			{
				buffer.x = 0;
				buffer.xPos = 0;
			}
		}

		//Up arrow
		else if (ch == KEY_UP)
		{
			if (buffer.y > 0)
			{
				buffer.y--;
				buffer.yPos--;
				get_prev_line(buffer, &(buffer.current_line));
				if (buffer.x > buffer.current_line->length)
				{
					if (buffer.current_line->length != 0)
					{
						buffer.x = buffer.current_line->length;
						buffer.xPos = buffer.current_line->length;
					}

					else
					{
						buffer.x = 0;
						buffer.xPos = 0;
					}
				}

				else if (buffer.current_line->next->length == buffer.x &&
				    buffer.current_line->xPos >= buffer.current_line->next->xPos)
				{
					buffer.x = buffer.current_line->xPos;
					buffer.xPos = buffer.x;
				}

				if (buffer.yPos == 0 && buffer.y > (buffer.text_win->h/2)-1)
				{
					set_edit_buffer(&buffer, buffer.y - buffer.text_win->h/2);
					buffer.yPos = (buffer.text_win->h-1)/2;
				}
			}
		}

		//Down arrow
		else if (ch == KEY_DOWN)
		{
			if (buffer.current_line->next != 0)
			{
				buffer.y++;
				buffer.yPos++;

				if (buffer.current_line->xPos == buffer.current_line->length &&
				    buffer.current_line->next->xPos >= buffer.current_line->xPos)
				{
					buffer.x = buffer.current_line->next->xPos;
					buffer.xPos = buffer.x;
				}

				buffer.current_line = buffer.current_line->next;

				/*if (buffer.y < buffer.node_count)
				{
					set_edit_buffer(&buffer, buffer.edit_start->lineno-1);
					}*/

				if (buffer.x > buffer.current_line->length)
				{
					if (buffer.current_line->length == 1)
						buffer.x = 0;

					else
						buffer.x = buffer.current_line->length;

					buffer.xPos = buffer.x;
				}

				if (buffer.yPos + 2 == buffer.text_win->h)
				{
					set_edit_buffer(&buffer, (buffer.edit_start->lineno-1) + buffer.text_win->h/2);
					buffer.yPos = (buffer.text_win->h-2)/2;
				}
			}
		}

		//Enter key
		else if (ch == 10 || ch == KEY_ENTER)
		{
			buffer.modified = true;
			struct buffer_node *new_line = (buffer_node*)malloc(sizeof(buffer_node));

			//Create new line
			new_line->next = buffer.current_line->next;
			buffer.current_line->next = new_line;

			if (buffer.x != 0)
			{
				struct character *tmp_ch;
				tmp_ch = get_letter(buffer.current_line, buffer.x-1);

				struct character *tmp_next = tmp_ch->next;

				tmp_ch->next = NULL;

				//Set Lengths of the lines
				new_line->length = buffer.current_line->length - buffer.x;

				//Set the head for the new line
				new_line->head = tmp_next;
			}

			else
			{
				new_line->head = buffer.current_line->head;
				new_line->length = buffer.current_line->length;
				buffer.current_line->head = NULL;
			}

			new_line->xPos = 0;
			new_line->lineno = buffer.current_line->lineno;
			buffer.current_line->length = buffer.x;
			buffer.current_line = new_line;

			struct buffer_node *tmp = buffer.current_line;

			size_t i = tmp->lineno;

			buffer.tail = tmp;
			
			for (tmp; tmp != NULL; tmp = tmp->next)
			{
				i++;
				tmp->lineno = i;

				if (tmp->next == NULL)
					buffer.tail = tmp;
			}

			buffer.node_count++;
			buffer.y++;
			buffer.yPos++;
			buffer.x = 0;
			buffer.xPos = 0;

			set_edit_buffer(&buffer, buffer.edit_start->lineno-1);
	
			//if (buffer.y > 1 && (buffer.y % (buffer.text_win->h - 3) == 0))
			/*if (buffer.y + 1 == buffer.edit_end->lineno &&
			    buffer.y + 2 >= buffer.text_win->h &&
			    buffer.edit_end->next != NULL)
			{
				buffer.edit_start = buffer.edit_start->next;
				buffer.edit_end = buffer.edit_end->next;
				}*/

			if (buffer.yPos + 2 == buffer.text_win->h)
			{
				set_edit_buffer(&buffer, (buffer.edit_start->lineno-1) + (int)buffer.text_win->h/2);
				buffer.yPos = (int)(buffer.text_win->h-2)/2;
			}
		}

		//Escape key
		else if (ch == 0x1B)
		{
		        insert_mode = !insert_mode;
		}

		else if (!insert_mode)
		{

			buffer.modified = true;
			/*if (buffer.x >= buffer.text_win->w - 6)//Check this
			{
				current_line->add_new_line = false;
			        struct buffer_node *tmp = (buffer_node*)malloc(sizeof(buffer_node));
				tmp->next = current_line->next;
				current_line->next = tmp;
				current_line->next->lineno = current_line->lineno;
				buffer.node_count++;
				current_line = current_line->next;
				current_line->length = 0;
				current_line->head = NULL;
				add_char_to_line(current_line, ch, 0);

				buffer.x = 0;
				buffer.xPos = 0;
				buffer.y++;
				buffer.yPos++;

				if (buffer.yPos + 2 == buffer.text_win->h)
				{
					set_edit_buffer(&buffer, (buffer.edit_start->lineno-1) + (int)buffer.text_win->h/2);
					buffer.yPos = (int)(buffer.text_win->h-2)/2;
				}

				set_edit_buffer(&buffer, buffer.edit_start->lineno - 1);
			}*/

		        //else

				add_char_to_line(buffer.current_line, ch, buffer.x);

		        buffer.x++;
			buffer.xPos++;

			if (buffer.x >= buffer.current_line->length - 1 && buffer.current_line->next == NULL)
			{
				buffer.current_line->next = malloc(sizeof(buffer_node));
				buffer.current_line->next->next = NULL;
				buffer.current_line->next->length = 0;
				buffer.current_line->next->lineno = buffer.current_line->lineno + 1;
				buffer.current_line->next->head = NULL;
				buffer.tail = buffer.current_line->next;
				buffer.node_count++;

				set_edit_buffer(&buffer, buffer.edit_start->lineno - 1);
			}

			if (++chars_entered >= 10)
			{
				chars_entered = 0;
				buffer_to_file(buffer.file_buffer_name, &buffer);
			}
		}

		buffer.current_line->xPos = buffer.x;

		wclear(buffer.text_win->win);
		//print_line(&buffer, buffer.current_line);Requires more work
		print_buffer(&buffer);
	}

        return 0;
}


void sigint_handler()
{
	command_state = (void*)quit_state;
	if (quit_state(buffer.modified, &buffer))
	{
		free_buffer(&buffer);
		endwin();
		exit(0);
	}
}
