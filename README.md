# text_editor
A command line based text editor


PLAN:

Upon opening a file, the information in the .txt file will be copied into a .txt~ file (backup). This will be periodically autosaved on a separate pthread so that it doesn't hurt the performance of the main thread.

A linked list will be used to hold the buffer that gets dynamically modified. When the user opens a file, the contents will also be copied to the linked list.

The text file will be broken into separate nodes. This will allow for the file to be dynamically altered in size and for faster searching through different lines and words.

If you want to help, email me at saxon.supple@gmail.com


NOTE:
The program recieves segmentation faults when compiled with -O3. This will be fixed by making the linked list volatile.
