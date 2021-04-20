//
// Created by Elliott Long on 4/18/21.
//
#include "mftp.h"
#include "client.h"

static char *serveAddr = NULL;

int clientSocket(int portNum, char *address) {
    if (serveAddr == NULL) {
        serveAddr = malloc(sizeof(address));
        strcpy(serveAddr, address);
    }
    int s_fd, error, size;
    size = snprintf( NULL, 0, "%d", portNum );
    char *port = malloc(size + 1);
    snprintf(port, size + 1, "%d", portNum);
    struct addrinfo hints, *actual;         //converting to string from int
    memset(&hints, 0, sizeof(hints));       //init hints to 0
    hints.ai_socktype = SOCK_STREAM;        //set hint to sock stream type
    hints.ai_family = AF_INET;              //set hint to internet
    error = getaddrinfo(serveAddr, port, &hints, &actual);
    free(port);                             //^get address info
    if (error != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(error));
        exit(1);
    }
    s_fd = socket(actual->ai_family, actual->ai_socktype, 0);
    //^make socket
    if (connect(s_fd, actual->ai_addr, actual->ai_addrlen) < 0) {
        fprintf(stderr, "Error: %s", strerror(errno));
        exit(1);
    }                                       //^connect to socket
    return s_fd;
}

char *getCommand() {
    char c;
    char *input = malloc(4096 + 5);
    memset(input, '\0', 4096 + 5);
    int control = 0;
    while ((c = getchar()) != EOF) {
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

int clientProcess(const char *input, int socketFD, int dataFD) {
    char *first, *second;
    int d_fd = dataFD;
    switch (input[0]) {
        case 'e':
            if (!strcmp(input, "exit")) {
                toServer("Q", " ", socketFD, 0);
                return -1;
            }
            else {
                //error
            }
            break;
        case 'c':
            first = strtok(input, " ");
            if (!strcmp(first, "cd")) {
                second = strtok(NULL, " ");
                toServer("C", second, socketFD, d_fd);
            }
            break;
        case 'l':
            first = strtok(input, " ");
            if (!strcmp(first, "ls")) {
                toServer("L", " ", socketFD, d_fd);
            }
            break;
        case 'g':
            first = strtok(input, " ");
            if (!strcmp(first, "get")) {
                second = strtok(NULL, " ");
                toServer("G", second, socketFD, dataFD);
            }
            break;
        case 's':
            first = strtok(input, " ");
            if (!strcmp(first, "show")) {
                second = strtok(NULL, " ");
                toServer("S", second, socketFD, dataFD);
            }
            break;
        case 'p':
            first = strtok(input, " ");
            if (!strcmp(first, "put")) {
                second = strtok(NULL, " ");
                toServer("P", second, socketFD, dataFD);
            }
            break;
        case 'r':
            if (!strcmp(input, "rls")) {
                toServer("rls", second, socketFD, dataFD);
            }
            else {
                first = strtok(input, " ");
                if (!strcmp(first, "rcd")) {
                    second = strtok(NULL, " ");
                    toServer("rcd", second, socketFD, dataFD);
                }
            }
            break;
        default:
            printf("Invalid input\n");
            break;
    }
    return d_fd;
}

int toServer(char *command, char *address, int socketFD, int dataFD) {
    char buff[1];
    char *d_port;
    int d_fd = dataFD;
    switch (command[0]) {
        case 'C':
            serveTalk(command, address, socketFD);
            break;
        case 'L':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD);
            }
            break;
        case 'G':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD);
            }
            serveTalk(command, address, socketFD);
            break;
        case 'P':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD);
            }
            serveTalk(command, address, socketFD);
            break;
        case 'Q':
            free(serveAddr);
            write(socketFD, command, 1);
            break;
        case 'S':
            break;
        case 'r':
            break;
        default:
            printf("Error Processing");
    }
    return d_fd;
}

int dataPort(int socketFD) {
    char buff[1] = " ";
    char *dataPort;
    int d_fd = 0;
    dataPort = malloc(2);
    write(socketFD, "D\n", 2);
    read(socketFD, buff, 1);
    if (buff[0] == 'A') {
        read(socketFD, buff, 1);
        strncat(dataPort, buff, 1);
        while (buff[0] != '\n') {
            realloc(dataPort, sizeof(*dataPort) + 1);
            strncat(dataPort, buff, 1);
        }
        strncat(dataPort, "\0", 1);
        d_fd = atoi(dataPort);
        d_fd = clientSocket(d_fd, serveAddr);
    }
    free(dataPort);
    return d_fd;
}

void serveTalk(char *command, char *address, int socketFD) {
    write(socketFD, command, 1);
    write(socketFD, address, sizeof(*address));
    write(socketFD, "\n", 1);
}