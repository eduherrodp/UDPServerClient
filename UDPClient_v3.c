// udp client driver program
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 5000
#define MAXLINE 1000

// Define the structure from the account
struct reg {
    int option;
    int ID;
    int amount;
} registration;

// Driver code
int main(){
    char buffer[100];
    int sockfd, n;
    struct sockaddr_in servaddr;

    // Clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // Create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    
    // Connect to server
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        printf("Connection failed \n");
        exit(0);
    }
    // Now recieve the message from the server
    // Send a mesaage to the server from the client
    sendto(sockfd, &registration, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));
    // Wait for the response from the server
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    puts(buffer);
}