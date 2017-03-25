#ifndef BUFFER_H
#define BUFFER_H

#include <unistd.h>
#include <stdbool.h>

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
	int length;
	struct character *head;
	struct buffer_node *next;
} buffer_node;

typedef struct text_buffer
{
	struct buffer_node *head;
	struct buffer_node *tail;
	unsigned int node_count;
} text_buffer;

extern struct winsize w;
extern struct text_buffer buffer;

void add_char_to_line(struct buffer_node *line, char value, int position);

int file_to_buffer(char *file_name, struct text_buffer *buffer);

void free_buffer(struct text_buffer *buffer);

char get_letter(struct buffer_node *node, unsigned int position);

void get_line(struct buffer_node *node, char *contents, unsigned int length);

void get_line_node(struct text_buffer *buffer, struct buffer_node **line, int num);

void delete_character(struct buffer_node *line, unsigned int position);

void print_buffer(struct text_buffer *buffer);

int get_line_length(struct buffer_node *line);

void get_prev_line(struct text_buffer buffer, struct buffer_node **line);

#endif
