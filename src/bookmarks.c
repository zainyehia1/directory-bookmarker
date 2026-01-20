#include "bookmarks.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

// Helper functions
static void free_bookmarks(BookmarkNode *head);
static BookmarkNode *load_bookmarks(void);
static int save_bookmarks(BookmarkNode *head);
static void trim_trailing_space(char *name);
static BookmarkNode *find_bookmark(BookmarkNode *head, char *name);
static bool bookmark_exists(BookmarkNode *head, char *name);
static bool is_initialized(void);
static char *resolve_tilde(char *path);

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
    FILE *file = fopen(BOOKMARK_FILE, "r");
    if (file == NULL) {
        file = fopen(BOOKMARK_FILE, "w");
        if (file != NULL) {
            fprintf(file, "Bookmark Name\tDirectory Path\n");
            fclose(file);

            printf("Bookmark system initialized!\n");
            return 0;
        }
        else {
            printf("Error initializing bookmark system!\n");
            return 1;
        }
    }
    else {
        fclose(file);

        printf("Bookmark system already initialized!\n");
        return 0;
    }
}

int add_bookmark(char *name, char *path) {
    if (!is_initialized()) {
        printf("Error adding bookmark to file!");
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    if (strlen(name) > MAX_NAME) {
        printf("The bookmark name is too long. Try again.\n");
        return 1;
    }

    if (strlen(path) > MAX_PATH) {
        printf("The directory path is too long. Try again.\n");
        return 1;
    }

    char *tilde_expanded = resolve_tilde(path);
    char *resolved_path = realpath(tilde_expanded, NULL);
    if (tilde_expanded != path) free(tilde_expanded);

    if (resolved_path == NULL) {
        if (path[0] == '~') {
            printf("'%s' is not a valid path.\n", path);
            printf("Tilde paths must start with '~/' (e.g., ~/Desktop)\n");
            return 1;
        }
        printf("'%s' is not a valid path.\n", path);
        return 1;
    }

    BookmarkNode *head = load_bookmarks();

    if (bookmark_exists(head, name)) {
        printf("Error: A bookmark named '%s' already exists --> %s.\n", name, find_bookmark(head, name)->bookmark.path);
        printf("Try using a different name.\n");
        free(resolved_path);
        free_bookmarks(head);
        return 1;
    }

    BookmarkNode *added_bookmark = malloc(sizeof(BookmarkNode));
    strcpy(added_bookmark->bookmark.name, name);
    strcpy(added_bookmark->bookmark.path, resolved_path);
    free(resolved_path);
    added_bookmark->next = NULL;

    if (head == NULL) {
        head = added_bookmark;
    }
    else {
        BookmarkNode *temp = head;
        while (temp->next != NULL) {
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
    FILE *file = fopen(BOOKMARK_FILE, "r");
    if (file == NULL) {
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }
    else {
        char line [MAX_LINE];

        fgets(line, MAX_LINE, file);
        char *name = strtok(line,"\t");
        char *path = strtok(NULL, "\n");
        printf("--------------------------------------------\n");
        printf("|%s\t\t%s     |\n", name, path);
        printf("--------------\t----------------------------\n");


        while (fgets(line, MAX_LINE, file) != NULL) {
            char *bookmark = strtok(line,"\t");
            char *directory = strtok(NULL, "\n");
            printf("%s\t%s\n", bookmark, directory);
        }
        fclose(file);
        return 0;
    }
}

int delete_bookmark(char *name) {
    if (!is_initialized()) {
        printf("Error deleting bookmark from file!");
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, name);

    if (head == NULL) {
        printf("You don't have any bookmarks to delete yet.\n");
        printf("Use bm add <name> <path> to add a bookmark.\n");
        return 1;
    }

    if (target != NULL) {
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
        printf("Error renaming bookmark in file!");
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, old_name);

    if (head == NULL) {
        printf("You don't have any bookmarks to delete yet.\n");
        printf("Use bm add <name> <path> to add a bookmark.\n");
        return 1;
    }

    if (target != NULL) {
        if (!bookmark_exists(head, new_name)) {
            strcpy(target->bookmark.name, new_name);

            printf("Bookmark '%s' has been renamed successfully!\n", old_name);
            printf("'%s' --> /%s\n.", new_name, target->bookmark.path);
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
        printf("Error editing bookmark path in file!");
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    char *tilde_expanded = resolve_tilde(new_path);
    char *resolved_path = realpath(tilde_expanded, NULL);
    if (tilde_expanded != new_path) free(tilde_expanded);

    if (resolved_path == NULL) {
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

    if (head == NULL) {
        printf("You don't have any bookmarks to delete yet.\n");
        printf("Use bm add <name> <path> to add a bookmark.\n");
        return 1;
    }

    if (target != NULL) {
        strcpy(target->bookmark.path, resolved_path);
        printf("Bookmark '%s' has been edited successfully!\n", name);
        printf("'%s' --> %s\n.", name, resolved_path);
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
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }

    BookmarkNode *head = load_bookmarks();
    BookmarkNode *target = find_bookmark(head, name);

    if (head == NULL) {
        printf("You don't have any bookmarks to delete yet.\n");
        printf("Use bm add <name> <path> to add a bookmark.\n");
        return 1;
    }

    if (target) {
        printf("%s\n", target->bookmark.path);
    }
    else {
        printf("'%s' is not a valid bookmark.\n", name);
        free_bookmarks(head);
        return 1;
    }

    free_bookmarks(head);
    return 0;
}

// Helper functions
static void free_bookmarks(BookmarkNode *head) {
    BookmarkNode *temp = head;

    while (temp != NULL) {
        BookmarkNode *next = temp->next;
        free(temp);
        temp = next;
    }
}

static BookmarkNode *load_bookmarks(void) {
    BookmarkNode *head = NULL;

    FILE *file = fopen(BOOKMARK_FILE, "r");
    if (file != NULL) {
        char line [MAX_LINE];

        fgets(line, MAX_LINE, file); // Skip headers

        while (fgets(line, MAX_LINE, file) != NULL) {
            char *name = strtok(line,"\t");
            char *path = strtok(NULL, "\n");

            if (name != NULL && path != NULL){
                BookmarkNode *bookmark_node = malloc(sizeof(BookmarkNode));
                if (bookmark_node == NULL) {
                    printf("Unable to allocate memory for a BookmarkNode.\n");
                    free_bookmarks(head);
                    return NULL;
                }

                trim_trailing_space(name);

                strcpy(bookmark_node->bookmark.name, name);
                strcpy(bookmark_node->bookmark.path, path);
                bookmark_node->next = NULL;
                if (head == NULL) {
                    head = bookmark_node;
                }
                else {
                    BookmarkNode *temp = head;
                    while (temp->next != NULL) {
                        temp = temp->next;
                    }
                    temp->next = bookmark_node;
                }
            }
        }
        fclose(file);
    }
    return head;
}

static int save_bookmarks(BookmarkNode *head) {
    FILE *file = fopen(BOOKMARK_FILE, "w");
    if (file == NULL) {
        printf("Error saving bookmarks to file.\n");
        return 1;
    }

    fprintf(file, "Bookmark Name\tDirectory Path\n");

    BookmarkNode *temp = head;
    while (temp != NULL) {
        fprintf(file, "%-15s\t%s\n", temp->bookmark.name, temp->bookmark.path);
        temp = temp->next;
    }

    fclose(file);
    return 0;
}

static void trim_trailing_space(char *name) {
    for (int i = 0, len = strlen(name); i < len; i++) {
        if (isspace(name[i])) {
            name[i] = '\0';
            break;
        }
    }
}

static BookmarkNode *find_bookmark(BookmarkNode *head, char *name) {
    if (head == NULL) return head;

    BookmarkNode *temp = head;
    while (temp != NULL && strcasecmp(temp->bookmark.name, name) != 0) {
        temp = temp->next;
    }

    return temp;
}

static bool bookmark_exists(BookmarkNode *head, char *name) {
    return find_bookmark(head, name) != NULL;
}

static bool is_initialized(void) {
    FILE *file = fopen(BOOKMARK_FILE, "r");
    if (file == NULL) return false;
    fclose(file);
    return true;
}

static char *resolve_tilde(char *path) {
    if (path[0] == '~') {
        const char *home = getenv("HOME");
        if (!home) {
            printf("HOME environment variable is not set.\n");
            return path;
        }

        const char *path_after_tilde = path + 1;
        char *resolved_path = malloc(strlen(home) + strlen(path_after_tilde) + 1);
        if (!resolved_path) return NULL;

        strcpy(resolved_path, home);

        strcat(resolved_path, path_after_tilde);
        return resolved_path;
    }
    else {
        return path;
    }
}