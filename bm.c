#include <stdio.h>
#include <string.h>

void print_helper(void) {
    printf("Usage: bm <command> [<args>]\n");
    printf("Commands\n");
    printf("  init                   Initialize bookmark system\n");
    printf("  add <name> <path>      Add a bookmark\n");
    printf("  list                   List all bookmarks\n");
    printf("  go <name>              Print path of a bookmark\n");
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        print_helper();
    }

    char *command = argv[1];

    if (strcmp(command, "init") == 0) {

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