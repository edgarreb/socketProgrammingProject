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

// port that serverB will be using (aws needs a copy of this port number)
#define SERVER_B_UDP_PORT "22054" 

int main()
{
    
    int status; 
    int socket_server_B; 

    // address info of server B
    struct addrinfo hints_serverB, *res_serverB;
    
    // used to store the address of AWS 
    struct sockaddr_storage aws_addr;
    socklen_t addr_len;

    memset(&hints_serverB, 0, sizeof(hints_serverB));  // making sure the struct is empty 
    hints_serverB.ai_family = AF_UNSPEC;               // compatible with both IPv4 and IPv6 
    hints_serverB.ai_socktype = SOCK_DGRAM;            // UDP

    // get address info for serverB to hear on the port SERVER_B_UDP_PORT
    if ((status = getaddrinfo("127.0.0.1", SERVER_B_UDP_PORT, &hints_serverB, &res_serverB)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }
  
    //create a datagram socket using serverB info 
    if((socket_server_B = socket(res_serverB->ai_family, res_serverB->ai_socktype, res_serverB->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    //bind to the datagram socket using serverB info 
    if(bind(socket_server_B, res_serverB->ai_addr, res_serverB->ai_addrlen) < 0)
    {
        perror("bind error");
        return 0;
    }

    cout << "server B is up and running using UDP on port<" << SERVER_B_UDP_PORT << ">" << endl;
    
    while(1)
    {
        
        struct data_info B_recv; 
        addr_len = sizeof(aws_addr); 
        int recieve_check = recvfrom(socket_server_B, &B_recv, sizeof(B_recv), 0 , (struct sockaddr *)&aws_addr, &addr_len);
        cout << "serverB recieved link id: " << B_recv.link_id << endl; 
        cout << "number of bytes recieved from aws: " << recieve_check << endl; 

        cout << "The Server B recieved input <" << B_recv.link_id << ">" << endl; 

        struct data_info B_send; 

        B_send.link_id = B_recv.link_id;
        B_send.size = B_recv.size;
        B_send.power = B_recv.power;
        B_send.m = 0;

        // search the database 

        ifstream myFile; 
        
        myFile.open("database_b.csv"); 

        while(myFile.good())
        {

            string line; 
            getline(myFile, line, ','); 

            int compare = stoi(line);

            if(compare == B_recv.link_id)
            {
                
                cout << compare << "=" << B_recv.link_id << endl;

                B_send.m = 1; 

                getline(myFile, line, ',');
                B_send.bandwidth = stod(line); 

                getline(myFile, line, ',');
                B_send.length = stod(line);

                getline(myFile, line, ',');
                B_send.velocity = stod(line); 

                getline(myFile, line, '\n');
                B_send.noise = stod(line);

                break; 
            }

            else
            {
                getline(myFile, line, '\n');
            }

        }

        myFile.close(); 

        cout << "The Server B has found <" << B_send.m << "> match" << endl;

        // cast to aws_addr to sockaddr *
        sendto(socket_server_B, &B_send, sizeof(B_send), 0, (struct sockaddr*)&aws_addr , addr_len);

        cout << "The Server B finished sending the output to AWS" << endl;
        
    }
        
    return 0;
}