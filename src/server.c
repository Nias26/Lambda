#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 100
#define BUFFER_SIZE 4096

int clients[MAX_CLIENTS];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Boradcast message to every client
void broadcast(char* msg, int client_fd){
  pthread_mutex_lock(&mutex);
  for(int i = 0; i < MAX_CLIENTS; i++){
    if(clients[i] != 0 && clients[i] != client_fd){
      send(clients[i], msg, strlen(msg), 0);
    }
  }
  pthread_mutex_unlock(&mutex);
}

// Handle the new clients
void *handle_client(void* arg){
  // Client fd, buffer and recv size
  int sock = *(int*)arg;
  char buffer[BUFFER_SIZE];
  int read_size = 0;

  memset(buffer, '0', BUFFER_SIZE);

  // while the client is still alive, broadcast their messages
  while((read_size = recv(sock, buffer, BUFFER_SIZE, 0)) > 0){
    buffer[read_size] = '\0';
    broadcast(buffer, sock);
  }

  // Cleanup
  pthread_mutex_lock(&mutex);
  for(int i = 0; i < MAX_CLIENTS; i++){
    if(clients[i] == sock){
      clients[i] = 0;
    }
  }
  pthread_mutex_unlock(&mutex);
  close(sock);
  return NULL;
}

int main(int argc, char** argv){
  if (argc < 2){
    printf("Too few arguments\nUsage: <port>\n");
    return 1;
  }

  int port = htons(atoi(argv[1]));

  // Zero out the clients fds
  memset(clients, 0, MAX_CLIENTS);

  int server_fd, new_sock;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Failed opening socket");
    return 1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = port;

  if (bind(server_fd, (struct sockaddr*)&address, addrlen) < 0){
    perror("Failed binding socket");
    close(server_fd);
    return 1;
  };

  listen(server_fd, MAX_CLIENTS);

  while(1){
    new_sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);

    pthread_mutex_lock(&mutex);
    for(int i = 0; i < MAX_CLIENTS; i++){
      if(clients[i] == 0){
        clients[i] = new_sock;
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, &clients[i]);
        break;
      }
    }
    pthread_mutex_unlock(&mutex);
  }
  return 0;
}

