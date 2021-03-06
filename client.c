#include "mftp.h"
#include "client.h"


int clientSocket(int portNum, char *address) {
    int s_fd, error, size;
    size = snprintf( NULL, 0, "%d", portNum );
    char *port = malloc(size + 1);
    snprintf(port, size + 1, "%d", portNum);
    struct addrinfo hints, *actual;         //converting to string from int
    memset(&hints, 0, sizeof(hints));       //init hints to 0
    hints.ai_socktype = SOCK_STREAM;        //set hint to sock stream type
    hints.ai_family = AF_INET;              //set hint to internet
    error = getaddrinfo(address, port, &hints, &actual);
    free(port);                             //^get address info
    if (error != 0) {
        fprintf(stderr, "Error: %s\n", gai_strerror(error));
        exit(1);
    }
    s_fd = socket(actual->ai_family, actual->ai_socktype, 0);
    //^make socket
    if (connect(s_fd, actual->ai_addr, actual->ai_addrlen) < 0) {
        fprintf(stderr, "Connect Error: %s", strerror(errno));
        exit(1);
    }                                       //^connect to socket
    return s_fd;
}

char *getCommand() {
    char *command, *first, *second;
    char *input = NULL;    //holds input
    size_t length = 0;
    char *tokens = " \f\n\r\t\v";               //all whitespace for strtok
    getline(&input, &length, stdin);            //get input
    first = strtok(input, tokens);              //tokenize
    command = malloc(strlen(first));            //init command
    memset(command, '\0', strlen(command));     //set memory to nl
    strncpy(command, first, strlen(first)); //copy first into command
    second = strtok(NULL, tokens);          //try and get second
    if (second != NULL) {                       //if it exists
        strncat(command, " ", 1);               //change last char to space
        realloc(command, strlen(command) + strlen(second));
        strncat(command, second, strlen(second));
        strncat(command, "\0", 1);              //realloc, cat 2nd, cat \n
    }
    free(input);
    printf("\n");
    return command;
}

