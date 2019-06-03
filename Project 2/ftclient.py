# !#/bin/python
#****************************************************************************
# Name: Tristan Santiago
# Date: May 28, 2019
# Description: ftclient.py connects to the ftserver.c and sends the request
# specified on the command line. I started with the chatServe.py program from
# Project 1 and modified my way from there, using a variety of sources to
# help me along the way, including: https://pymotw.com/2/socket/tcp.html
# Other sources used are mentioned throughout the program.
#****************************************************************************
import socket
import sys
import struct
import time
import os.path

#def setupConnection(server_port):  # Function responsible for setting up the socket.
def setupConnection(server_name, server_port):
    # Create a TCP/IP socket.
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
    	clientSocket.connect((server_name, int(server_port)))
    except:
    	print("Error connecting to port %d\n" % (server_port), 1)
    return clientSocket

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

def sendCommand(socket, command):
	try:
		clientSocket.send(command)
	except:
		print ("Error sending '%s' command" % (command), 1)

def bytes_to_number(b):
	# if Python2.x
	b = map(ord, b)
	res = 0
	for i in range(4):
		res += b[i] << (i*8)
	return res

# Referenced: https://stackoverflow.com/questions/17667903/python-socket-receive-large-amount-of-data/34236030
def recvall(sock, n):
	data = b''
	while len(data) < n:
		packet = sock.recv(n - len(data))
		if not packet:
			return None
		data += packet
	return data

if __name__ == "__main__":
	validate_args()

	# Initialize variables.
	server_name = sys.argv[1]	# Server host name.
	server_port = sys.argv[2]	# Server port number.
	command = sys.argv[3]	# Command line argument.
	filename = "filename"	# Initialize to default string.
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
	elif command == '-g':
		# print("Get command received.")
		sendCommand(server_port, command)
		receivedMessage = clientSocket.recv(1024)
		# print("Receiving directory structure from " + server_name + ":" + server_port)		
		# Send filename.
		clientSocket.send(filename)
		#receivedMessage = clientSocket.recv(1024)
		#print("Message received from server: " + receivedMessage)

		# Send data port.
		clientSocket.send(data_port)
		receivedMessage = clientSocket.recv(1024)
		#print("Message received from server: " + receivedMessage)

		# Determine whether or not file exists.
		clientSocket.send("ok")
		receivedMessage = clientSocket.recv(1024)
		# If the server doesn't respond with "File found." message:
		if receivedMessage != "File found.":
			print("Message received from server: " + receivedMessage)	# Print error message & exit.
			exit(1)	# Return statement doesn't work here for some reason.

		# Get file size from the server.
		# clientSocket.send("ok")
		# size = clientSocket.recv(4)
		# size = bytes_to_number(size)
		#print ("Size: ", size)

		overwrite = True
		# Check for existing copy of requested file.
		# http://stackoverflow.com/questions/82831/check-whether-a-file-exists-using-python
		if os.path.isfile(filename):
			# Prompt for overwrite
			choice = raw_input("File Already Exists; Overwrite? (y/n)")
			if choice == 'y':
				overwrite = True
			else:
				overwrite = False
		if overwrite == False:
			filename = filename + ".copy"

		# Receive contents of the file from server.
		# with open(filename, 'w+') as f:
		# 	data = b''
		# 	while len(receivedMessage) < size:
		# 		receivedMessage = clientSocket.recv(size - len(data))
		# 		if not receivedMessage:
		# 			break
		# 		# Write incoming data to file.
		# 		f.write(receivedMessage)
		# 		data += receivedMessage
		# f.close()

		# # Receive contents of the file from server.
		with open(filename, 'wb') as f:
			print 'File opened'
			while True:
				try:
					#print('Receiving data...')
					receivedMessage = clientSocket.recv(1024)
					#print('receivedMessage=%s', (receivedMessage))
					if not receivedMessage:
						break
					# Write incoming data to file.
					f.write(receivedMessage)
				except:
					print ("Error writing from socket.")
		f.close()

		print("Transfer complete.")
	clientSocket.close()