/*****************************************************************************
 * server-c.c
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

#define QUEUE_LENGTH 10
#define RECV_BUFFER_SIZE 2048

/* TODO: server()
 * Open socket and wait for client to connect
 * Print received message to stdout
 * Return 0 on success, non-zero on failure
 */
int server(char *server_port) {
  // Server socket id
  int sockfd, ret;

  // Server socket address structures
  struct sockaddr_in serverAddr;

  // Client socket id
  int clientSocket;

  // Client socket address structures
  struct sockaddr_in cliAddr;

  // Stores byte size of server socket address
  socklen_t addr_size;

  // Child process id
  pid_t childpid;

  // Creates a TCP socket id from IPV4 family
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Error handling if socket id is not valid
  if (sockfd < 0) {
    printf("Error in connection.\n");
    exit(1);
  }

  printf("Server Socket is created.\n");

  // Initializing address structure with NULL
  memset(&serverAddr, '\0', sizeof(serverAddr));

  // Assign port number and IP address
  // to the socket created
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(server_port);

  // 127.0.0.1 is a loopback address
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Binding the socket id with
  // the socket structure
  ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

  // Error handling
  if (ret < 0) {
    printf("Error in binding.\n");
    exit(1);
  }

  // Listening for connections (upto 10)
  if (listen(sockfd, 10) == 0) {
    printf("Server is listening...\n\n");
  }

  int cnt = 0;

  while (1) {

    // Accept clients and
    // store their information in cliAddr
    clientSocket = accept(sockfd, (struct sockaddr *)&cliAddr, &addr_size);

    // Error handling
    if (clientSocket < 0) {
      exit(1);
    }

    // Displaying information of
    // connected client
    printf("Connection accepted from %s:%d\n", inet_ntoa(cliAddr.sin_addr),
           ntohs(cliAddr.sin_port));

    // Print number of clients
    // connected till now
    printf("Clients connected: %d\n\n", ++cnt);

    // Creates a child process
    if ((childpid = fork()) == 0) {

      // Closing the server socket id

      // Send a confirmation message
      // to the client
      FILE *fp;
      char hello[64];
      if ((fp = fopen("hello.txt", "r")) == NULL) {
        printf("can not open file\n");
        exit(1);
      }
      fgets(hello, 64, fp);
      fclose(fp);
      send(clientSocket, hello, strlen(hello), 0);

      char buffer[RECV_BUFFER_SIZE];
      int n;

      // infinite loop for chat
      for (;;) {
        // read the message from client and copy it in buffer

        while (recv(clientSocket, buffer, sizeof(buffer), 0) > 0) {
          // print buffer which contains the client contents
          printf("From client: %s\n", buffer);

          // if msg contains "exit" then server exit and chat ended.
          if (strncmp("exit", buffer, 4) == 0) {
            printf("Client Exit...\n");
            close(clientSocket);
            break;
          }
          // } else if (strncmp("all exit", buffer, 8) == 0) {
          //     printf("Server Exit Completely...\n");
          //     close(clientSocket);
          //     goto quit;
          else {
            printf("The length of str is: ");
            printf("%d\n", strlen(buffer));
          }
          bzero(buffer, RECV_BUFFER_SIZE);

          // n = 0;
          // copy server message in the buffer
          // printf("(Enter the msg:)");
          // while ((buffer[n++] = getchar()) != '\n')
          //     ;

          // and send that buffer to client
          // send(clientSocket, buffer, sizeof(buffer),0);
        }
      }
    }
  }

  // Close the client socket id
  close(sockfd);
  return 0;
}

/*
 * main():
 * Parse command-line arguments and call server function
 */
int main(int argc, char **argv) {
  char *server_port;

  if (argc != 2) {
    fprintf(stderr, "Usage: ./server-c [server port]\n");
    exit(EXIT_FAILURE);
  }

  server_port = argv[1];
  return server(server_port);
}
