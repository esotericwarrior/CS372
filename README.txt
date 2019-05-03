Tristan Santiago
May 2, 2019
CS344 - Project 1
Simple TCP/IP Client-Server Network Chat Application

Instructions for compiling chatClient.c
1. To compile chatClient.c, run 'make' command.

Instruction for running chatServe.py & chatClient.c
1. To run chatServe.py, run 'python chatServe.py [port]' command,
where [port] is the port number you choose to start the server
on (without the brackets). The server is now running.
2. The server will print the HOST and PORT to the console. This
information will be used to run chatClient.c
3. To start the chat client, run 'chatClient [hostname] [port]',
where [hostname] = HOST and [port] = PORT from the previous step,
without the brackets. The chat client should now be connected to the
server.
4. Now that the connection has been established, the chat client will
prompt you to enter a user handle. Once you enter the handle and press
ENTER, that information will be sent to the server, and will wait for
the server's response.
5. You can now send a message from the client to the server. Once
the server sends the message, the client will display it and prompt
you to send a message back to the server.
6. You can continue to send messages back and forth for as long as
7. To terminate the chat session, type '\quit' from either the server
or the client.
8. The server will continue listening for future client requests until
it receives a keyboard signal (e.g. CTRL+C or CTRL+Z).
9. Run the 'make clean' command to remove the chatClient executable
from the directory.
