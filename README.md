In this project I have created multiple servers that are able to interact with clients and . The network was designed so that the servers and clients could interact with one another to compute some measurements with values from a database.

## serverA.cpp
ServerA is the primary database server, it searches a database file and returns the requested information if found. ServerA communicates with the server setup in the "aws server" cpp file.

## serverB.cpp
ServerB has the same functionality as serverA, whenever there is a database miss in ServerA we search ServerB before concluding that the requested information is in neither of our servers.

## serverC.cpp
If the information is found in either ServerA or ServerB they will relay back the requested information to the "aws server". Once "aws server" receives the requested information it will forward it to ServerC so that some computation can be made (bandwidth calculations, throughput etc...).

## aws.cpp
The "aws server" is the main server that ties our design together. "aws server" communicates with ServerA, ServerB, and ServerC on the back end, and the monitor/client on the front end.

# monitor.cpp
This file acts as a monitor. The results from computations will be sent to the monitor to be displayed (in the terminal).

## client.cpp
client.cpp is the client process that provides all the input parameters. A request is made from client then passed to "aws server", and then looked up in back end servers. If the search is successful the results are passed to the monitor from the "aws server".


## UDP protocol implementation:

udp_client.cpp and udp_server.cpp use UDP to establish a connection between one another. These two files were created before implementing the above design (using TCP).


- Code was referenced from Beej's guide to socket programming.
