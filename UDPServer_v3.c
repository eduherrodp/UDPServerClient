#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdbool.h>
#define PORT 5000
#define MAXLINE 1500

// A server UDP has the next optiones: setup account, withdrawal and deposit
struct reg {
    int option;
    int ID;
    int amount;
} registration;

bool IDExist() {
    FILE* ptr;
    int id, amount;
    ptr = fopen("cuentas.txt", "r");
    if ( ptr == NULL ) {
        printf("Error opening file");
        exit(1);
    }
    rewind(ptr);
    while ( fscanf(ptr, "%d %d", &id, &amount) != EOF ) 
        if ( id == registration.ID ) 
            return true;
    return false;
}

void createAccount() {
    FILE* ptr;
    ptr = fopen("cuentas.txt", "a");
    if ( ptr == NULL ) {
        printf("Error opening file");
        exit(1);
    }
    fprintf(ptr, "\n%d %d", registration.ID, registration.amount);
    fclose(ptr);
}

int getData() {
    FILE* ptr;
    int id, amount;
    ptr = fopen("cuentas.txt", "r");
    if ( ptr == NULL ) {
        printf("Error opening file");
        exit(1);
    }
    rewind(ptr);
    // Get the data from the file
    while ( fscanf(ptr, "%d %d", &id, &amount) != EOF ) 
        if ( id == registration.ID ) 
            registration.amount = amount;
}

void modifyAmount() {
    FILE* ptr;
    int id, amount;
    ptr = fopen("cuentas.txt", "r");
    if ( ptr == NULL ) {
        printf("Error opening file");
        exit(1);
    }
    rewind(ptr);
    // Get the data from the file
    while ( fscanf(ptr, "%d %d", &id, &amount) != EOF ) {
        if ( id == registration.ID ) 
            amount = registration.amount;
        // Write the new data in a temporal file
        FILE* ptrTemp;
        ptrTemp = fopen("temp.txt", "a");
        if ( ptrTemp == NULL ) {
            printf("Error opening file");
            exit(1);
        }
        // add new line if it is not the first line of the file 
        if ( ftell(ptrTemp) != 0 ) 
            fprintf(ptrTemp, "\n%d %d", id, amount);
        else 
            fprintf(ptrTemp, "%d %d", id, amount);
        fclose(ptrTemp);
    }
    fclose(ptr);
    // Delete the old file
    remove("cuentas.txt");
    // Rename the temporal file
    rename("temp.txt", "cuentas.txt");
}

// Driver code
int main() {
    int client;
    char buffer[MAXLINE];
    char message[MAXLINE];
    int listenfd, len;
    struct sockaddr_in servaddr, cliaddr;
    bzero(&servaddr, sizeof(servaddr));

    // Show the server ID from the socket
    printf("Server ID: %d \n", getpid());

    // Create a UDP socket
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;

    // Bind server address to socket descriptor
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    // Receive the datagram
    len = sizeof(cliaddr);
    // Waiting for the client to select an option
    recvfrom(listenfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&cliaddr, &len);
    // Send the menu to the client
    sprintf(message, "Welcome to the bank! [server -> %d]\nSelect a option to continue\n1. Alta de cuenta\n2. Retiro\n3. Deposito\n", getpid());
    sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
    // Get the option from the client
    recvfrom(listenfd, &registration.option, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
    switch ( registration.option ) {
        case 1:
            printf("Client creating an account\n");
            sprintf(message, "Alta de cuenta");
            sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

            recvfrom(listenfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
            // Searh if the ID account don't exist
            if ( IDExist() )
                // The ID account don't exist
                sprintf(message, "The ID account already exist");
            else {
                // The ID account exist
                sprintf(message, "Cuenta creada satisfactoriamente, tu ID es: %d", registration.ID);
                // Save the ID account in the file txt with 0 amount of money
                createAccount();
            }
            sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
            
        break;
        case 2:
            bzero(buffer, sizeof(buffer));
            printf("Cliente realizando un retiro\n");

            // Request the ID account to the client
            sprintf(message, "Retiro\n\nID de tu cuenta: ");
            sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

            // Get the ID account from the client
            recvfrom(listenfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
            printf("ID account: %d\n", registration.ID);

            // Seatch if the ID account exist
            if( IDExist() ) {
                // The ID account exist
                // Send just the amount of money to the client
                getData();
                sprintf(message, "%d", registration.amount);       
                // Sent to the client
                sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
                // Get the amount of money to withdraw
                recvfrom(listenfd, &registration.amount, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
                // Modify the amount of money in the file txt
                modifyAmount();
                // Send the message to the client
                sprintf(message, "Retiro realizado satisfactoriamente");
                sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
            } else 
                // The ID account don't exist
                sprintf(message, "El ID no existe");
        break;
        case 3:
            bzero(buffer, sizeof(buffer));
            printf("Cliente realizando un retiro\n");
            sprintf(message, "Abono\n\nID de tu cuenta: ");
            sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

            // Get the ID account from the client
            recvfrom(listenfd, &registration.ID, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
            printf("ID account: %d\n", registration.ID);

            // Seatch if the ID account exist
            if( IDExist() ) {
                // The ID account exist
                // Send just the amount of money to the client
                getData();
                sprintf(message, "%d", registration.amount);       
                // Sent to the client
                sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
                // Get the amount of money to withdraw
                recvfrom(listenfd, &registration.amount, sizeof(struct reg), 0, (struct sockaddr*)&cliaddr, &len);
                // Modify the amount of money in the file txt
                modifyAmount();
                // Send the message to the client
                sprintf(message, "Abono realizado satisfactoriamente");
                sendto(listenfd, message, MAXLINE, 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));
            } else 
                // The ID account don't exist
                sprintf(message, "El ID no existe");
        break;
        default:
            // Send the error message to the client
            sprintf(message, "Error: Invalid option");
        break;
    }
}