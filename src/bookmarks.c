#include "bookmarks.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

// Helper functions
static void free_bookmarks(BookmarkNode *head);
static BookmarkNode *load_bookmarks(void);
static int save_bookmarks(BookmarkNode *head);
static void trim_trailing_space(char *name);
static BookmarkNode *find_bookmark(BookmarkNode *head, char *name);
static bool bookmark_exists(BookmarkNode *head, char *name);
static bool is_initialized(void);
static char *resolve_tilde(char *path);
static char *get_bookmark_file_path(void);
static char *get_bookmark_dir_path(void);

void print_helper(void) {
    printf("Usage: bm <command> [<args>]\n");
    printf("Commands:\n");
    printf("  init                                  Initialize bookmark system\n");
    printf("  add <name> <path>                     Add a bookmark\n");
    printf("  delete <name>                         Delete a bookmark\n");
    printf("  list                                  List all bookmarks\n");
    printf("  rename <old_name> <new_name>          Rename a bookmark\n");
    printf("  edit <name> <new_path>                Edit a bookmark's path\n");
    printf("  go <name>                             Print path of a bookmark\n");
    printf("  help                                  Print this message\n");
}

int init_bookmark(void) {
    char *dir_path = get_bookmark_dir_path();
    if (!dir_path) {
        printf("Error initializing bookmark system!\n");
        return 1;
    }
    if (mkdir(dir_path, 0700) == -1 && errno != EEXIST) {
        fprintf(stderr, "Failed to make directory %s: %s\n", dir_path, strerror(errno));
        free(dir_path);
    }
    free(dir_path);

    char *file_path = get_bookmark_file_path();
    if (!file_path) {
        printf("Error initializing bookmark system!\n");
        return 1;
    }
    FILE *file = fopen(file_path, "r");
    if (!file) {
        file = fopen(file_path, "w");
        if (file) {
            fprintf(file, "Bookmark Name\tDirectory Path\n");
            if (fclose(file) == -1) {
              fprintf(stderr, "Failed to close %s: %s\n", file_path, strerror(errno));
            }
            free(file_path);

            printf("Bookmark system initialized!\n");
            return 0;
        }
        else {
            fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
            free(file_path);
            return 1;
        }
    }
    else {
        if (fclose(file) == -1) {
            fprintf(stderr, "Failed to close %s: %s\n", file_path, strerror(errno));
        }
        free(file_path);

        printf("Bookmark system already initialized!\n");
        return 0;
    }
}

