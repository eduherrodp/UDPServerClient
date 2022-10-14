#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#define PORT 5000
#define MAXLINE 1000

// A server UDP has the next optiones: setup account, withdrawal and deposit
struct reg {
    int option;
    int ID;
    int amount;
} registration;

// Driver code
int main() {
    int client;
    char buffer[100];
    char message[150];
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    // Show the server ID from the socket
    printf("Server ID: %d \n", getpid());
    
    sprintf(message, "Welcome to the bank! [server -> %d]\n", getpid());

    // Create a UDP socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // Bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    // Receive the datagram
    len = sizeof(cliaddr);
    
    int n = recvfrom(listenfd, &registration, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);

    
    // Send the message to the client
    sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    printf("En espera de respuesta");
}
