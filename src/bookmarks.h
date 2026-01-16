#ifndef BOOKMARKS_H

#define BOOKMARKS_H

#define BOOKMARK_FILE "bookmarks.tsv"

void print_helper(void);
int init_bookmark(void);
int add_bookmark(char *bookmark, char *directory);

#endif
