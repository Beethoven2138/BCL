#ifndef TUI_H
#define TUI_H

#include <ncurses.h>
#include <panel.h>
#include <menu.h>
#include <string.h>

#define COLOR_DEFAULT 1
#define COLOR_COMMENT 2
#define COLOR_INCLUDE 3
#define COLOR_STDLIB 4
#define COLOR_TODO 5
#define COLOR_DATA 6
#define COLOR_CTL 7
#define COLOR_BOOL 8
#define COLOR_JUMP 9

struct window_data
{
	int x, y, w, h;
	char label[80]; 
	int label_color;
	WINDOW *win;
};

WINDOW *create_new_win(int startx, int starty, int width, int height);
WINDOW *simple_create_new_win(int startx, int starty, int width, int height);//No Box
void destroy_win(WINDOW *local_win);
void init_colors();

#endif
