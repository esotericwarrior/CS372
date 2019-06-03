/************************************************************
* Name: Tristan Santiago
* Date: May 26, 2019
* Description: ftserver.c starts on a command-line specified
* host and waits for a connection from ftclient.py. Once the
* connection has been established, the command is processed
* and the results are sent back to the client. I borrowed
* heavily from my CS344 OTP submission, which of was based on
* code provided from the instructor in CS344 (server.c) &
* (client.c). Additionally sources used are found in comments
* throughout the program.
*************************************************************/
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
int countFiles(struct dirent *de, int count);
int getDirectory(struct dirent *de, int count, char current_dir[count + 1][256]);
int searchForFile(int count, char current_dir[count + 1][256], char * filename);
char ** create_string_array(int size);
int does_file_exist(char ** files, int count, char * filename);
char* readFile(char* fileName);

int main(int argc, char *argv[]) {
	int i, listenSocketFD, establishedConnectionFD, portNumber, charsRead, charsWritten;
	socklen_t sizeOfClientInfo;
	//int count = 0;	/* Variable to hold the number of files found in directory. */
	char buffer[500000];
	// char current_dir[count + 1][256];	// 2D Array to hold the directory.
	struct sockaddr_in serverAddress, clientAddress;
	struct dirent *de;	// Pointer for directory entry.


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
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char clientHost[256];
	memset(clientHost, '\0', 256);
	strcpy(clientHost, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	printf("Connection from %s\n", clientHost);	// Print message, as per instructions.

	/* Get command from the client. */
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char command[256];
	memset(command, '\0', 256);
	strcpy(command, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

	/* Get filename from the client. */
	memset(buffer, '\0', sizeof(buffer));
	charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
	if (charsRead < 0) error("ERROR reading from socket");
	//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
	char filename[256];
	memset(filename, '\0', 256);
	strcpy(filename, buffer);
	charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back

	/* Get the data port from the client. */
	memset(buffer, '\0', sizeof(buffer));
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
		// struct dirent *de;	// Pointer for directory entry
		int count = 0;	/* Initialize the number of files found in directory. */
		/* Count number of files in the directory. */
		count = countFiles(de, count);

		char current_dir[count + 1][256];	// 2D Array to hold the directory.
		getDirectory(de, count, current_dir);

		/* Referenced: https://tinyurl.com/y3wottjy */
		int number_to_send = (count + 1);
		// printf("Number to send = %d\n", number_to_send);	/* Debugging: Print number of files to be sent to client. */
		int converted_number = htonl(number_to_send);
		/* Send the size of the directory to the client. */
		charsRead = send(establishedConnectionFD, &converted_number, sizeof(converted_number), 0);
		memset(buffer, '\0', sizeof(buffer));
		charsRead = recv(establishedConnectionFD, buffer, 255, 0); /* Read the client's message from the socket. */
		if (charsRead < 0) error("ERROR reading from socket");
		printf("Sending directory contents to %s:%s\n", clientHost, data_port);	/* Print message as specified by instructions. */
		for (i = 0; i < (count+1); i++) {
			// printf("Index[%d]: %s\n", i, current_dir[i]);
			charsRead = send(establishedConnectionFD, current_dir[i], strlen(current_dir[i]), 0);
			memset(buffer, '\0', sizeof(buffer));
			charsRead = recv(establishedConnectionFD, buffer, 255, 0); // Read the client's message from the socket
			if (charsRead < 0) error("ERROR reading from socket");
			//printf("SERVER: I received this from the client: \"%s\"\n", buffer);
		}
	}
	/* If command = "-g": */
	else if(strcmp(command, "-g") == 0) {
		printf("File \"%s\" requested on port %s\n", filename, data_port);
		/* TO DO: Send file stuff here... */
		int count = 0;	/* Initialize the number of files found in directory. */
		/* Count number of files in the directory. */
		count = countFiles(de, count);

		char current_dir[count + 1][256];	// 2D Array to hold the directory.
		getDirectory(de, count, current_dir);
		/* Search for file. */
		char ** files = create_string_array(100);
		//int file_exists = does_file_exist(files, count, filename);
		int fileFound = searchForFile(count, current_dir, filename);

		// printf("fileFound value = %d\n", fileFound);	// Debugging: print value of fileFound.
		/* If we don't find it, do that: */
	    if (!fileFound) {
	    	char * file_not_found = "File not found.";
	    	send(establishedConnectionFD, file_not_found, 15, 0);
	    	printf("File not found!\n");
	    }
	    /* If we find it, do this: */
	    else {	// fileFound == 1
	    	int n = 0;
	    	int bufferLength;
	    	printf("Sending \"%s\" to %s:%s\n", filename, clientHost, data_port);
	    	/* Send success message to client. */
	    	char * file_found = "File found.";
	    	send(establishedConnectionFD, file_found, 11, 0);
	    	
	    	char *toSend;
	    	toSend = readFile(filename);

	    	/* Send file size to client. */
	    	// int file_size = strlen(toSend);
	    	// int converted_size = htonl(file_size);
	    	// printf("Size of file: %d\n", file_size);
	    	// printf("Converted size: %d\n", converted_size);
	    	// charsRead = send(establishedConnectionFD, &converted_size, sizeof(converted_size), 0);
	    	// if (charsRead < 0) error("ERROR reading from socket");

	    	strcpy(buffer, toSend);
	    	// strcat(buffer, "@");
	    	//printf("Buffer: %s\n", buffer);

	    	
	    	/* Send contents of the file to the client. */
	    	// strcpy(buffer, toSend);	// Debugging: Verify the contents of toSend.
	    	// printf("BUFFER = %s\n", buffer);	// Debugging: Verify contents of toSend.
	  //   	charsWritten = send(establishedConnectionFD, toSend, strlen(toSend), 0);
	  //   	if (charsWritten < 0) error("ERROR reading from socket");
			// if (charsWritten < strlen(toSend)) printf("CLIENT: WARNING: Not all data written to socket!\n");
			// printf("Characters written: %d\n", charsWritten);	// Debugging: Verify all characters sent through socket.

			/* Send contents of the file to the client. */
	    	charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0);
	    	if (charsWritten < 0) error("ERROR reading from socket");
			if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
			printf("Characters written: %d\n", charsWritten);	// Debugging: Verify all characters sent through socket.


	  //   	memset(buffer, '\0', sizeof(buffer));
	  //   	strcpy(buffer, "__@__");	// Special character
	  //   	//printf("Buffer = %s\n", buffer);
	  //   	charsWritten = send(establishedConnectionFD, buffer, strlen(buffer), 0);
			// if (charsWritten < 0) error("ERROR reading from socket");
			// if (charsWritten < strlen(buffer)) printf("CLIENT: WARNING: Not all data written to socket!\n");
	    	
	    	close(establishedConnectionFD);
	    }
	}
	/* Default case: Invalid or empty command: */
	else {
		printf("Invalid command entered.\n");
	}

	/* Send a Success message back to the client. */
	//charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
	if (charsRead < 0) error("ERROR writing to socket");
	close(establishedConnectionFD); // Close the existing socket which is connected to the client
	
	}	// While Loop

	close(listenSocketFD); // Close the listening socket
	return 0;	
}

