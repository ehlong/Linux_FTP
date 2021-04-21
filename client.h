//
// Created by Elliott Long on 4/18/21.
//

#ifndef INC_360FINAL_CLIENT_H
#define INC_360FINAL_CLIENT_H

int clientSocket(int port, char *address);
char *getCommand();
int clientProcess(const char *input, int socketFD, int dataFD);
int toServer(char *command, char *address, int connect, int dataFD);
int dataPort(int socketFD);
void serveTalk(char *command, char *address, int socketFD);
void forker(char **args);

#endif //INC_360FINAL_CLIENT_H
