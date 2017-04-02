#ifndef TUI_H
#define TUI_H

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <string.h>

struct window_data
{
	int x, y, w, h;
	char label[80]; 
	int label_color;
	WINDOW *win;
};

WINDOW *create_new_win(int startx, int starty, int width, int height);
void destroy_win(WINDOW *local_win);

#endif
