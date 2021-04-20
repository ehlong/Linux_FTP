#include "mftp.h"


int main(int argc, char **argv) {
    //listen for connections
    //have a queue of 4 simultaneous connections
    //on success, print "Successful Connection of $clientName to stdout
    //fork
    //child reads commands from connection and execute
    int l_fd, c_fd, length, hostEntry, pid, connects;
    char clientName[NI_MAXHOST];            //where client name will be
    l_fd = socket(AF_INET, SOCK_STREAM, 0); //make socket
    if (l_fd < 0) {
        perror("Error: ");
        exit(1);
    }
    if (setsockopt(l_fd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int))
        < 0) {
        perror("Error: ");
        exit(1);
    }                                       //set socket options
    struct sockaddr_in serverAddress, clientAddress;
    //make server/client address obj
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;     //using internet
    serverAddress.sin_port = htons(PORT_NUM);
    //use specified port
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    //listen for ANY network
    //interface
    if (bind(l_fd, (struct sockaddr*) &serverAddress, sizeof(serverAddress))
        < 0) {
        perror("Error: ");
        exit(1);
    }                                       //bind server to port
    if (listen(l_fd, 4) < 0) {
        perror("Error: ");
        exit(1);
    }                                       //mark socket as "listen" socket
    length = sizeof(struct sockaddr_in);
    connects = 0;
    while(1) {
        c_fd = accept(l_fd, (struct sockaddr*) &clientAddress,
                      &length);
        if (c_fd < 0) {                 //actually listen on port
            perror("Error: ");
            exit(1);
        }
        connects += 1;
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
        if (pid > 0) {	                //parent
            printf("%s %i\n", clientName, connects);
            fflush(stdout);         //print client, #of connects
            close(c_fd);            //close connection
        }
        else if (pid < 0) {             //error
            fprintf(stderr, "Error: %s", strerror(errno));
            exit(1);
        }
        else {                          //child
            char buf[19];           //then flush stdout
            memset(buf, '\n', 19);  //make buffer, then set to \n
            time_t current;
            time(&current);         //get time from epoch
            strncpy(buf, ctime(&current), 18);
            write(c_fd, buf, 19);  //format, copy to buf, send
            exit(1);                     //into client
        }
    }
}

