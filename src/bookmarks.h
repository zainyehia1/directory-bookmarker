#ifndef BOOKMARKS_H

#define BOOKMARKS_H

#define BOOKMARK_FILE "bookmarks.tsv"
#define MAX_LINE 512

void print_helper(void);
int init_bookmark(void);
int add_bookmark(char *bookmark, char *directory);
int list_bookmarks(void);

#endif
