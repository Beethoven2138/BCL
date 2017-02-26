#include <textcolor.h>

#include <stdio.h>

void textcolor(int attr, int fg, int bg)
{
	char command[13];

	/* Command is the control command to the terminal */
	sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
	printf("%s", command);
}
