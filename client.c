//
// Created by Elliott Long on 4/18/21.
//
#include "mftp.h"
#include "client.h"

//TODO: Work on forks, test client

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
    char c, *command, *first, *second;
    char *input = NULL;    //holds input
    size_t length = 0;
//    int control = 0;                          //initialize control
    char *tokens = " \f\n\r\t\v";               //all whitespace for strtok
    getline(&input, &length, stdin);            //get input
    first = strtok(input, tokens);              //tokenize
    command = malloc(strlen(first));            //init command
    memset(command, '\n', strlen(command));     //set memory to nl
    strncpy(command, first, strlen(first) - 1); //copy first into command
    second = strtok(NULL, tokens);          //try and get second
    if (second != NULL) {                       //if it exists
        strncat(command, " ", 1);               //change last char to space
        realloc(command, strlen(command) + strlen(second));
        strncat(command, second, strlen(second) - 1);
        strncat(command, "\n", 1);              //realloc, cat 2nd, cat \n
    }
    //   while ((c = getchar()) != EOF) {
    //    if (isspace(c) && control == 0) {
    //        continue;
    //    }
    //    else if (isspace(c) && control == 1) {
    //        strncat(input, " ", 1);
    //        control = 2;
    //        continue;
    //    }
    //    else if (isspace(c) && control == 3) {
    //        break;
    //    }
    //    else {
    //        strncat(input, &c, 1);
    //        if (control == 0) {
    //            control = 1;
    //        }
    //        if (control == 2) {
    //            control = 3;
    //        }
    //    }
    //}
    free(input);
    return command;
}

int clientProcess(const char *input, int socketFD, int dataFD) {
    char *first, *second;           //holds args
    int d_fd = dataFD;              //grab dataFD
    switch (input[0]) {             //check input
        case 'e':
            if (!strcmp(input, "exit")) {
                toServer("Q", " ", socketFD, 0);
                return -1;          //exit, tell client
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'c':
            first = strtok(input, " ");
            if (!strcmp(first, "cd")) {
                second = strtok(NULL, " ");
                //fork cd
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'l':
            first = strtok(input, " ");
            if (!strcmp(first, "ls")) {
                //fork ls-l
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'g':
            first = strtok(input, " ");
            if (!strcmp(first, "get")) {
                second = strtok(NULL, " ");
                d_fd = toServer("G", second, socketFD, dataFD);
                //make file
                //set to 700
                //write to file
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 's':
            first = strtok(input, " ");
            if (!strcmp(first, "show")) {
                second = strtok(NULL, " ");
                d_fd = toServer("G", second, socketFD, dataFD);
                //write to stdout
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'p':
            first = strtok(input, " ");
            if (!strcmp(first, "put")) {
                second = strtok(NULL, " ");
                d_fd = toServer("P", second, socketFD, dataFD);
            }
            break;
        case 'r':
            if (!strcmp(input, "rls")) {
                d_fd = toServer("L", second, socketFD, dataFD);
                //print server ls -l to client
            }
            else {
                first = strtok(input, " ");
                if (!strcmp(first, "rcd")) {
                    second = strtok(NULL, " ");
                    toServer("C", second, socketFD, dataFD);
                    //cd on the server
                }
                else {
                    printf("Invalid input\n");
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
    int d_fd = dataFD;
    switch (command[0]) {
        case 'C':
            write(socketFD, command, 1);
            write(socketFD, "\n", 1);
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
            write(socketFD, "\n", 1);
            break;
        case 'L':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD);
            }
            write(socketFD, command, 1);
            write(socketFD, "\n", 1);
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
            realloc(dataPort, strlen(dataPort) + 1);
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
    write(socketFD, address, strlen(address));
    write(socketFD, "\n", 1);
}