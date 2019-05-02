# !#/bin/python

# Name: Tristan Santiago
# Date: May 1, 2019
# Description: chatServe.py
# Referenced: https://pymotw.com/2/socket/tcp.html

import socket
import sys

def setupConnection(port):
	# Create a TCP/IP socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

	# Bind the socket to the port
	HOST = 'localhost'
	PORT = int(port)
	print('HOST: ', HOST)
	print('PORT:', PORT)
	try:	# Test for errors.
		sock.bind((HOST, PORT))
	except socket.error as msg:	# If there are errors, handle them.
		print 'ERROR on binding'	# Print error message.
		exit(1)	# Exit 
	print('Socket bind complete')	# Success message.

	sock.listen(5)	# Flip the socket on - it can now receive up to 5 connections.
	return sock 	# Return created socket.

def chat(connection):
    while 1:
        clientMessage = ''
        clientMessage = connection.recv(501)	# Received message from client.

        if clientMessage == "":
            print(sys.stderr, 'ERROR receiving client chat data...')
            exit(1)

        print(clientMessage)
            if clientMessage == "\quit":	# Quit command.
            print('Client closed the connection')
            connection.close()	# Close the connection.
            break

        # server handle is 8 chars long
        prompt = 'SERVER> '
        message = raw_input(prompt)
        serverChat = prompt + message
        
        # only get the first 500 chars
        serverMessage = serverChat[:500]
        connection.send(serverMessage)
        if serverMessage.find(exitMessage) != -1:
            print('Server closed the connection')
            connection.close()
            break
# MAIN
def main(argv):
    if len(sys.argv) != 2:	# Check for correct number of arguments.
    	print "Usage: python chatServe.py [port]"	# If invalid number or arguments, print usage message.
    	exit(1)
    # Call setupConnection to create socket.
    sock = setupConnection(argv[1])
    while 1:
        # Wait for a connection
        print "Socket now listening on port #", argv[1]
        connection, client_address = sock.accept()

        try:	# Test for errors.
            print('connection from', client_address)
            chat(connection)
        finally:	# Execute
            connection.close()	# Clean up and close the connection.

main(sys.argv)
