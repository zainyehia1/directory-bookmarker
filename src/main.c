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
        init_bookmark();
    }
    else if (strcmp(command, "add") == 0) {
        if (argc != 4) {
            printf("Add usage: add <name> <path>      Add a bookmark\n");
            return 1;
        }
        else {
            add_bookmark(argv[2], argv[3]);
        }
    }
    // else if (strcmp(command, "list") == 0) {
    //
    // }
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