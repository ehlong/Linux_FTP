#include "mftp.h"
#include "client.h"

int main(int argc, char **argv) {
    if (argc != 2) {
            //error handle
    }
    else {
        int s_fd, d_fd;
        s_fd = clientSocket(PORT_NUM, argv[1]);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
        d_fd = 0;
        while (1){
            char *input;
            printf("Enter one of the following commands:\n");
            printf("exit\ncd <pathname>\nrcd <pathname>\n ls\n"
                   "rls\nget <pathname>\nshow <pathname>\n"
                   "put <pathname>\n");
            input = getCommand();
            d_fd = clientProcess(input, s_fd, d_fd);
            if (d_fd == -1) {
                break;
            }
        }
#pragma clang diagnostic pop
    }
    return 0;
}