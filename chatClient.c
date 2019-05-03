/****************************************************************************
* Name: Tristan Santiago
* Date: May 2, 2019
* Description: A simple client-server network chat application. Most of this
* code has been modified and adopted from boilerplate code provided in CS344.
****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues.
void sendUserHandle(int socketFD, char userHandle[12], char *portNumber);
int readMessage(int socketFD);
int sendMessage(int socketFD, char userHandle[12]);

int main(int argc, char *argv[]) {
    int socketFD, portNumber;   // Socket and port number.
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
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
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address.

    // Set up the socket.
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket.
    if (socketFD < 0) error("CLIENT: ERROR opening socket");
    
    // Connect to server.
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address.
        error("CLIENT: ERROR connecting");

    // Get handle from user.
    memset(userHandle, '\0', 12);
    printf("Please enter a username of no more than 10 characters: ");
    if((fgets(userHandle, 11, stdin) == NULL)) {    // If no handle is entered:
        fprintf(stderr, "ERROR reading user's handle.\n"); // Throw an error.
        exit(1);
    }
    userHandle[strcspn(userHandle, "\n")] = '\0'; // Remove the trailing \n that fgets adds.
    
    // Clear the input stream of any extra characters entered.
    // https://stackoverflow.com/questions/26192830/what-does-this-do-whilec-getchar-n-c-eof
    if(strlen(userHandle) > 9) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);    // Read until we find a new line character or EOF.
    }

    // Add '>' to the user's handle, as specified in the assignment instructions.
    userHandle[strlen(userHandle)] = '>';
    // Add ' ' to the end of the user's handle to separate handle from entered text.
    userHandle[strlen(userHandle)] = ' ';

    // Send initial message to the server.
    sendUserHandle(socketFD, userHandle, argv[2]);
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array.

    while(1) {
        readMessage(socketFD);
        sendMessage(socketFD, userHandle);
    }   // End of while loop.

    close(socketFD); // Close the socket.
    return 0;
}

/****************************************************************************
* Function: void sendUserHandle                                                                           
* Parameters: int socketFD, char userHandle[12], char *portNumber
* Description: This function is responsible for sending the user's handle
* to the server and establishing the initial handshake before chatting begins.
* It copies the user's handle (which has already been formatted according to 
* the assignmnet's specification) into the buffer before concatenating the port
* number and sending it to the server.
*****************************************************************************/
void sendUserHandle(int socketFD, char userHandle[12], char *portNumber) {
    int charsWritten;
    char buffer[512];
    strcpy(buffer, userHandle);
    strcat(buffer, portNumber);
    charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server.
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
    if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
}

/****************************************************************************
* Function: int readMessage
* Parameters: int socketFD
* Description: This function receives messages from the server. If the server
* sends the quit command, this function exits the program, otherwise it prints
* the messages to the console.
*****************************************************************************/
int readMessage(int socketFD) {
    int charsRead;
    char buffer[512];
    // Get return message from server.
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again for reuse.
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end.
    if (charsRead < 0) error("CLIENT: ERROR reading from socket");  // Throw an error.
    printf("%s\n", buffer);   // Print the message sent from the server.

    if(strstr(buffer, "\\quit") != NULL) {  // If the user has entered the quit command:
        printf("Connection terminated by the chat server.\n");    // Print message.
        exit(-1);
        //return -1;  // Exit the program.
    }
    return 0;
}

/****************************************************************************
* Function: int sendMessage
* Parameters: int socketFD, char userHandle[12]
* Description: This function is responsible for sending the user's message to
* the server. It displays the user's handle as a prompt, waiting for the user
* to enter their message before sending it to the server. If the user enters
* the quit command, this function sends the quit command to the server before
* terminating the connection and exitting the program.
****************************************************************************/
int sendMessage(int socketFD, char userHandle[12]) {
    int charsWritten;
    char buffer[512];   // Buffer.
    char messageBuffer[512];
    // Send message back to the server.
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer array.
    strcpy(buffer, userHandle); // Copy the user's handle into the buffer.

    // Clear buffer that holds the user's message before use.
    memset(messageBuffer, '\0', sizeof(messageBuffer));

    // Prompt the user to enter a message.
    printf(userHandle); // userHandle >
    if ((fgets(messageBuffer, 500-1, stdin) == NULL)) { // If no message is entered:
        fprintf(stderr, "ERROR reading message.\n");    // Throw an error.
        return -1;
    }
    // Remove the trailing \n that fgets adds.
    messageBuffer[strcspn(messageBuffer, "\n")] = 0;

    // Clear the input stream of any extra characters entered.
    if(strlen(messageBuffer) > 500) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }

    // Check to see if user has entered the quit command.
    if(strncmp(messageBuffer, "\\quit", 4) == 0) {
        charsWritten = send(socketFD, messageBuffer, strlen(messageBuffer), 0); // Write to the server.
        if (charsWritten < 0) error("CLIENT: ERROR sending message to server.\n");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
        printf("Chat session ended.\n");
        close(socketFD);    // Close the socket.
        return -2;
        //exit(1);
    }
    else {  // Otherwise, send the user's message.
        strcat(buffer, messageBuffer);  // Concatenate the message with the user's handle.
        charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server.
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
    return 0;
}
