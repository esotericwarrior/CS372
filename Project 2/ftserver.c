/************************************************************
* Name: Tristan Santiago
* Date: May 26, 2019
* Description:
*************************************************************/
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[]) {
	int i, listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[256];
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	/* Set up the address struct for this process (the server). */
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	/* Set up the socket. */
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	/* Enable the socket to begin listening. */
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	/* Print message according to assignment's output. */
	printf("Server open on %d\n", portNumber);

	while (1) {
	/* Accept a connection, blocking if one is not available until one connects. */
	sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
	establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
	if (establishedConnectionFD < 0) error("ERROR on accept");

	/* Get host name from the client. */
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char clientHost[256];
	memset(clientHost, '\0', 256);
	strcpy(clientHost, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	printf("Connection from %s\n", clientHost);	// Print message, as per instructions.

	/* Get command from the client. */
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char command[256];
	memset(command, '\0', 256);
	strcpy(command, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

	/* Get filename from the client. */
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char filename[256];
	memset(filename, '\0', 256);
	strcpy(filename, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

	/* Get the data port from the client. */
	memset(buffer, '\0', 256);
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char data_port[256];
	memset(data_port, '\0', 256);
	strcpy(data_port, buffer);
	//charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

	/* Execute commands. */
	/* If command is "-l": */
	if(strcmp(command, "-l") == 0) {
	printf("List directory requested on port %s\n", data_port);
	//int count = countFiles();
	// Referenced: https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory/		
		struct dirent *de;	// Pointer for directory entry
		int count = 0;	/* Initialize the number of files found in directory. */
		/* Count number of files in the directory. */
		count = countFiles(de, count);

		char current_dir[count + 1][256];	// 2D Array to hold the directory.
		getDirectory(de, count, current_dir);
		/* Store files found in directory for later use. */
		// DIR *ent = opendir(".");
		// char current_dir[count + 1][256];	// 2D Array to hold the directory.
		// int i = 0;
		// while ((de = readdir(ent)) != NULL) {
		//  	if(strcmp(".", de->d_name) == 0 || strcmp("..", de->d_name) == 0) {
		//  		continue;
		//  	}
		//  	strcpy(current_dir[i], de->d_name);
		//  	i++;
		//  	//printf("Test: %s\n", de->d_name);
		// }
		// /* Add special character to the end of the directory array. */
		// strcpy(current_dir[count], "@");
		// //printf("LAST INDEX = %s\n", current_dir[count]);	/*Debugging: Print last index to verify special character. */
		// /* Close the directory. */
		// closedir(ent);


		/* Referenced: https://tinyurl.com/y3wottjy */
		int number_to_send = (count + 1);
		// printf("Number to send = %d\n", number_to_send);	/* Debugging: Print number of files to be sent to client. */
		int converted_number = htonl(number_to_send);
		/* Send the size of the directory to the client. */
		charsRead = send(establishedConnectionFD, &converted_number, sizeof(converted_number), 0);
		memset(buffer, '\0', 256);
		charsRead = recv(establishedConnectionFD, buffer, 255, 0); /* Read the client's message from the socket. */
		if (charsRead < 0) error("ERROR reading from socket");
		printf("Sending directory contents to %s:%s\n", clientHost, data_port);	/* Print message as specified by instructions. */
		for (i = 0; i < (count+1); i++) {
			// printf("Index[%d]: %s\n", i, current_dir[i]);
			charsRead = send(establishedConnectionFD, current_dir[i], strlen(current_dir[i]), 0);
			memset(buffer, '\0', 256);
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");
			//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		}
		//ptest(current_dir, count);
		//printDirectoryTest(current_dir, buffer, charsRead, establishedConnectionFD, count);
	}
	/* If command = "-g": */
	else if(strcmp(command, "-g") == 0) {
		printf("File \"%s\" requested on port %s\n", filename, data_port);
		/* TO DO: Send file stuff here... */

		/* Search for file. */
			/* If we find it, do this: */
			/* If we don't find it, do that: */
		printf("Sending \"%s\" to %s:%s\n", filename, clientHost, data_port);
	}
	/* Default case: Invalid or empty command: */
	else {
		printf("Test\n");
	}

	/* Send a Success message back to the client. */
	//charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	
	}	// While Loop

	close(listenSocketFD); // Close the listening socket
	return 0;	
}

countFiles(struct dirent *de, int count) {
		/* openDir() returns a pointer of DIR type. */
		DIR *dir = opendir(".");
		//int count = 0;
		if (dir == NULL) {	// opendir returns NULL if couldn't open directory.
			printf("Could not open current directory.\n");
			return 0;
		}
		// Refer http://pubs.opengroup.org/onlinepubs/7990989775/xsh/readdir.html 
		// for readdir() 
		while ((de = readdir(dir)) != NULL) {
			/* https://stackoverflow.com/questions/33589672/c-print-out-directories-and-files-recursively */
			/* Found a directory, but ignore "." and ".."" */
			if(strcmp(".", de->d_name) == 0 || strcmp("..", de->d_name) == 0) {
				continue;
			}
			//printf("%s\n", de->d_name);	/* Print the name of the file. */
			count++;	/* Increment the number of files. */
		}
		//printf("Count = %d\n", count);	/* Print the total number of files in directory. */
		return count;
		closedir(dir);
}

getDirectory(struct dirent *de, int count, char current_dir[count + 1][256]) {
		DIR *ent = opendir(".");
		int i = 0;
		while ((de = readdir(ent)) != NULL) {
		 	if(strcmp(".", de->d_name) == 0 || strcmp("..", de->d_name) == 0) {
		 		continue;
		 	}
		 	strcpy(current_dir[i], de->d_name);
		 	i++;
		 	//printf("Test: %s\n", de->d_name);
		}
		/* Add special character to the end of the directory array. */
		strcpy(current_dir[count], "@");
		//printf("LAST INDEX = %s\n", current_dir[count]);	/*Debugging: Print last index to verify special character. */
		/* Close the directory. */
		closedir(ent);
}

// printDirectoryTest(char *current_dir, char *buffer, int charsRead, int establishedConnectionFD, int count) {
// 	int i;
// 	printf("PRINT TEST\n");
// 	for (i = 0; i < (count+1); i++) {
// 		printf("Index[%d]: %c\n", i, current_dir[i]);
// 		charsRead = send(establishedConnectionFD, current_dir[i], strlen(current_dir[i]), 0);
// 		memset(buffer, '\0', 256);
// 		charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
// 		if (charsRead < 0) error("ERROR reading from socket");
// 		//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
// 		}
// }

// ptest(char (*current_dir)[5], int count) {
// 	int i;
// 	for (i = 0; i < (count+1); i++) {
// 		printf("Index[%d]: %s\n", i, current_dir[i]);
// 	}
// }