int countFiles(struct dirent *de, int count) {
		/* Referenced: https://www.geeksforgeeks.org/c-program-list-files-sub-directories-directory */
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

int getDirectory(struct dirent *de, int count, char current_dir[count + 1][256]) {
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

char ** create_string_array(int size) {
	/* Create heap string array. */
	char ** array = malloc(size*sizeof(char *));
	int i;
	for (i = 0; i < size; i++) {
		array[i] = malloc(100*sizeof(char));
		memset(array[i], 0, sizeof(array[i]));
	}
	return array;
}

int does_file_exist(char ** files, int count, char * filename) {
	int file_exists = 0;	// Boolean
	int i;
	for (i = 0; i < count; i++) {
		if(strcmp(files[i], filename) == 0) {
			file_exists = 1;
		}
	}
	return file_exists;
}

// https://stackoverflow.com/questions/17323748/how-to-search-a-word-in-a-2d-string-array-in-c-programming
int searchForFile(int count, char current_dir[count + 1][256], char * filename) {
	int fileFound = 0;	/* Boolean */
	int i;
	for (i = 0; i < count; i++) {
		if (strstr(current_dir[i], filename) != NULL) {
			// printf("Found %s in position %d,%s\n", filename, i + 1,
			// 	current_dir[i]);
			// printf(" index of file is %d.\n", i + 1);
			fileFound = 1;
		}
	}
	return fileFound;
}

// Referenced: https://stackoverflow.com/questions/2029103/correct-way-to-read-a-text-file-into-a-buffer-in-c
char* readFile(char* fileName) {
	char *source = NULL;
	/* Open the file. */
	FILE* fp = fopen(fileName, "r");
	if (fp == NULL) {	// If file can't be found:
		error("Unable to open file");	// Throw an error.
	}
	/* If file is found: */
	if (fp != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) { // Move the file pointer to the end of the file.
			long bufSize = ftell(fp);	// Get the size of the file and assign it to variable.
			if (bufSize == -1) {
				error("Invalid file");
				exit(1);
			}
			/* Allocate our buffer to that size. */
			source = malloc(sizeof(char) * (bufSize + 1));
			/* Go back to the start of the file. */
			if (fseek(fp, 0L, SEEK_SET) != 0) { // Return the file pointer back to the beginning of the file.
				error("Unable to read file");
			}
			/* Read the entire file into memory. */
			size_t newLen = fread(source, sizeof(char), bufSize, fp);
			if ( ferror( fp ) != 0 ) {
				fputs("Error reading file", stderr);
			}
			else {
				source[newLen++] = '\0'; /* Just to be safe. */
			}
		}
	}
	fclose(fp);
	return source;
}