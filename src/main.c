#include <stdio.h>
#include <stdlib.h>

#include <fileops.h>
#include <textcolor.h>

int main(int argc, char *argv[])
{
	system ("/bin/stty raw");
	
	FILE *fp = create_file_buffer("/home/saxon/Programming/userspace/text_editor/text_editor.txt");
	fclose(fp);

	int ret = copy_file("/home/saxon/Programming/userspace/text_editor/text_editor.txt", "/home/saxon/Programming/userspace/text_editor/text_editor.txt~");

	printf("%d\n", ret);
	

	system ("/bin/stty cooked");
	
        //textcolor(BRIGHT, RED, BLACK);
	//printf("In color\n");
	//textcolor(RESET, WHITE, BLACK);
	
	return 0;
}
