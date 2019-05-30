# !#/bin/python
#****************************************************************************
# Name: Tristan Santiago
# Date: May 28, 2019
# Description:
# Referenced: https://pymotw.com/2/socket/tcp.html
#****************************************************************************
import socket
import sys
import struct

#def setupConnection(server_port):  # Function responsible for setting up the socket.
def setupConnection(server_name, server_port):
    # Create a TCP/IP socket.
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
    	clientSocket.connect((server_name, int(server_port)))
    except:
    	print("Error connecting to port %d\n" % (server_port), 1)
    return clientSocket

    # Bind the socket to the port.
    #HOST = socket.gethostname()	# Get host name.
    # if sys.argv[3] == "-l":
    # 	portNum = 2
    # elif sys.argv[3] == "-g":
    # 	portNum = 5
    # PORT = int(sys.argv[portNum])	# Transform the port into an integer.
    # print('HOST: ', HOST)   # Print Host information to make connecting with the chat client easier.
    # print('PORT:', PORT)    # Print the Port as well.

    # try:
    # 	sock.connect((HOST, PORT))
    # except:
    # 	print("Error connecting to port %d" % (PORT), 1)
    # return sock     # Return created socket to be stored in a variable for later use.

def validate_args():
	if len(sys.argv) < 5 or len(sys.argv) > 6:
		print "USAGE: python ftclient.py <server_host> <server_port> <command> <filename (-g only)> <data_port>"
		exit(1)
	elif (sys.argv[3] != "-g" and sys.argv[3] != "-l"):
		print "Invalid command. Please use -l or -g"
		exit(1)

	if (len(sys.argv) == 5):
		if (int(sys.argv[2]) < 1024 or (int(sys.argv[2]) > 65535)):
			print "Invalid port number. Enter a port number within the range of 1024 to 65535."
			exit(1)

	if (len(sys.argv) == 6):
		if (int(sys.argv[5]) < 1024 or (int(sys.argv[5]) > 65535)):
			print "Invalid port number. Enter a port number within the range of 1024 to 65535."
			exit(1)
	# elif (int(sys.argv[2]) < 1024 or (int(sys.argv[2]) > 65535)):
	# 	print "Invalid port number. Enter a port number within the range of 1024 to 65535."
	# 	exit(1)
	# elif (int(sys.argv[5]) < 1024 or (int(sys.argv[5]) > 65535)):
	# 	print "Invalid port number. Enter a port number within the range of 1024 to 65535."
	# 	exit(1)

def sendCommand(socket, command):
	try:
		clientSocket.send(command)
	except:
		print ("Error sending '%s' command" % (command), 1)

if __name__ == "__main__":
	validate_args()

	# Initialize variables.
	server_name = sys.argv[1]	# Server host name.
	server_port = sys.argv[2]	# Server port number.
	command = sys.argv[3]	# Command line argument.
	filename = "filename"	# Initialize to empty string.
	if command == '-g':	# If the "-g" command is used:
		filename = sys.argv[4]	# Assign filename.

	if len(sys.argv) == 6:	# If there are six arguments:
		data_port = sys.argv[5]	# Assign the 5th argument as the data port.

	if len(sys.argv) == 5:	# Else, if there are five arguments:
		data_port = sys.argv[4]	# Assign the 4th argument as the data port.

	# Initialize connection to server.
	clientSocket = setupConnection(server_name, server_port)

	# Send host name.
	clientSocket.send(server_name)
	receivedMessage = clientSocket.recv(1024)
	print("Message received from server: " + receivedMessage)

	# Send command.
	if command == '-l':
		sendCommand(server_port, command)
		receivedMessage = clientSocket.recv(1024)
		print("Receiving directory structure from " + server_name + ":" + server_port)

	# Send filename.
	clientSocket.send(filename)
	#receivedMessage = clientSocket.recv(1024)
	#print("Message received from server: " + receivedMessage)

	# Send data port.
	clientSocket.send(data_port)
	receivedMessage = clientSocket.recv(1024)
	#print("Message received from server: " + receivedMessage)

	# Receive size of directory.
	# https://stackoverflow.com/questions/33913308/socket-module-how-to-send-integer
	size = clientSocket.recv(4)
	# print "size is ", size
	# print "repr(size) is ", repr(size)
	dir_size = struct.unpack("!i", size)[0]
	#print "Number of files in directory = ", dir_size
	clientSocket.send("ok")

	times_to_repeat = dir_size
	while times_to_repeat >= 0:
		receivedMessage = clientSocket.recv(1024)
		if (receivedMessage != "@"):
			print receivedMessage
			clientSocket.send("ok")
			times_to_repeat -= 1
		elif (receivedMessage == "@"):
			break


	# receivedMessage = clientSocket.recv(1024)
	# print("Index 0: " + receivedMessage)
	# clientSocket.send("ok")
	
	# receivedMessage = clientSocket.recv(1024)
	# print("Index 1: " + receivedMessage)
	# clientSocket.send("ok")

	# receivedMessage = clientSocket.recv(1024)
	# print("Index 2: " + receivedMessage)
	# clientSocket.send("ok")

	# receivedMessage = clientSocket.recv(1024)
	# print("Index 3: " + receivedMessage)
	# clientSocket.send("ok")

	# receivedMessage = clientSocket.recv(1024)
	# print("Index 4: " + receivedMessage)
	# clientSocket.send("ok")

	clientSocket.close()