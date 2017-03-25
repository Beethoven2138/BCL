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

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <pthread.h>

#include <fileops.h>
#include <ansi.h>
#include <buffer.h>

#define FILENAME "/home/ssupple/Programming/text_editor-master/text_editor.txt"
#define FILEBUFFERNAME "/home/ssupple/Programming/text_editor-master/text_editor.txt~"

/*
  Start at 0,0.
  When user enters a char:
  if (! arrow key or enter or delete)
  Add it to the appropriate line at the correct position
  Then move the cursor to the new location
  
  else:
  
*/

//valgrind --leak-check=full ./text_editor

struct winsize w;
struct text_buffer buffer;

struct cursor cursor = {.line = 0, .column = 0};

int main(int argc, char *argv[])
{

	char *file_buffer_name;
	char *file_name;
	
        ioctl(0, TIOCGWINSZ, &w);

	//If the user entered he address of the file that he wants to open
	if (argc == 2)
	{
		file_buffer_name = create_file_buffer(argv[1]);
		char *file_name = argv[1];
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
        file_to_buffer(FILENAME, &buffer);

	char c;

	bool insert_mode = false;

	//pthread_t auto_save_thread;
	//pthread_create(&auto_save_thread, 0, auto_save, (void*) &file_buffer_name);

        system ("/bin/stty raw");

	move_cursor(0,0);

	print_buffer(&buffer);

	//Used to know which line to add a char to
	struct buffer_node *current_line = buffer.head;

	while (1)
	{
		//Get info on screen width and height
		ioctl(0, TIOCGWINSZ, &w);

		//Get the char that the user enters
		scanf("%c", &c);

		if (c > 0x7F)
		{
			c = 0;
			continue;
		}

		//Quit
		if (c == 'q')
		{
			clear();
			move_cursor(0,0);
			break;
		}

		//Delete key
		else if (c == 0x7F && !insert_mode)
		{
			delete_character(current_line, cursor.column-2);
			if (cursor.column > 0)
			{
				cursor.column--;
				if (cursor.column < 1 && cursor.line > 0)
				{
					//TODO Get hold of the previous line
					get_prev_line(buffer, &current_line);
					cursor.column = current_line->length+1;
					cursor.line--;
				}

				else if (cursor.column <= 0 && cursor.line == 0)
				{
					cursor.column = 0;
				}
			}
				
			else
			{
				if (cursor.line > 0)
				{
					cursor.line--;
					get_prev_line(buffer, &current_line);
					cursor.column = current_line->length;
				}
			}
		}

		//Forward arrow
		else if ((c == 'f'|| c == 'F') && insert_mode)
		{		
			if (cursor.column < current_line->length+1)
				cursor.column++;
			else
			{
				cursor.line++;
				if (current_line->next != 0)
				{
					current_line = current_line->next;
				}
			}
		}

		//Backward arrow
		else if ((c == 'b' || c == 'B') &&  insert_mode)
		{
			cursor.column--;
			
			if (cursor.column < 1 && cursor.line > 0)
			{
				//TODO Get hold of the previous line
				get_prev_line(buffer, &current_line);
				cursor.column = current_line->length+1;
				cursor.line--;
			}

			else if (cursor.column <= 0 && cursor.line == 0)
			{
				cursor.column = 0;
			}
		}

		//New line
		else if (c == 'n')
		{
			cursor.line++;

			if (current_line->next == 0)
			{
				current_line->next = (buffer_node*)malloc(sizeof(buffer_node));
				buffer.node_count++;
				buffer.tail = current_line->next;
				current_line->next->head = 0;
				current_line->next->next = 0;
				current_line->next->length = 0;
			}
			current_line = current_line->next;
		}

		//Save file
		else if (c == 'S' && insert_mode)
		{
			//pthread_join(auto_save_thread, 0);
			buffer_to_file(file_name, &buffer);
			break;
		}

		//Escape key
		else if (c == 0x1B)
		{
		        insert_mode = !insert_mode;
		}
		
		else if (!insert_mode)
		{
			if (cursor.column > current_line->length)
				add_char_to_line(current_line, c, current_line->length);

			else

				//system ("/bin/stty cooked");

				add_char_to_line(current_line, c, cursor.column);

			cursor.column++;

			clear();

			//move_cursor(0,2);
		}
		print_buffer(&buffer);

		move_cursor(0,0);
	}

        buffer_to_file(FILEBUFFERNAME, &buffer);

        free_buffer(&buffer);

        system ("/bin/stty cooked");

        return 0;
}
