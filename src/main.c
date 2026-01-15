#include <stdio.h>
#include <string.h>
#include "bookmarks.h"

int main(int argc, char *argv[]) {

    if (argc < 2) {
        print_helper();
    }

    char *command = argv[1];

    if (strcmp(command, "init") == 0) {
        init_bookmark();
    }
    else if (strcmp(command, "add") == 0) {

    }
    else if (strcmp(command, "list") == 0) {

    }
    else if (strcmp(command, "go") == 0) {

    }
    else {
        printf("Invalid command!\n");
        print_helper();
    }
    return 0;
}