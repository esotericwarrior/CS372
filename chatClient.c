#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues.

int main(int argc, char *argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char message[512];
    char messageBuffer[512];
    char buffer[512];   // Buffer.
    char userHandle[12];    // Array to store user's handle.
    
    if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); } // Check usage & args.

    // Set up the server address struct.
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct.
    portNumber = atoi(argv[2]); // Get the port number, convert to an integer from a string.
    serverAddress.sin_family = AF_INET; // Create a network-capable socket.
    serverAddress.sin_port = htons(portNumber); // Store the port number.
    serverHostInfo = gethostbyname(argv[1]); // Convert the machine name into a special form of address.
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Set up the socket.
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");
    
    // Connect to server.
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
        error("CLIENT: ERROR connecting");

    // Get handle from user.
    memset(userHandle, '\0', 12);
    printf("Please enter a username of no more than 10 characters: ");
    if((fgets(userHandle, 11, stdin) == NULL)) {    // If no handle is entered:
        fprintf(stderr, "ERROR reading handle.\n"); // Throw an error.
        exit(1);
    }
    handle[strcspn(handle, "\n")] = '\0'; // Remove the trailing \n that fgets adds.
    
    // Clear the input stream of any extra characters entered.
    if(strlen(userHandle) > 9) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    // Add '>' to the user's handle, as specified in the assignment instructions.
    userHandle[strlen(userHandle)] = '>';
    // Add ' ' to the end of the user's handle to separate handle from entered text.
    userHandle[strlen(userHandle)] = ' ';
    // Get user's message.
    // memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array.
    // fgets(buffer, sizeof(buffer) - 1, stdin); // Get input from the user, trunc to buffer - 1 chars, leaving \0.
    // buffer[strcspn(buffer, "\n")] = '\0'; // Remove the trailing \n that fgets adds.
    // strcpy(buffer, userName);

    // Send initial message to the server.
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array.
    strcpy(buffer, userHandle);
    strcat(buffer, portNumber);
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server.
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
    if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");

    // Get return message from server.
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse.
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end.
    if (charsRead < 0) error("CLIENT: ERROR reading from socket");  // Throw an error.
    printf("%s\"\n", buffer);   // Print the message sent from the server.

    if(strstr(buffer, "\\quit") != NULL) {
        printf("Connection terminated by the chat server.");
        return -1;
    }

    // Send message back to the server.
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array.
    strcpy(buffer, userHandle);

    memset(messageBuffer, '\0', sizeof(messageBuffer));

    // Prompt the user to enter a message.
    printf(userHandle);
    if ((fgets(messageBuffer, 500-1, stdin) == NULL)) {
        fprintf(stderr, "ERROR reading message.\n");
        return -1;
    }
    // Remove the trailing \n that fgets adds.
    messageBuffer[strcspn(messageBuffer, "\n")] = 0;

    // Clear the input stream of any extra characters entered.
    if(strlen(messageBuffer) > 500) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    // Check for quit command.
    if(strncmp(messageBuffer, "\\quit", 4) == 0) {
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server.
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
        printf("Chat session ended.\n");
        close(socketFD);    // Close the socket.
        return -2;
    }
    else {
        strcat(message, messageBuffer);
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server.
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
    }



    close(socketFD); // Close the socket.
    return 0;
}
