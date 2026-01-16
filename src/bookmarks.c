#include "bookmarks.h"
#include <stdio.h>

void print_helper(void) {
    printf("Usage: bm <command> [<args>]\n");
    printf("Commands\n");
    printf("  init                   Initialize bookmark system\n");
    printf("  add <name> <path>      Add a bookmark\n");
    printf("  delete <name>          Delete a bookmark\n");
    printf("  list                   List all bookmarks\n");
    printf("  go <name>              Print path of a bookmark\n");
    printf("  help                   Print this message\n");
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

int add_bookmark(char *bookmark, char *directory) {
    FILE *file = fopen(BOOKMARK_FILE, "a");

    if (file != NULL) {
        fprintf(file, "%s\t%s", bookmark, directory);
        fclose(file);

        printf("Bookmark added successfully!\n");
        return 0;
    }
    else {
        printf("Error adding bookmark to file!");
        return 1;
    }
}