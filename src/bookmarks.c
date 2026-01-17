#include "bookmarks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int add_bookmark(char *name, char *path) {
    FILE *file = fopen(BOOKMARK_FILE, "a");

    if (file == NULL) {
        printf("Error adding bookmark to file!");
        printf("You haven't initialized the bookmark system yet.\nRun 'bm init' first to initialize the bookmark system!\n");
        return 1;
    }
    else {
        if (strlen(name) > MAX_NAME) {
            printf("The bookmark name is too long. Try again.\n");
            return 1;
        }
        else if (strlen(path) > MAX_PATH) {
            printf("The directory path is too long. Try again.\n");
            return 1;
        }

        fprintf(file, "%-15s\t%s\n", name, path);
        fclose(file);

        printf("Bookmark added successfully!\n");
        return 0;
    }
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