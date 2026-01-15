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
    FILE *file = fopen("bookmarks.tsv", "r");
    if (file == NULL) {
        file = fopen("bookmarks.tsv", "w");
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