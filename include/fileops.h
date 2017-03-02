#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdio.h>
#include <buffer.h>

char* create_file_buffer(char *file_name);

int copy_file(char *src_file_name, char *dest_file_name);

int buffer_to_file(char *file_name, char *permissions, struct text_buffer *buffer);

#endif
