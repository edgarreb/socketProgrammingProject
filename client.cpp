#include <iostream> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <string>
#include <sstream>

struct data_info 
{
	int link_id; 
	int size; 
	double power; 
	double bandwidth; 
	double length; 
	double velocity; 
	double noise;
	int m;
	double trans;
    double prop;
};

using namespace std;

#define AWS_PORT "25054"

int main(int argc, char *argv[])
{
	int socket_client;	
 	
	struct addrinfo hints_client, *res_client;

	memset(&hints_client, 0, sizeof(hints_client));    // make sure that the struct is empty 
	hints_client.ai_family = AF_UNSPEC;  			   // compatible with both IPv4 and IPv6
	hints_client.ai_socktype = SOCK_STREAM; 		   //TCP connection
	
	getaddrinfo("127.0.0.1", AWS_PORT, &hints_client, &res_client); 

	// create a socket
	if((socket_client = socket(res_client->ai_family, res_client->ai_socktype, res_client->ai_protocol)) < 0)
	{
		perror("socket error");
        return 0;
	}

	// connect to the socket
	if(connect(socket_client, res_client->ai_addr, res_client->ai_addrlen) < 0)
	{
		perror("connect error");
        return 0;
    }

    cout << "the client is up and running" << endl; 

    struct data_info client_info; 

    stringstream ss; 

    ss << argv[1]; 
    ss >> client_info.link_id; 
    ss.clear(); 
    ss.str(""); 

    ss << argv[2]; 
    ss >> client_info.size; 
    ss.clear(); 
    ss.str(""); 

    ss << argv[3]; 
    ss >> client_info.power; 
    ss.clear(); 
    ss.str(""); 

	if(send(socket_client, &client_info, sizeof(client_info), 0) < 0)
	{
    		perror("sending data failed");
        	return 0;
    }

    cout << "the client sent ID=<" << client_info.link_id << ">, size=<" << client_info.size << ">, and power=<" << client_info.power << "> to AWS " << endl; 

	//receive results from the AWS server
	char buff[20]; 
	struct data_info client_recv; 

	recv(socket_client, &client_recv, sizeof(client_recv), 0);

	if (client_recv.m == 0)
	{ 
		cout << "found no matches for link<" << client_info.link_id << ">" << endl;
	}
	
	cout << "The delay for the link<" << client_recv.link_id << "> is <" << (client_recv.prop +client_recv.trans) << "> ms" << endl;

  	close(socket_client);
  	return 0;

}