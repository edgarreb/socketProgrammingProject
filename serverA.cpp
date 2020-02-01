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
#include <fstream>

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

// port that serverA will be using (aws needs a copy of this port number)
#define SERVER_A_UDP_PORT "21054" 

int main()
{
    
    int status; 
    int socket_server_A; 

    // address info of server A
    struct addrinfo hints_serverA, *res_serverA;
    
    // to store the address of AWS 
    struct sockaddr_storage aws_addr;
    socklen_t addr_len;

    memset(&hints_serverA, 0, sizeof(hints_serverA));  // making sure the struct is empty 
    hints_serverA.ai_family = AF_UNSPEC;               // compatible with both IPv4 and IPv6 
    hints_serverA.ai_socktype = SOCK_DGRAM;            // UDP

    // get address info for myself to hear on the port 
    if ((status = getaddrinfo("127.0.0.1", SERVER_A_UDP_PORT, &hints_serverA, &res_serverA)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
  
    //create a datagram socket
    if((socket_server_A = socket(res_serverA->ai_family, res_serverA->ai_socktype, res_serverA->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    //bind to the datagram socket 
    if(bind(socket_server_A, res_serverA->ai_addr, res_serverA->ai_addrlen) < 0)
    {
        perror("bind error");
        return 0;
    }

    cout << "The Server A is up and running using UDP on port<" << SERVER_A_UDP_PORT << ">" << endl;
    
    while(1)
    {
        
        struct data_info A_recv;
        addr_len = sizeof(aws_addr); 
        
        int recieve_check = recvfrom(socket_server_A, &A_recv, sizeof(A_recv), 0 , (struct sockaddr *)&aws_addr, &addr_len);
        cout << "server A recieved link id: " << A_recv.link_id << endl; 
        cout << "number of bytes recieved from aws: " << recieve_check << endl; 

        cout << "The Server A recieved input <" << A_recv.link_id << ">" << endl; 

        struct data_info A_send; 

        A_send.link_id = A_recv.link_id;
        A_send.size = A_recv.size;
        A_send.power = A_recv.power;
        A_send.m = 0; 

        // search the database 

        ifstream myFile; 
        
        myFile.open("database_a.csv"); 

        while(myFile.good())
        {

            string line; 
            getline(myFile, line, ','); 

            int compare = stoi(line); 

            if(compare == A_recv.link_id)
            {
                
                cout << compare << "=" << A_recv.link_id << endl;

                A_send.m = 1; 

                getline(myFile, line, ',');
                A_send.bandwidth = stod(line); 

                getline(myFile, line, ',');
                A_send.length = stod(line);

                getline(myFile, line, ',');
                A_send.velocity = stod(line); 

                getline(myFile, line, '\n');
                A_send.noise = stod(line);

                break; 
            }

            else
            {
                getline(myFile, line, '\n');
                line.clear(); 
            }

        }

        myFile.close(); 

        cout << "The Server A has found <" << A_send.m << "> match" << endl; 

        sendto(socket_server_A, &A_send, sizeof(A_send), 0, (struct sockaddr*)&aws_addr , addr_len);
        
        cout << "The Server A finished sending the output to AWS" << endl; 

    }

    return 0;
}