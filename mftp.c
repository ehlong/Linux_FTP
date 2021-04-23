#include "mftp.h"
#include "client.h"

int main(int argc, char **argv) {
    if (argc != 2) {        //if too many args
            printf("Error: Too many arguments. Exiting. \n");
            return 1;
    }
    else {
        int s_fd, d_fd;     //socket FDs
        s_fd = clientSocket(PORT_NUM, argv[1]); //get control socket
        d_fd = 0;           //default dataFD to 0
        while (1){          //infinite loop
            char *input;    //holds command
            printf("Enter one of the following commands:\n");
            printf("exit\ncd <pathname>\nrcd <pathname>\n ls\n"
                   "rls\nget <pathname>\nshow <pathname>\n"
                   "put <pathname>\n");
            input = getCommand();   //process input
            d_fd = clientProcess(input, s_fd, d_fd);    //process command,
            free(input);        //free input              get dataFD
            if (d_fd == -1) {   //if quit
                break;          //exit
            }
        }
    }
    return 0;
}