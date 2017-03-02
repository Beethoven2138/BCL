#ifndef BUFFER_H
#define BUFFER_H

#define BUFFER_NODE_LENGTH 80

typedef struct buffer_node
{
	unsigned int length;//TODO set the length of the text
	char text[BUFFER_NODE_LENGTH];
	struct buffer_node *next;
	struct buffer_node *prev;
} buffer_node;

typedef struct text_buffer
{
	struct buffer_node *head;
	struct buffer_node *tail;
	unsigned int node_count;
} text_buffer;

int file_to_buffer(char *file_name, struct text_buffer *buffer, struct buffer_node *node);

#endif
