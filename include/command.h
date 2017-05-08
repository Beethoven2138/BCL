#ifndef COMMAND_H
#define COMMAND_H

#include <buffer.h>
#include <fileops.h>
#include <tui.h>
#include <stdlib.h>
#include <errno.h>
#include <ncurses.h>
#include <string.h>

int quit_state(bool modified, struct text_buffer *buffer);
void default_state();
void goto_line(struct text_buffer *buffer);
void save_buffer();
void change_buffer();
int shell_state(struct text_buffer *buffer);
void read_state(struct text_buffer *buffer);

#endif
