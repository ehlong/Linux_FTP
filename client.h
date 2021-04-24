#ifndef INC_360FINAL_CLIENT_H
#define INC_360FINAL_CLIENT_H

int clientSocket(int port, char *address);
char *getCommand();
int clientProcess(const char *input, int socketFD, int dataFD, char *address);
int toServer(char *command, char *address, int connect, int dataFD, char *servAddr);
int dataPort(int socketFD, char *address);
void serveTalk(char *command, char *address, int socketFD);
void forker(char **args);
void more(int d_fd);

#endif //INC_360FINAL_CLIENT_H