int add_bookmark(char *name, char *path) {
    if (!is_initialized()) {
        printf("Error adding bookmark to file!\n");
        printf("You haven't initialized the bookmark system yet.\n");
        printf("Run 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    if (strlen(name) >= MAX_NAME) {
        printf("The bookmark name is too long. Try again.\n");
        return 1;
    }

    if (strlen(path) >= MAX_PATH) {
        printf("The directory path is too long. Try again.\n");
        return 1;
    }

    char *tilde_expanded = resolve_tilde(path);
    if (!tilde_expanded) {
        printf("Error: Could not resolve the path.\n");
        return 1;
    }
    char *resolved_path = realpath(tilde_expanded, NULL);
    if (tilde_expanded != path) free(tilde_expanded);

    if (!resolved_path) {
        if (path[0] == '~') {
            printf("'%s' is not a valid path.\n", path);
            printf("Tilde paths must start with '~/' (e.g., ~/Desktop)\n");
            return 1;
        }
        fprintf(stderr, "Failed to resolve %s: %s\n", path, strerror(errno));
        return 1;
    }

    BookmarkNode *head = load_bookmarks();

    if (head) {
        if (bookmark_exists(head, name)) {
            printf("Error: A bookmark named '%s' already exists --> %s\n", name, find_bookmark(head, name)->bookmark.path);
            printf("Try using a different name.\n");
            free(resolved_path);
            free_bookmarks(head);
            return 1;
        }
    }

    BookmarkNode *added_bookmark = malloc(sizeof(BookmarkNode));
    if (!added_bookmark) {
        fprintf(stderr, "Failed to allocate memory for bookmark: %s\n", strerror(errno));
        free(resolved_path);
        free_bookmarks(head);
        return 1;
    }
    strcpy(added_bookmark->bookmark.name, name);
    strcpy(added_bookmark->bookmark.path, resolved_path);
    free(resolved_path);
    added_bookmark->next = NULL;

    if (!head) {
        head = added_bookmark;
    }
    else {
        BookmarkNode *temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = added_bookmark;
    }

    save_bookmarks(head);
    free_bookmarks(head);
    printf("Bookmark added successfully!\n");
    return 0;
}

int list_bookmarks(void) {
    if (!is_initialized()) {
        printf("You haven't initialized the bookmark system yet.\n");
        printf("Run 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();

    if (!head) {
        printf("+------------------+------------------+\n");
        printf("|  Bookmark Name   |  Directory Path  |\n");
        printf("+------------------+------------------+\n");
        printf("|          No bookmarks yet           |\n");
        printf("+------------------+------------------+\n");
        return 0;
    }

    int longest_path = strlen(head->bookmark.path);

    BookmarkNode *temp = head;
    while (temp) {
        int len = strlen(temp->bookmark.path);
        if (len > longest_path) longest_path = len;
        temp = temp->next;
    }

    printf("+-----------------+");
    for (int i = 0; i < longest_path + 2; i++) {
        printf("-");
    }
    printf("+\n");
    printf("| %-15s | %-*s |\n", " Bookmark Name", longest_path, "Directory Path");
    printf("+-----------------+");
    for (int i = 0; i < longest_path + 2; i++) {
        printf("-");
    }
    printf("+\n");

    temp = head;
    while (temp) {
        printf("| %-15s | %-*s |\n", temp->bookmark.name, longest_path, temp->bookmark.path);
        temp = temp->next;
    }
    printf("+-----------------+");
    for (int i = 0; i < longest_path + 2; i++) {
        printf("-");
    }
    printf("+\n");

    free_bookmarks(head);
    return 0;
}

int delete_bookmark(char *name) {
    if (!is_initialized()) {
        printf("Error deleting bookmark from file!\n");
        printf("You haven't initialized the bookmark system yet.\n");
        printf("Run 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, name);

    if (!head) {
        printf("You don't have any bookmarks yet.\n");
        printf("Use bm add <name> <path> to add one.\n");
        return 1;
    }

    if (target) {
        if (head == target) {
            head = head->next;
            free(target);
            printf("Bookmark '%s' deleted successfully!\n", name);
        }
        else {
            BookmarkNode *temp = head;
            while (temp->next != target) {
                temp = temp->next;
            }
            temp->next = target->next;
            free(target);
            printf("Bookmark '%s' deleted successfully!\n", name);
        }
    }
    else {
        printf("Error: There isn't a bookmark named '%s' to delete.\n", name);
        printf("Use 'bm add %s <path>' to add one.\n", name);
        free_bookmarks(head);
        return 1;
    }

    save_bookmarks(head);
    free_bookmarks(head);
    return 0;
}

int rename_bookmark(char *old_name, char *new_name) {
    if (!is_initialized()) {
        printf("Error renaming bookmark in file!\n");
        printf("You haven't initialized the bookmark system yet.\n");
        printf("Run 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, old_name);

    if (!head) {
        printf("You don't have any bookmarks yet.\n");
        printf("Use bm add <name> <path> to add one.\n");
        return 1;
    }

    if (target) {
        if (!bookmark_exists(head, new_name)) {
            if (strlen(new_name) >= MAX_NAME) {
                printf("The new bookmark name is too long. Try again\n");
                free_bookmarks(head);
                return 1;
            }
            strcpy(target->bookmark.name, new_name);

            printf("Bookmark '%s' has been renamed successfully!\n", old_name);
            printf("'%s' --> %s\n", new_name, target->bookmark.path);
        }
        else {
            printf("There is already a bookmark named '%s'.\n", new_name);
            free_bookmarks(head);
            return 1;
        }
    }
    else {
        printf("Error: There isn't a bookmark named '%s' to rename.\n", old_name);
        printf("Use 'bm add %s <path>' to add one.\n", old_name);
        free_bookmarks(head);
        return 1;
    }

    save_bookmarks(head);
    free_bookmarks(head);
    return 0;
}

int edit_path(char *name, char *new_path) {
    if (!is_initialized()) {
        printf("Error editing bookmark path in file!\n");
        printf("You haven't initialized the bookmark system yet.\n");
        printf("Run 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    char *tilde_expanded = resolve_tilde(new_path);
    if (!tilde_expanded) {
        printf("Error: Could not resolve the path.\n");
        return 1;
    }
    char *resolved_path = realpath(tilde_expanded, NULL);
    if (tilde_expanded != new_path) free(tilde_expanded);

    if (!resolved_path) {
        if (new_path[0] == '~') {
            printf("'%s' is not a valid path.\n", new_path);
            printf("Tilde paths must start with '~/' (e.g., ~/Desktop)\n");
            return 1;
        }
        printf("'%s' is not a valid path.\n", new_path);
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, name);

    if (!head) {
        printf("You don't have any bookmarks yet.\n");
        printf("Use bm add <name> <path> to add one.\n");
        return 1;
    }

    if (target) {
        if (strlen(resolved_path) >= MAX_PATH) {
            printf("The new directory path is too long. Try again.\n");
            free(resolved_path);
            free_bookmarks(head);
            return 1;
        }
        strcpy(target->bookmark.path, resolved_path);
        printf("Bookmark '%s' has been edited successfully!\n", name);
        printf("'%s' --> %s\n", name, resolved_path);
        free(resolved_path);
    }
    else {
        printf("Error: There isn't a bookmark named '%s' to edit.\n", name);
        printf("Use 'bm add %s %s to add it.\n", name, resolved_path);
        free(resolved_path);
        free_bookmarks(head);
        return 1;
    }

    save_bookmarks(head);
    free_bookmarks(head);
    return 0;
}

int go(char *name) {
    if (!is_initialized()) {
        fprintf(stderr,"You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, name);

    if (!head) {
        fprintf(stderr, "You don't have any bookmarks yet.\n");
        fprintf(stderr, "Use bm add <name> <path> to add one.\n");
        return 1;
    }

    if (target) {
        printf("%s\n", target->bookmark.path);
    }
    else {
        fprintf(stderr, "'%s' is not a valid bookmark.\n", name);
        free_bookmarks(head);
        return 1;
    }

    free_bookmarks(head);
    return 0;
}

// Helper functions

/*
 * Frees all bookmarks in the linked list.
 */
static void free_bookmarks(BookmarkNode *head) {
    BookmarkNode *temp = head;

    while (temp) {
        BookmarkNode *next = temp->next;
        free(temp);
        temp = next;
    }
}

/*
 * Load bookmarks from bookmarks.tsv file into  linked list.
 * Returns head of list, or NULL if file doesn't exist or if file is empty.
 * Caller must free the returned list using free_bookmarks.
 */
static BookmarkNode *load_bookmarks(void) {
    BookmarkNode *head = NULL;
    char *file_path = get_bookmark_file_path();

    FILE *file = fopen(file_path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s: %s\n", file_path, strerror(errno));
        return NULL;
    }
    char line [MAX_LINE];

    fgets(line, MAX_LINE, file); // Skip headers

    while (fgets(line, MAX_LINE, file)) {
        char *name = strtok(line,"\t");
        char *path = strtok(NULL, "\n");

        if (name && path){
            BookmarkNode *bookmark_node = malloc(sizeof(BookmarkNode));
            if (!bookmark_node) {
                printf("Failed to load bookmarks due to insufficient memory.\n");
                free_bookmarks(head);
                free(file_path);
                return NULL;
            }

            trim_trailing_space(name);

            strcpy(bookmark_node->bookmark.name, name);
            strcpy(bookmark_node->bookmark.path, path);
            bookmark_node->next = NULL;
            if (!head) {
                head = bookmark_node;
            }
            else {
                BookmarkNode *temp = head;
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = bookmark_node;
            }
        }
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close %s: %s\n", file_path, strerror(errno));
    }
    
    free(file_path);
    
    return head;
}

/*
 * Overwrites bookmarks.tsv with the modified bookmarks from the linked list.
 * Returns 0 on success, 1 on error.
 */
static int save_bookmarks(BookmarkNode *head) {
    char *path = get_bookmark_file_path();
    FILE *file = fopen(path, "w");
    if (!file) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        free(path);
        return 1;
    }

    fprintf(file, "Bookmark Name\tDirectory Path\n");

    BookmarkNode *temp = head;
    while (temp) {
        fprintf(file, "%-15s\t%s\n", temp->bookmark.name, temp->bookmark.path);
        temp = temp->next;
    }

    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close %s: %s\n", path, strerror(errno));
    }

    free(path);

    return 0;
}

/*
 * Removes trailing whitespace from bookmark names.
 * Needed because TSV file uses fixed-width padding for alignment.
 */
static void trim_trailing_space(char *name) {
    for (int i = 0, len = strlen(name); i < len; i++) {
        if (isspace(name[i])) {
            name[i] = '\0';
            break;
        }
    }
}

static BookmarkNode *find_bookmark(BookmarkNode *head, char *name) {
    if (!head) return head;

    BookmarkNode *temp = head;
    while (temp && strcasecmp(temp->bookmark.name, name) != 0) {
        temp = temp->next;
    }

    return temp;
}

/*
 * Checks if the bookmark exists.
 * Returns true if it exists, false otherwise.
 */
static bool bookmark_exists(BookmarkNode *head, char *name) {
    return find_bookmark(head, name) != NULL;
}

/*
 * Checks if the bookmark system has been initialized.
 * Returns true if it is initialized, false otherwise.
 */
static bool is_initialized(void) {
    char *path = get_bookmark_file_path();
    FILE *file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Failed to open %s: %s\n", path, strerror(errno));
        free(path);
        return false;
    }
    if (fclose(file) == -1) {
        fprintf(stderr, "Failed to close %s: %s\n", path, strerror(errno));
    }
    free(path);
    return true;
}

/*
 * Expands ~ to HOME.
 * Returns string of absolute path if path starts with ~.
 * Otherwise, the original path's string is returned.
 */
static char *resolve_tilde(char *path) {
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) {
            printf("HOME environment variable is not set.\n");
            return path;
        }

        const char *path_after_tilde = path + 1;
        char *resolved_path = malloc(strlen(home) + strlen(path_after_tilde) + 1);
        if (!resolved_path) {
            fprintf(stderr, "Failed to allocate memory for resolved_path: %s\n", strerror(errno));
            return NULL;
        }

        strcpy(resolved_path, home);

        strcat(resolved_path, path_after_tilde);
        return resolved_path;
    }
    else {
        return path;
    }
}

/*
 * Returns the path to bookmarks.tsv.
 * If the HOME environment variable is not set, NULL is returned.
 */
static char *get_bookmark_file_path(void) {
    const char *home = getenv("HOME");
    if (!home) {
        printf("HOME environment variable is not set.\n");
        return NULL;
    }

    char *file_path = malloc(strlen(home) + strlen(BOOKMARK_DIRECTORY) + strlen(BOOKMARK_FILE) + 1); //home + "/.bm/bookmarks.tsv" + null terminator
    if (!file_path) {
            fprintf(stderr, "Failed to allocate memory for file_path: %s\n", strerror(errno));
            return NULL;
    }

    sprintf(file_path, "%s%s%s", home, BOOKMARK_DIRECTORY, BOOKMARK_FILE);
    return file_path;
}

/*
 * Returns the path to /.bm/ unless the HOME environment variable is not set
 * If it is not set, then NULL is returned
 */
static char *get_bookmark_dir_path(void) {
    const char *home = getenv("HOME");
    if (!home) {
        printf("HOME environment variable is not set.\n");
        return NULL;
    }

    char *dir_path = malloc(strlen(home) + strlen(BOOKMARK_DIRECTORY) + 1); //home + "/.bm/" + null terminator
    if (!dir_path) {
        fprintf(stderr, "Failed to allocate memory for dir_path: %s\n", strerror(errno));
        return NULL;
    }

    sprintf(dir_path, "%s%s", home, BOOKMARK_DIRECTORY);
    return dir_path;
}