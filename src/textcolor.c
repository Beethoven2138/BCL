#include <ansi.h>

#include <stdio.h>

void textcolor(int attr, int fg, int bg)
{
	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}

void cursor_up(char lines)
{
	printf("%c[%cA", 0x1b, (char)lines);
}

void cursor_down(char lines)
{
	printf("%c[%cB", 0x1b, (char)lines);
}

void cursor_right(char spaces)
{
	printf("%c[%cC", 0x1b, spaces);
}

void cursor_left(char spaces)
{
	printf("%c[%cD", 0x1b, spaces);
}

void move_cursor(char line, char column)
{
	printf("%c[%c;%cH", 0x1b, line, column);
}

void clear()
{
	printf("%c[2J", 0x1B);
}

void clear_line()
{
	printf("%c[K", 0x1B);
}
