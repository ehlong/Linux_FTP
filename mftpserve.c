#include "server.h"


int main(int argc, char **argv) {
    //listen for connections
    //have a queue of 4 simultaneous connections
    //on success, print "Successful Connection of $clientName to stdout
    //fork
    //child reads commands from connection and execute
    int l_fd = serverSocket(-1, 4);          //make server socket
    serveLoop(l_fd);                    //run the socket
    return 0;
}

