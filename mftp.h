#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>
#define PORT_NUM 49999
#ifndef INC_360FINAL_MFTP_H
#define INC_360FINAL_MFTP_H

void fdProc(int read_fd, int write_fd);
char *fdReader(int read_fd);
void fdWriter(char *message, int write_fd);

#endif //INC_360FINAL_MFTP_H