int clientProcess(const char *input, int socketFD, int dataFD, char* address) {
    char buff[1], *first, *second, **local;  //holds args
    int d_fd = dataFD;              //grab dataFD
    switch (input[0]) {             //check input
        case 'e':
            if (!strcmp(input, "exit")) {
                toServer("Q", " ", socketFD, 0, address);
                return -1;          //exit, tell server
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'c':
            first = strtok(input, " "); //get first arg
            if (!strcmp(first, "cd")) {     //if cd
                second = strtok(NULL, " ");
                if (second == NULL) {
                    fprintf(stderr, "CD Error\n");//if error
                    break;
                }
                if (access(second, X_OK) == -1) {   //if no execute access
                    fprintf(stderr, "CD Error: %s\n", strerror(errno));//if error
                }
                if (chdir(second) == -1) {
                    fprintf(stderr, "CD Error: %s\n", strerror(errno));//if error
                }
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'l':
            first = strtok(input, " ");
            if (!strcmp(first, "ls")) {
                local = malloc(sizeof(char*) * 3);
                local[0] = malloc(strlen(first));
                strcpy(local[0], first);
                local[1] = malloc(3);
                local[2] = NULL;
                strcpy(local[1], "-l");
                forker(local);              //fork, do an ls exec
                free(local[0]);
                free(local[1]);
                free(local);
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'g':
            first = strtok(input, " ");
            if (!strcmp(first, "get")) {
                second = strtok(NULL, " ");
                if (second == NULL) {
                    fprintf(stderr, "Get Error\n");//if error
                    break;
                }
                char *name = second;
                char *filename = strrchr(name, '/');
                if (filename == NULL) {
                    filename = name;
                }
                else {
                    filename++;
                }
                if (access(filename, F_OK) == -1) {
                    int file = open(filename, O_WRONLY | O_CREAT, S_IRWXU);
                    if (file == -1) {
                        fprintf(stderr, "Get Error: %s\n", strerror(errno));//if error
                    }
                    else {
                        d_fd = toServer("G", second, socketFD, dataFD, address);
                        read(socketFD, buff, 1);
                        if (buff[0] == 'A') {
                            read(socketFD, buff, 1);
                        }
                        fdProc(d_fd, file);
                        close(d_fd);
                    }
                }
                else {
                    fprintf(stderr, "File already exists\n");//if error
                }
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
                if (second == NULL) {
                    fprintf(stderr, "Show Error\n");//if error
                    break;
                }
                d_fd = toServer("G", second, socketFD, dataFD, address);
                read(socketFD, buff, 1);
                if (buff[0] == 'A') {
                    read(socketFD, buff, 1);
                }
                more(d_fd);            //write from d_fd to stdout
                close(d_fd);
            }
            else {
                printf("Invalid input\n");
            }
            break;
        case 'p':
            first = strtok(input, " ");
            if (!strcmp(first, "put")) {
                second = strtok(NULL, " ");
                if (second == NULL) {
                    fprintf(stderr, "Put Error\n");//if error
                    break;
                }
                char *name = second;
                char *filename = strrchr(name, '/');
                if (filename == NULL) {
                    filename = name;
                }
                else {
                    filename++;
                }
                int file = open(filename, O_RDONLY);
                if (file == -1) {
                    fprintf(stderr, "Put Error: %s\n", strerror(errno));//if error
                }
                else {
                    d_fd = toServer("P", second, socketFD, dataFD, address);
                    read(socketFD, buff, 1);
                    if (buff[0] == 'A') {
                        read(socketFD, buff, 1);
                    }
                    fdProc(file, d_fd);
                    close(d_fd);
                }
            }
            break;
        case 'r':
            if (!strcmp(input, "rls")) {
                d_fd = toServer("L", NULL, socketFD, dataFD, address);
                read(socketFD, buff, 1);
                if (buff[0] == 'A') {
                    read(socketFD, buff, 1);
                    more(d_fd);        //write from d_fd to stdout
                }
                close(d_fd);
            }
            else {
                first = strtok(input, " ");
                if (!strcmp(first, "rcd")) {
                    second = strtok(NULL, " ");
                    if (second == NULL) {
                        fprintf(stderr, "rcd Error\n");//if error
                        break;
                    }
                    toServer("C", second, socketFD, dataFD, address);
                    read(socketFD, buff, 1);
                    if (buff[0] == 'A') {
                        printf("rcd worked\n");
                        read(socketFD, buff, 1);
                    }
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


int toServer(char *command, char *address, int socketFD, int dataFD,
             char *servAddr) {
    int d_fd = dataFD;
    switch (command[0]) {
        case 'C':
            write(socketFD, command, 1);
            write(socketFD, address, strlen(address));
            write(socketFD, "\n", 1);
            break;
        case 'G':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD, servAddr);
            }
            serveTalk(command, address, socketFD);
            break;
        case 'P':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD, servAddr);
            }
            serveTalk(command, address, socketFD);
            break;
        case 'Q':
            write(socketFD, command, 1);
            write(socketFD, "\n", 1);
            break;
        case 'L':
            if (d_fd == 0) {
                d_fd = dataPort(socketFD, servAddr);
            }
            else {
                fprintf(stderr, "No dataport error\n");
            }
            write(socketFD, command, 1);
            write(socketFD, "\n", 1);
            break;
        default:
            printf("Error Processing");
    }
    return d_fd;
}

int dataPort(int socketFD, char *address) {
    char buff[1] = " ";
    char dataPort[6];
    int d_fd = 0;
    write(socketFD, "D\n", 2);
    read(socketFD, buff, 1);
    if (buff[0] == 'A') {
        int i = 0;
        read(socketFD, buff, 1);
        while (buff[0] != '\n') {
            write(1, buff, 1);
            dataPort[i] = buff[0];
            i++;
            read(socketFD, buff, 1);
        }
        dataPort[5] = '\0';
        d_fd = atoi(dataPort);
        d_fd = clientSocket(d_fd, address);
    }
    return d_fd;
}

void serveTalk(char *command, char *address, int socketFD) {
    write(socketFD, command, 1);
    write(socketFD, address, strlen(address));
    write(socketFD, "\n", 1);
}

void forker(char **args) {
    int c_id;       //holds child id
    c_id = fork();
    if (c_id < 0) { //fork error
        fprintf(stderr, "Fork Error: %s\n", strerror(errno));
        exit(-errno);
    }
    else if (c_id) {    //parent
        wait(NULL);    //wait for child
    }
    else {              //child
        execlp(args[0], args[0], args[1], args[2]);
        fprintf(stderr, "EXECVP Error: %s\n", strerror(errno)); //if error
    }
}

void more(int d_fd) {
    int test;
    test = fork();
    if (test < 0) {             //error
        fprintf(stderr, "Error: %s", strerror(errno));
        exit(1);
    }
    else if (test > 0) {	                //parent
        wait(NULL);                 //wait for child
    }
    else {                          //child
        dup2(d_fd, 0);
        execlp("more", "more", "-20", NULL);
        fprintf(stderr, "Error: %s\n", strerror(errno)); //if error
    }
}