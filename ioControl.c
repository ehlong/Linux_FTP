#include "mftp.h"

void fdProc(int read_fd, int write_fd) {
    char buff[1];
    int check = 0;
    while ((check = read(read_fd, buff, 1))) {
        if (check == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        }
        if (buff[0] == EOF) {
            break;
        }
        check = write(write_fd, buff, 1);
        if (check == -1) {
            fprintf(stderr, "Error: %s\n", strerror(errno));
        }
    }
}

char *fdReader(int read_fd) {
    write(1, "here\n", 5);
    char *readVal = malloc(1);
    memset(readVal, '\n', 1);
    char buff[1];
    int check = 0;
    check = read(read_fd, buff, 1);
    if (check == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }
    if (buff[0] == 'E') {
        fdProc(read_fd, 1);
    }
    else if (buff[0] == 'A') {
        while ((check = read(read_fd, buff, 1))) {
            if (check == -1) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
            }
            if (buff[0] == '\n') {
                readVal = realloc(readVal, strlen(readVal) + 1);
                strncat(readVal, "\0", 1);
                break;
            }
            strncat(readVal, buff, 1);
            readVal = realloc(readVal, strlen(readVal) + 1);
        }
    }
    else {
        int i = 0;
        readVal = realloc(readVal, 4098);
        readVal[i] = buff[0];
        write(1, buff, 1);
        while ((check = read(read_fd, buff, 1))) {
            i++;
            if (check == -1) {
                break;
            }
            check = write(1, buff, 1);
            if (check == -1) {
                fprintf(stderr, "Error: %s\n", strerror(errno));
            }
            if (buff[0] == '\n') {
                readVal[i] = '\0';
                break;
            }
            readVal[i] = buff[0];
        }
    }
    return readVal;
}

void fdWriter(char *message, int write_fd) {
    int check;
    check = write(write_fd, message, strlen(message));
    if (check == -1) {
        fprintf(stderr, "Error: %s\n", strerror(errno));
    }
}