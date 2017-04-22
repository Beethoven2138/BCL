#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdbool.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include <tui.h>

/*
The buffer is structured by having a singly linked list of lines called buffer_node
Each buffer_node contains a nested linked list for the characters that the line contains
This is to allow for easily changing the contents of the buffer. 
 */

typedef struct character
{
	char value;
	struct character *next;
} character;

typedef struct buffer_node
{
	size_t length;
        size_t lineno;//TODO: add this in all the functions
	struct character *head;
	struct buffer_node *next;
	size_t xPos;
} buffer_node;

typedef struct text_buffer
{
	char *file_name;
	char *file_buffer_name;
	bool modified;
	int x, y;
	int xPos, yPos;
	struct buffer_node *head;
	struct buffer_node *tail;
        size_t node_count;
	struct buffer_node *current_line;

	struct buffer_node *edit_start;
	struct buffer_node *edit_end;

	struct window_data *text_win;
	struct window_data *command_win;
	//struct window_data *title_win;

	//TODO: Add support for linked list of buffers
	struct text_buffer *next;
	struct text_buffer *prev;
} text_buffer;

//extern struct winsize w;
extern struct text_buffer buffer;

void add_char_to_line(struct buffer_node *line, char value, int position);

int file_to_buffer(char *file_name, struct text_buffer *buffer);

void free_buffer(struct text_buffer *buffer_to_free);

struct character* get_letter(struct buffer_node *node, size_t position);

void get_line(struct buffer_node *node, char *contents, size_t length);

void get_line_node(struct text_buffer *buffer, struct buffer_node **line, int num);

void delete_character(struct buffer_node *line, unsigned int position);

void print_buffer(struct text_buffer *buffer);

void print_line(struct text_buffer *buffer, struct buffer_node *line);

void get_prev_line(struct text_buffer buffer, struct buffer_node **line);

void delete_line(struct text_buffer *buffer, struct buffer_node *prev);

void init_buffer(struct text_buffer *buffer);

void set_edit_buffer(struct text_buffer *buffer, int line);

static int longest_line_length(struct text_buffer buffer);

void update_environment(struct text_buffer *buffer);

#endif

//int mvwprintw(WINDOW *win, int y, int x, char *fmt [, arg] ...);
