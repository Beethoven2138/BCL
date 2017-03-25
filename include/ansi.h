#ifndef ANSI_H
#define ANSI_H

#define RESET		0
#define BRIGHT 		1
#define DIM		2
#define UNDERLINE 	3
#define BLINK		4
#define REVERSE		7
#define HIDDEN		8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7

void textcolor(int attr, int fg, int bg);

void cursor_up(char lines);
void cursor_down(char lines);
void cursor_right(char spaces);
void cursor_left(char spaces);

void clear();
void clear_line();

void move_cursor(char line, char column);

struct cursor
{
	unsigned int line;
	unsigned int column;
};

extern struct cursor cursor;

#endif
