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
#define MAXLINE 1500

// Define the structure from the account
struct reg {
    int option;
    int ID;
    int amount;
} registration;

// Driver code
int main(){
    char buffer[MAXLINE];
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
    if ( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        printf("Connection failed \n");
        exit(0);
    }

    sendto(sockfd, &registration, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

    // Now recieve the message from the server
    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
    puts(buffer);

    // Get the option from the client
    printf("OP: ");
    scanf("%d", &registration.option);

    // Send the option to the server
    bzero(buffer, sizeof(buffer));
    sendto(sockfd, &registration.option, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));
    bzero(buffer, sizeof(buffer));
    switch(registration.option) {
        case 1:
            do {
                // Receive the message from the server
                recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
                puts(buffer);
                printf("Select a ID to create an account (4 numbers maximum): ");
                scanf("%d", &registration.ID);
            } while ( registration.ID > 9999 );
            sendto(sockfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

            // Receive the message from the server
            bzero(buffer, sizeof(buffer));
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            puts(buffer);
        break;
        case 2:
            bzero(buffer, sizeof(buffer));
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            puts(buffer);
            scanf("%d", &registration.ID);
            sendto(sockfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

            // Receive the message from the server
            bzero(buffer, sizeof(buffer));
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            printf("Saldo: %s\n", buffer);
            // Convert buffer to int
            int amount = atoi(buffer);
            if ( amount == 0 )
                printf("No cuentas con los fondos suficientes\n");
            else {
                printf("Cantidad a retirar: ");
                scanf("%d", &registration.amount);
                if ( registration.amount > amount ) 
                    printf("No cuentas con los fondos suficientes\n");
                else {
                    registration.amount = amount - registration.amount;
                    sendto(sockfd, &registration.amount, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

                    // Receive the message from the server
                    bzero(buffer, sizeof(buffer));
                    recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
                    puts(buffer);
                }
            }
        break;
        case 3:
            bzero(buffer, sizeof(buffer));            
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            puts(buffer);
            scanf("%d", &registration.ID);
            sendto(sockfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

            // Receive the message from the server
            bzero(buffer, sizeof(buffer));
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            printf("Saldo: %s\n", buffer);
            // Convert buffer to int
            int amount2 = atoi(buffer);
            
            printf("Cantidad a depositar: ");
            scanf("%d", &registration.amount);
            registration.amount = amount2 + registration.amount;
            sendto(sockfd, &registration.amount, sizeof(struct reg), 0, (struct sockaddr*)NULL, sizeof(servaddr));

            // Receive the message from the server
            bzero(buffer, sizeof(buffer));
            recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
            puts(buffer);
        break;
    }
}