Tristan Santiago
May 2, 2019
CS 372_400
Project 2

Instructions for compiling and running both ftserver.c and ftclient.py

1. First, run the "make" command to compile ftserver.c
2. Next, run the following command: "./ftserver [port]", replacing
[port] with the number of your choice, without the brackets "[]."
The server is now running and waiting for a connection from the client.
3. To start the client, please use the following command:
"python ftclient.py <server_host> <server_port> <command> <file_name> <data_port>
4. <server_host> is the name of the host the server was started on. It will either
be "flip1", "flip2", or "flip3". Be sure not to include the brackets "<>" with
the command.
5. <server_port> is the port number the server was started on (i.e. the port
number from step 2.)
6. <command> is either "-l" or "-g"
Enter "-l" for a list of the current directory, or enter "-g" to retrieve a file.
7. <file_name> is the name of the file you wish to retrieve. A file name must be
given following any "-g" command. The "-l" command uses the data port a the file name.
8. <data_port> corresponds to the port the client will use for its operations. This
port number cannot be the same as the server's port number (i.e. the port number from
step 2.) You can use any port number you wish, but it's preferred that you use +1 of
the server's port. That is, if the server's port is 4556, use 4557 for the <data_port>.
Again this is preferred, but not required.
9. Once ftclient.py request has completed, the program terminates. However, the server
continues until it receives an interrupt signal from the keyboard (CTRL+C or CTRL+Z)