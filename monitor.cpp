#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>

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

#define AWS_PORT "26054"

int main(int argc, char *argv[])
{
	int socket_monitor;

	struct addrinfo hints_monitor, *res_monitor;

	memset(&hints_monitor, 0, sizeof(hints_monitor));    // make sure that the struct is empty
	hints_monitor.ai_family = AF_UNSPEC;  			     // compatible with both IPv4 and IPv6
	hints_monitor.ai_socktype = SOCK_STREAM; 		     //TCP connection

	getaddrinfo("127.0.0.1", AWS_PORT, &hints_monitor, &res_monitor);

	// create a socket
	if((socket_monitor = socket(res_monitor->ai_family, res_monitor->ai_socktype, res_monitor->ai_protocol)) < 0)
	{
		perror("socket error");
        return 0;
	}

	// connect to the socket
	if(connect(socket_monitor, res_monitor->ai_addr, res_monitor->ai_addrlen) < 0)
	{
		perror("connect error");
        return 0;
    }

    cout << "the monitor is up and running" << endl; 

	struct data_info M_recv;

	recv(socket_monitor, &M_recv, sizeof(M_recv), 0);

	cout << "the monitor recieved link ID=<" << M_recv.link_id << ">, size=<" << M_recv.size << ">, and power=<" << M_recv.power <<
    "> from the client using TCP over port<" << AWS_PORT << ">" << endl;

    struct data_info M_recv_two;
    int check2 = recv(socket_monitor, &M_recv_two, sizeof(M_recv_two), 0);

    if(M_recv_two.m == 0)
    {
    	cout << "found no matches for link <" << M_recv_two.link_id << ">" << endl;
    }

    cout << "The results of link <" << M_recv_two.link_id << ">" << endl;
    cout << "Tt = <" << M_recv_two.trans << "> ms" << endl;
    cout << "Tp = <" << M_recv_two.prop << "> ms" << endl;
    cout << "Delay <" << (M_recv_two.trans + M_recv_two.prop) << "> ms" << endl;

  	//close(socket_monitor);

  	return 0;

}
