#include "mftp.h"

void fdProc(int read_fd, int write_fd) {
    char buff[1];
    int check = 0;
    while ((check = read(read_fd, buff, 1))) {
        if (check == -1) {
            printf("Error: Read error\n");
        }
        if (buff[0] == EOF) {
            break;
        }
        check = write(write_fd, buff, 1);
        if (check == -1) {
//            printf("Error: Write error\n");
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
        perror("Read Error:");
    }
    if (buff[0] == 'E') {
        fdProc(read_fd, 1);
    }
    else if (buff[0] == 'A') {
        while ((check = read(read_fd, buff, 1))) {
            if (check == -1) {
                //r error
            }
            if (buff[0] == '\n') {
                realloc(readVal, strlen(readVal) + 1);
                strncat(readVal, "\0", 1);
                break;
            }
            strncat(readVal, buff, 1);
            realloc(readVal, strlen(readVal) + 1);
        }
    }
    else if (buff[0] == 'Q' || buff[0] == 'D' || buff[0] == 'C'
                || buff[0] == 'L' || buff[0] == 'G' || buff[0] == 'P'){
        strncpy(readVal, buff, 1);
        realloc(readVal, strlen(readVal) + 1);
        write(1, buff, 1);
        while ((check = read(read_fd, buff, 1))) {
            if (check == -1) {
                break;
            }
            write(1, buff, 1);
            if (buff[0] == '\n') {
                realloc(readVal, strlen(readVal) + 1);
                strncat(readVal, "\0", 1);
                break;
            }
            strncat(readVal, buff, 1);
            realloc(readVal, strlen(readVal) + 1);
        }
    }
    return readVal;
}

void fdWriter(char *message, int write_fd) {
    int check;
    check = write(write_fd, message, strlen(message));
    if (check == -1) {
        printf("Error: Write failed\n");
    }
}