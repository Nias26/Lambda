#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

void *receive_messages(void *sock_ptr) {
  int sock = *(int *)sock_ptr;
  char buffer[BUFFER_SIZE];
  int len;

  memset(buffer, '0', BUFFER_SIZE);

  while ((len = recv(sock, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[len] = '\0';
    printf("%s", buffer);
  }
  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 3){
    printf("Too few arguments\nUsage: <address> <port>\n");
    return 1;
  }

  char* address = argv[1];
  int port = htons(atoi(argv[2]));

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  inet_pton(AF_INET, address, &server_addr.sin_addr);

  if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 ){
    perror("Failed connecting to sevrer");
    close(sock);
    return 1;
  }

  pthread_t tid;
  pthread_create(&tid, NULL, receive_messages, &sock);

  char message[1024];
  while (fgets(message, 1024, stdin)) {
    send(sock, message, strlen(message), 0);
  }

  close(sock);
  return 0;
}
