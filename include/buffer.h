#ifndef BUFFER_H
#define BUFFER_H

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

void add_char_to_line(struct buffer_node *line, char value, int position);

int file_to_buffer(char *file_name, struct text_buffer *buffer);

void free_buffer(struct text_buffer *buffer);

char get_letter(struct buffer_node *node, unsigned int position);

void get_line(struct buffer_node *node, char *contents, unsigned int length);

#endif
