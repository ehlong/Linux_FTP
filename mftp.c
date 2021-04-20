#include "mftp.h"
#include "client.h"

int main(int argc, char **argv) {
        if (argc != 2) {
                //error handle
        }
        else {
                int s_fd, error, size, check;
                size = snprintf( NULL, 0, "%d", PORT_NUM );
                char *port = malloc(size + 1);
                snprintf(port, size + 1, "%d", PORT_NUM);
                struct addrinfo hints, *actual;         //converting to string from int
                memset(&hints, 0, sizeof(hints));       //init hints to 0
                hints.ai_socktype = SOCK_STREAM;        //set hint to sock stream type
                hints.ai_family = AF_INET;              //set hint to internet
                error = getaddrinfo(argv[1], port, &hints, &actual);
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
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
                while (1){
                        char *input;
                        printf("Enter one of the following commands:\n");
                        printf("exit\ncd <pathname>\nrcd <pathname>\n ls\n"
                               "rls\nget <pathname>\nshow <pathname>\n"
                               "put <pathname>\n");
                        input = getCommand();
                        check = clientProcess(input, s_fd);
                        if (check == 1) {
                                break;
                        }
                }
#pragma clang diagnostic pop
        }
        return 0;
}