# !#/bin/python
#****************************************************************************
# Name: Tristan Santiago
# Date: May 28, 2019
# Description:
# Referenced: https://pymotw.com/2/socket/tcp.html
#****************************************************************************
import socket
import sys

def setupConnection(server_port):  # Function responsible for setting up the socket.
    # Create a TCP/IP socket.
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind the socket to the port.
    HOST = socket.gethostname()	# Get host name.
    if sys.argv[3] == "-l":
    	portNum = 2
    elif sys.argv[3] == "-g":
    	portNum = 5
    PORT = int(sys.argv[portNum])	# Transform the port into an integer.
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

def validate_args():
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print "USAGE: python ftclient.py <server_host> <server_port> <command> <filename (-g only)> <data_port>"
		exit(1)
	elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):
		print "Invalid command. Please use -l or -g"
	elif (int(sys.argv[2]) < 1024 or (int(sys.argv[2]) > 65535)):
		print "Invalid port number. Enter a port number within the range of 1024 to 65535."
		exit(1)
	elif (int(sys.argv[5]) < 1024 or (int(sys.argv[5]) > 65535)):
		print "Invalid port number. Enter a port number within the range of 1024 to 65535."
		exit(1)

if __name__ == "__main__":
	validate_args()

	server_name = sys.argv[1]
	server_port = sys.argv[2]
	command = sys.argv[3]

	filename = ""	# Initialize to empty string.
	if command == '-g':
		filename = sys.argv[4]	# Assign filename.

	sock = setupConnection(server_port)