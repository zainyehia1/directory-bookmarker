#include <stdio.h>
#include <string.h>
#include "bookmarks.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        print_helper();
        return 1;
    }

    char *command = argv[1];

    if (strcmp(command, "init") == 0) {
        if (argc == 2) {
            init_bookmark();
        }
        else {
            printf("'init' usage: bm init\n");
            return 1;
        }
    }
    else if (strcmp(command, "add") == 0) {
        if (argc == 4) {
           add_bookmark(argv[2], argv[3]);
        }
        else {
            printf("'add' usage: bm add <name> <path>\n");
            return 1;
        }
    }
    else if (strcmp(command, "list") == 0) {
        if (argc == 2) {
            list_bookmarks();
        }
        else {
            printf("'list' usage: bm list\n");
            return 1;
        }
    }
    else if (strcmp(command, "delete") == 0) {
        if (argc == 3) {
            delete_bookmark(argv[2]);
        }
        else {
            printf("'delete' usage: bm delete <name>\n");
            return 1;
        }
    }
    else if (strcmp(command, "rename") == 0) {
        if (argc == 4) {
            rename_bookmark(argv[2], argv[3]);
        }
        else {
            printf("'rename' usage: bm rename <current_name> <new_name>\n");
            return 1;
        }
    }
    else if (strcmp(command, "edit") == 0) {
        if (argc == 4) {
            edit_path(argv[2], argv[3]);
        }
        else {
            printf("'edit' usage: bm edit <name> <new_path>\n");
            return 1;
        }
    }
    // else if (strcmp(command, "go") == 0) {
    //
    // }
    else if (strcmp(command, "help") == 0) {
        print_helper();
    }
    else {
        printf("Invalid command!\n");
        print_helper();
        return 1;
    }
    return 0;
}