#ifndef BOOKMARKS_H

#define BOOKMARKS_H

#define BOOKMARK_DIRECTORY "/.bm/"
#define BOOKMARK_FILE "bookmarks.tsv"

#define MAX_NAME 16        // Max buffer size (15 visible chars + '\0')
#define MAX_PATH 512       // Max buffer size (511 visible chars + '\0')

#define MAX_LINE (MAX_NAME + MAX_PATH + 2) // Max line in bookmarks.tsv file (MAX_NAME + MAX_PATH + \t + \n)

typedef struct {
    char name[MAX_NAME];
    char path[MAX_PATH];
} Bookmark;

typedef struct node{
    Bookmark bookmark;
    struct node *next;
} BookmarkNode;

/*
 * Prints usage information and available commands.
 */
void print_helper(void);

/*
 * Initialize the bookmark system by creating ~/.bm/ directory
 * and bookmarks.tsv file if they don't exist.
 * Returns 0 on success, 1 on error.
 */
int init_bookmark(void);

/*
 * Add a bookmark mapping name to path.
 * Validates path and rejects duplicates.
 * Returns 0 on success, 1 if not initialized or on error.
 */
int add_bookmark(char *name, char *path);

/*
 * List all the bookmarks in a formatted table.
 * Returns 0 on success, 1 if not initialized.
 */
int list_bookmarks(void);

/*
 * Remove bookmark by name.
 * Returns 0 on success, 1 if not initialized or if bookmark not found.
 */
int delete_bookmark(char *name);

/*
 * Rename a bookmark from old_name to new_name.
 * Returns 0 on success, 1 if not initialized or if a bookmark with new_name already exists.
 */
int rename_bookmark(char *old_name, char *new_name);

/*
 * Edit the path mapped to a certain bookmark.
 * Validates new path exists before editing.
 * Returns 0 on success, 1 if not initialized or on error.
 */
int edit_path(char *name, char *new_path);

/*
 * Prints the path of a bookmark to stdout for the shell wrapper.
 * Error messages are printed to stderr to not interfere with the shell wrapper.
 * Returns 0 on success, 1 if bookmark not found.
 */
int go(char *name);

#endif
