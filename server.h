//
// Created by Elliott Long on 4/21/21.
//

#ifndef INC_360FINAL_SERVER_H
#define INC_360FINAL_SERVER_H

int serverSocket(int port, int maxCon);
void serveLoop(int l_fd);
int serverProcess(char *input, int c_fd, int data_fd);
void ls(int d_fd);
int quit(int c_fd);
int cd(int c_fd, char *input);
int lsl(int c_fd, int d_fd);
int get(int c_fd, int d_fd, char *input);
int put(int c_fd, int d_fd, char *input);
int dataFD(int c_fd);
void errorFormat(int c_fd);
int getSock(int l_fd);

#endif //INC_360FINAL_SERVER_H
