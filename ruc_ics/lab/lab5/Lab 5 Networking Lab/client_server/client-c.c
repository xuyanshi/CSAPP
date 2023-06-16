/*****************************************************************************
 * client-c.c
 * Name:
 * NetId:
 *****************************************************************************/

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SEND_BUFFER_SIZE 2048

/* TODO: client()
 * Open socket and send message from stdin.
 * Return 0 on success, non-zero on failure
 */
int client(char *server_ip, char *server_port) {
  // Socket id
  int clientSocket, ret;

  // Client socket structure
  struct sockaddr_in serverAddr, cliAddr;

  // char array to store incoming message
  char buffer[1024];
  int n;
  // Creating socket id
  clientSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (clientSocket < 0) {
    printf("Error in connection.\n");
    exit(1);
  }
  printf("Client Socket is created.\n");

  // Initializing socket structure with NULL
  memset(&cliAddr, '\0', sizeof(cliAddr));

  // Initializing buffer array with NULL
  memset(buffer, '\0', sizeof(buffer));

  // Assigning port number and IP address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(server_port);

  // 127.0.0.1 is Loopback IP
  serverAddr.sin_addr.s_addr = inet_addr(server_ip);

  // connect() to connect to the server
  ret =
      connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  if (ret < 0) {
    printf("Error in connection.\n");
    exit(1);
  }

  printf("Connected to Server.\n");
  recv(clientSocket, buffer, sizeof(buffer), 0);
  printf("%s", buffer);
  while (1) {

    // // recv() receives the message
    // // from server and stores in buffer
    // if (recv(clientSocket, buffer, 1024, 0)
    //     < 0) {
    //     printf("Error in receiving data.\n");
    // }
    // else {
    //     printf("Server: %s\n", buffer);
    //     bzero(buffer, sizeof(buffer));
    // }

    // recv(clientSocket, buffer, sizeof(buffer),0);
    // recv(clientSocket, buffer, sizeof(buffer),0);
    // printf("From Server : %s", buffer);
    // bzero(buffer, sizeof(buffer));
    bzero(buffer, SEND_BUFFER_SIZE);
    printf("Enter the msg: ");
    n = 0;
    scanf("%s", buffer);
    // while ((buffer[n++] = getchar()) != '\n');
    printf("The length of str is: ");
    printf("%d\n", strlen(buffer));

    send(clientSocket, buffer, strlen(buffer), 0);

    printf("Client Exit...\n");
    break;
    // if ((strncmp(buffer, "exit", 4)) ==
    //     0) { //|| (strncmp(buffer, "all exit", 8)) == 0) {
    //   printf("Client Exit...\n");
    //   break;
    // }
  }
  close(ret);
  return 0;
}

/*
 * main()
 * Parse command-line arguments and call client function
 */
int main(int argc, char **argv) {
  char *server_ip;
  char *server_port;

  if (argc != 3) {
    fprintf(stderr,
            "Usage: ./client-c [server IP] [server port] < [message]\n");
    exit(EXIT_FAILURE);
  }

  server_ip = argv[1];
  server_port = argv[2];
  return client(server_ip, server_port);
}
