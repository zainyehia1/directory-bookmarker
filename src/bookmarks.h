#ifndef BOOKMARKS_H

#define BOOKMARKS_H

#define BOOKMARK_FILE "bookmarks.tsv"

#define MAX_NAME 16
#define MAX_PATH 512

#define MAX_LINE 528

typedef struct {
    char name[MAX_NAME];
    char path[MAX_PATH];
} Bookmark;

typedef struct node{
    Bookmark bookmark;
    struct node *next;
} BookmarkNode;

void print_helper(void);
int init_bookmark(void);
int add_bookmark(char *name, char *path);
int list_bookmarks(void);
int delete_bookmark(char *name);
int rename_bookmark(char *old_name, char *new_name);
int edit_path(char *name, char *new_path);

#endif
