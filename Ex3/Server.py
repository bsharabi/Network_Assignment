# import socket module
from socket import *
# In order to terminate the program
import sys  
# AF_INET=ipv4,   SOCK_STREAM = TCP
serverSocket = socket(AF_INET, SOCK_STREAM)
#Prepare a sever socket
#The server port is the port number of the server it is listening to
serverPort = 80
serverSocket.bind(("127.0.0.1", serverPort))
# Queue for http request
serverSocket.listen(5)
while True:
    # Establish the connection
    print('Ready to serve...')
    connectionSocket, addr = serverSocket.accept()
    #print the addres's client
    print(f"Hello client ip number {addr}")
    try:
        # message will get from client receive on bits 1024 bit
        message = connectionSocket.recv(1024)
        #Finding the file name from the client's request
        filename = message.split()[1].decode('utf-8')
        #opened and read
        f = open(filename[1:])
        outputdata = f.read()
        # Send one HTTP header line into socket (Status code)
        first_header = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
        connectionSocket.send(bytes(first_header, 'utf-8'))
        
        # Send the content of the requested file to the client
        for i in range(0, len(outputdata)):
            connectionSocket.send(bytes(outputdata[i], 'utf-8'))
        connectionSocket.send("\r\n".encode('utf-8'))
        # Close client socket
        connectionSocket.close()
    except IOError:
        # Send response message for file not found
        connectionSocket.send(
            bytes("HTTP/1.1 404 Not Found\r\nContent-Type:"+
                " text/html\r\n\r\n<!doctype html><html><body><h1>404 Not Found<h1></body></html>",'utf-8'))
        # Close client socket
        connectionSocket.close()
    # Close server socket
    serverSocket.close()
    sys.exit()  # Terminate the program after sending the corresponding data
