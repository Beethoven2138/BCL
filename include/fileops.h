#ifndef FILEOPS_H
#define FILEOPS_H

#include <stdio.h>
#include <buffer.h>
#include <string.h>
#include <stdbool.h>

char* create_file_buffer(char *file_name);

int copy_file(char *src_file_name, char *dest_file_name);

int buffer_to_file(char *file_name, struct text_buffer *buffer);

void* auto_save(void *file_buffer_name);

#endif
