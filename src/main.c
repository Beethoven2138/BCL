/*  text_editor is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    text_editor is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with text_editor.  If not, see <http://www.gnu.org/licenses/>.*/

//AUTHOR: SAXON SUPPLE

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
