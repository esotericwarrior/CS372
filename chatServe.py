# !#/bin/python
#****************************************************************************
# Name: Tristan Santiago
# Date: May 1, 2019
# Description: chatServe.py is the server for the network chat application.
# It waits for a client connection and accepts the user's handle and port
# number before enabling communication.
# Referenced: https://pymotw.com/2/socket/tcp.html
#****************************************************************************
import socket
import sys

def setupConnection(port):  # Function responsible for setting up the socket.
    # Create a TCP/IP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind the socket to the port
    HOST = socket.gethostname()
    PORT = int(port)
    print('HOST: ', HOST)   # Print Host information to make connecting with the chat client easier.
    print('PORT:', PORT)    # Print the Port as well.
    try:    # Test for errors.
        sock.bind((HOST, PORT))
    except socket.error as msg: # If there are errors, handle them.
        print 'ERROR on binding'    # Print error message.
        exit(1) # Exit 
    print('Socket bind complete')   # Success message.

    sock.listen(5)  # Flip the socket on - it can now receive up to 5 connections.
    return sock     # Return created socket to be stored in a variable for later use.

def chat(connection):   # Function responsible for sending/receiving messages.
    clientMessage = ""  # Variable to hold the client's message.
    while 1:    # Continue until quit command received.
        clientMessage = connection.recv(501)    # Received message from client.

        if not clientMessage:   # If no message is received:
            print(sys.stderr, 'ERROR receiving client chat data...')    # Throw an error.
            sys.exit(1) # Exit.

        print(clientMessage)    # Print the client's message to the console.
        if clientMessage == "\quit":    # Quit command.
            print('Client closed the connection')   # Print terminating message.
            connection.close()  # Close the connection.
            break

        # Prompt message from server.
        prompt = 'SERVER> ' # Server's handle.
        message = raw_input(prompt) # Copy prompt into message variable.
        serverChat = prompt + message   # Combine message with prompt.
        
        # Allow messages up to 500 characters.
        serverMessage = serverChat[:500]
        connection.send(serverMessage)
        if serverMessage.find("\quit") != -1:    # Quit command.
            print('Connection terminated by the server.') # Termination message.
            connection.close()  # Close the connection.
            break   # Break back to main.
# MAIN
def main(argv):
    if len(sys.argv) != 2:  # Check for correct number of arguments.
        print "Usage: python chatServe.py [port]"   # If invalid number or arguments, print usage message.
        exit(1)
    # Call setupConnection to create socket.
    sock = setupConnection(argv[1])
    while 1:
        # Wait for a connection
        print "Socket now listening on port #", argv[1]
        connection, client_address = sock.accept()

        try:    # Test for errors.
            print('connection from', client_address)
            chat(connection)
        finally:    # Execute
            connection.close()  # Clean up and close the connection.

main(sys.argv)
