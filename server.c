//
// Created by Elliott Long on 4/21/21.
//

#include "server.h"
#include "mftp.h"

int serverSocket(int port, int maxCon) {
    int portUsed;
    if (port == -1) {
        portUsed = PORT_NUM;
    }
    else {
        portUsed = port;
    }
    int l_fd;
    l_fd = socket(AF_INET, SOCK_STREAM, 0); //make socket
    if (l_fd < 0) {
        perror("Error");
        exit(1);
    }
    if (setsockopt(l_fd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int))
        < 0) {
        perror("Error");
        exit(1);
    }                                       //set socket options
    struct sockaddr_in serverAddress;
    //make server/client address obj
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;     //using internet
    serverAddress.sin_port = htons(portUsed);
    //use specified port
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //listen for ANY network
    //interface
    if (bind(l_fd, (struct sockaddr*) &serverAddress, sizeof(serverAddress))
        < 0) {
        perror("Error");
        exit(1);
    }                                       //bind server to port
    if (listen(l_fd, maxCon) < 0) {
        perror("Error");
        exit(1);
    }                                       //mark socket as "listen" socket
    return l_fd;
}

void serveLoop(int l_fd) {
    int c_fd, length, hostEntry, pid, flag;
    struct sockaddr_in clientAddress;
    char *instructions;
    char clientName[NI_MAXHOST];            //where client name will be
    length = sizeof(struct sockaddr_in);
    while(1) {
        c_fd = accept(l_fd, (struct sockaddr*) &clientAddress,
                      &length);
        if (c_fd < 0) {                 //actually listen on port
            perror("Error");
            exit(1);
        }
        hostEntry = getnameinfo(
            (struct sockaddr *) &clientAddress,
            sizeof(clientAddress), clientName,
            sizeof(clientName), NULL, 0,
            NI_NUMERICSERV);
        if (hostEntry != 0) {   //get client name
            fprintf(stderr,
                    "Error: %s\n", gai_strerror(hostEntry));
            exit(1);
        }
        pid = fork();                   //fork
        if (pid < 0) {             //error
            fprintf(stderr, "Error: %s", strerror(errno));
            exit(1);
        }
        else if (pid > 0) {	                //parent
            printf("Successful Connection of %s\n", clientName);
            fflush(stdout);         //print client
            close(c_fd);
            while(waitpid(0, NULL, WNOHANG) > 0);   //clean up zombies
        }
        else {                          //child
            while(1) {
                instructions = fdReader(l_fd);
                flag = serverProcess(instructions, l_fd, 0);
                if (flag > 0) {
                    free(instructions);
                    instructions = fdReader(l_fd);
                    serverProcess(instructions, l_fd, flag);
                }
                else if (flag < 0) {
                    free(instructions);
                    break;
                }
                free(instructions);
            }
        }
    }
}

int serverProcess(char *input, int c_fd, int data_fd) {
    int d_fd = data_fd;
    switch (input[0]) {             //check input
        case 'Q':
            d_fd = quit(c_fd);
            break;
        case 'D':
            d_fd = dataFD(c_fd);
            break;
        case 'C':
            //cd pathname
            cd(c_fd, input);
            break;
        case 'L':
            //dfd
            //ls -l
            lsl(c_fd, d_fd);
            close(d_fd);
            break;
        case 'G':
            //dfd
            //get pathname
            get(c_fd, d_fd, input);
            close(d_fd);
            break;
        case 'P':
            //dfd
            //put file
            put(c_fd, d_fd, input);
            close(d_fd);
            break;
        default:
            printf("Invalid input\n");
            break;
    }
    return d_fd;
}

void ls(int d_fd) {
    int test;
    if (test != 0) {		//if pipe fails
        printf("%s\n", strerror(errno));
    }
    test = fork();
    if (test < 0) {             //error
        fprintf(stderr, "Error: %s", strerror(errno));
        exit(1);
    }
    else if (test > 0) {	                //parent
        wait(NULL);
        //acknowledge?
    }
    else {                          //child
        close(read);
        dup2(d_fd, 1);
        execlp("ls", "ls", "-l", "-a", NULL);
        fprintf(stderr, "Error: %s\n", strerror(errno)); //if error
    }
}

int quit(int c_fd) {
    fdWriter("A\n", c_fd);          //acknowledge
    return -1;                              //quit value
}
int cd(int c_fd, char *input) {
    char *first;
    int test;
    first = malloc(strlen(input) - 1);
    memset(first, '\0', strlen(first));
    memcpy(first, &input[1], strlen(input) - 2);
    test = chdir(first);
    if (test == -1) {
        errorFormat(c_fd);
    }
    else {
        fdWriter("A\n", c_fd);
    }
}

int lsl(int c_fd, int d_fd) {
    if (d_fd <= 0) {
        fdWriter("EError: Data connection not established\n", c_fd);
    }
    else {
        int in = dup(0);
        int out = dup(1);
        ls(d_fd);
        dup2(in, 0);
        dup2(out, 1);
        fdWriter("A\n", c_fd);
    }
}

int get(int c_fd, int d_fd, char *input) {
    char *first;
    int test;
    first = malloc(strlen(input) - 1);
    memset(first, '\0', strlen(first));
    memcpy(first, &input[1], strlen(input) - 2);
    test = access(first, R_OK | F_OK);
    if (test == -1) {
        errorFormat(c_fd);
    }
    int file = open(first, O_RDONLY);
    if (file < 0) {
        errorFormat(c_fd);
    }
    free(first);
    fdProc(file, d_fd);
}

int put(int c_fd, int d_fd, char *input) {
    char *first;
    int test;
    first = malloc(strlen(input) - 1);
    memset(first, '\0', strlen(first));
    memcpy(first, &input[1], strlen(input) - 2);
    char *last = strrchr(first, '/');
    if (last != NULL) {
        //use last for put
        test = access(last, F_OK);
        if (test == -1) {               //file does NOT exist
            last++;
            int file = open(last, O_WRONLY | O_CREAT, S_IRWXU);
            fdProc(d_fd, file);
            close(file);
        }
        else {
            fdWriter("EError: File already exists\n", c_fd);
        }
    }
    else {
        //use first for put
        test = access(first, F_OK);
        if (test == -1) {               //file does NOT exist
            first++;
            int file = open(first, O_WRONLY | O_CREAT, S_IRWXU);
            fdProc(d_fd, file);
            close(file);
        }
        else {
            fdWriter("EError: File already exists\n", c_fd);
        }
    }
}

int dataFD(int c_fd) {
    int d_fd, size;
    d_fd = serverSocket(0, 1);  //make data connection
    size = snprintf( NULL, 0, "%d", d_fd );
    char *port = malloc(size + 2);
    snprintf(port, size + 2, "A%d", d_fd);  //write fd to port
    fdWriter(port, c_fd);                   //send to client
    free(port);
    return d_fd;
}

void errorFormat(int c_fd) {
    int len = strlen(strerror(errno));
    char *estring = malloc(len);
    memset(estring, '\n', len);
    strncpy(estring, strerror(errno), len - 1);
    fdWriter("E", c_fd);
    fdWriter(estring, c_fd);
}