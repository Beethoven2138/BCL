#include <command.h>

void goto_line(struct text_buffer *buffer)
{
	
	char input[buffer->command_win->w];	
	nocbreak();
	echo();

	bool valid = false;
	
	while (!valid)
	{
		werase(buffer->command_win->win);
		box(buffer->command_win->win, 0, 0);
		wmove(buffer->command_win->win, 1, 1);
		wprintw(buffer->command_win->win, "GOTO: ");
		wrefresh(buffer->command_win->win);
		wgetstr(buffer->command_win->win, input);

		for (int i = 0; i < strlen(input); i++)
		{
			if (input[i] == 'q' || input[i] == 'Q')
				goto exit;

			if (input[i] < 48 || input[i] > 57)
			{
				valid = false;
				break;
			}

			valid = true;
		}
	}

	unsigned int line = (unsigned int)strtol(input, (char**)NULL, 10);

	if (line != ERANGE && line != EINVAL)
	{
		if (line > buffer->node_count)
			buffer->current_line = buffer->tail;

		else
			get_line_node(buffer, &(buffer->current_line), line - 1);

		buffer->y = buffer->current_line->lineno - 1;
		buffer->yPos = buffer->y;//COULD BE AN ISSUE!!!
		buffer->x = buffer->current_line->xPos;
		buffer->xPos = buffer->x;

		//THIS IS SO MUCH BETTER!!!
	        set_edit_buffer(buffer, (buffer->text_win->h-2) * (buffer->current_line->lineno / (buffer->text_win->h-2)));
		buffer->yPos = buffer->current_line->lineno - buffer->edit_start->lineno;
		//set_edit_buffer(&buffer, buffer.current_line->lineno - 1);
	}
exit:
	cbreak();
	noecho();
}

int quit_state(bool modified, struct text_buffer *buffer)
{
	if (!modified)
		return 1;

	char input;
	echo();

	while (1)
	{	
		werase(buffer->command_win->win);
		box(buffer->command_win->win, 0, 0);
		wmove(buffer->command_win->win, 1, 1);
		wprintw(buffer->command_win->win, "Modified buffers exist. Exit anyway? (y/n): ");
		wrefresh(buffer->command_win->win);
	        input = wgetch(buffer->command_win->win);

		if (input == 'n' || input == 'N')
		{
			input = 0;
			goto exit;
		}

		if (input == 'y' || input == 'Y')
		{
			input = 1;
			goto exit;
		}
	}
exit:
	noecho();
	return input;
}

int shell_state(struct text_buffer *buffer)
{
	char input[buffer->command_win->w];	
	nocbreak();
	echo();

	bool valid = false;

	werase(buffer->command_win->win);
	box(buffer->command_win->win, 0, 0);
	wmove(buffer->command_win->win, 1, 1);
	wprintw(buffer->command_win->win, "Shell command: ");
	wrefresh(buffer->command_win->win);
	wgetstr(buffer->command_win->win, input);

	cbreak();
	echo();
        return system(input);
}

void read_state(struct text_buffer *buffer)
{
	char input[buffer->command_win->w];	
	nocbreak();
	echo();

	while (1)
	{
		werase(buffer->command_win->win);
		box(buffer->command_win->win, 0, 0);
		wmove(buffer->command_win->win, 1, 1);
		wprintw(buffer->command_win->win, "File to insert: ");
		wrefresh(buffer->command_win->win);
		wgetstr(buffer->command_win->win, input);

		if ((input[0] == 'q' || input[0] == 'Q') && strlen(input) == 1)
			goto exit;

		char *tmp = create_file_buffer(input);
		if (tmp == 0)
			continue;

		buffer_to_file(buffer->file_name, buffer);
		free_buffer(buffer);
		//buffer->file_name = input;
		buffer->file_name = (char*)malloc((strlen(input) + 1) * sizeof(char)); 
		strncpy(buffer->file_name, input, strlen(input));
		buffer->file_name[strlen(input)] = 0;
		buffer->file_buffer_name = create_file_buffer(buffer->file_name);

		if (buffer->file_buffer_name == 0)
			exit(1);

		init_buffer(buffer);

		if (file_to_buffer(buffer->file_name, buffer) == 0)
			exit(1);

		set_edit_buffer(buffer, 0);
		buffer->current_line = buffer->head;
		print_buffer(buffer);
		goto exit;
	}
exit:
	cbreak();
	noecho();
}
