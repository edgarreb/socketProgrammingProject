#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
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

// port that serverB will be using (AWS needs a copy of this port number)
#define SERVER_C_UDP_PORT "23054" 

int main()
{
    
    int status; 
    int socket_server_C; 

    // address info of server C
    struct addrinfo hints_serverC, *res_serverC;
    
    // used to store the address of AWS 
    struct sockaddr_storage aws_addr;
    socklen_t addr_len;

    memset(&hints_serverC, 0, sizeof(hints_serverC));  // making sure the struct is empty 
    hints_serverC.ai_family = AF_UNSPEC;               // compatible with both IPv4 and IPv6 
    hints_serverC.ai_socktype = SOCK_DGRAM;            // UDP

    // get address info for serverC to hear on the port SERVER_C_UDP_PORT
    if ((status = getaddrinfo("127.0.0.1", SERVER_C_UDP_PORT, &hints_serverC, &res_serverC)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
  
    //create a datagram socket using serverC info 
    if((socket_server_C = socket(res_serverC->ai_family, res_serverC->ai_socktype, res_serverC->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    //bind to the datagram socket using serverC info 
    if(bind(socket_server_C, res_serverC->ai_addr, res_serverC->ai_addrlen) < 0)
    {
        perror("bind error");
        return 0;
    }

    cout << "server C is up and running using UDP on port<" << SERVER_C_UDP_PORT << ">" << endl;
    
    while(1)
    {
    
        struct data_info C_recv; 

        addr_len = sizeof(aws_addr); 
        int recieve_check = recvfrom(socket_server_C, &C_recv, sizeof(C_recv), 0 , (struct sockaddr *)&aws_addr, &addr_len);

        cout << "The Server C recieved link information of link <" << C_recv.link_id << ">, file size <" 
        << C_recv.size << ">, and signal power <" << C_recv.power << ">" << endl; 

        double t_prop = (C_recv.length / C_recv.velocity) / 10; 
        double t_trans = (C_recv.size) / (C_recv.bandwidth * 1000000);

        struct data_info C_send; 

        C_send.link_id = C_recv.link_id;
        C_send.size = C_recv.size;
        C_send.power = C_recv.power;
        C_send.bandwidth = C_recv.bandwidth;
        C_send.length = C_recv.length;
        C_send.velocity = C_recv.velocity;
        C_send.noise = C_recv.noise;
        C_send.prop = t_prop; 
        C_send.trans = t_trans;  

        cout << "The server C finished the calculation for <" << C_recv.link_id << ">" << endl; 
        
        int send_check = sendto(socket_server_C, &C_send, sizeof(C_send), 0, (struct sockaddr*)&aws_addr , addr_len);
       
        cout << "The Server C finished sending the output to AWS" << endl;

    }  

    return 0;
}