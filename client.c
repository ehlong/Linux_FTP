//
// Created by Elliott Long on 4/18/21.
//
#include "mftp.h"
#include "client.h"

char *getCommand() {
        char c;
        char *input = malloc(4096 + 5);
        memset(input, '\0', 4096 + 5);
        int control = 0;
        while (c = getchar()) {
                if (isspace(c) && control == 0) {
                        continue;
                }
                else if (isspace(c) && control == 1) {
                        strncat(input, " ", 1);
                        control = 2;
                        continue;
                }
                else if (isspace(c) && control == 3) {
                        break;
                }
                else {
                        strncat(input, &c, 1);
                        if (control == 0) {
                                control = 1;
                        }
                        if (control == 2) {
                                control = 3;
                        }
                }
        }
        return input;
}

int clientProcess(const char *input, int socketFD) {
        char *first, *second;
        switch (input[0]) {
                case 'e':
                        if (strcmp(input, "exit")) {
                                return 1;
                        }
                        else {
                                //error
                        }
                        break;
                case 'c':
                        first = strtok(input, " ");
                        if (!strcmp(first, "cd")) {
                                second = strtok(NULL, " ");
                                //cd to second
                        }
                        break;
                case 'l':
                        first = strtok(input, " ");
                        if (!strcmp(first, "ls")) {
                                //ls
                        }
                        break;
                case 'g':
                        first = strtok(input, " ");
                        if (!strcmp(first, "get")) {
                                second = strtok(NULL, " ");
                                //get second from server, store in CWD
                        }
                        break;
                case 's':
                        first = strtok(input, " ");
                        if (!strcmp(first, "show")) {
                                second = strtok(NULL, " ");
                                //show contents of second
                        }
                        break;
                case 'p':
                        first = strtok(input, " ");
                        if (!strcmp(first, "put")) {
                                second = strtok(NULL, " ");
                                //put contents of second in cwd
                        }
                        break;
                case 'r':
                        if (!strcmp(input, "rls")) {
                                //ls the server
                        }
                        else {
                                first = strtok(input, " ");
                                if (!strcmp(first, "rcd")) {
                                        second = strtok(NULL, " ");
                                        //cd server to second
                                }
                        }
                        break;
                default:
                        printf("Invalid input\n");
                        break;
        }
        return 0;
}