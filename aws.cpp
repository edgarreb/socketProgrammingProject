#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

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

// this is the port that the server will be connecting on
#define CLIENT_TCP_PORT "25054"
#define MONITOR_TCP_PORT "26054"
#define SERVER_UDP_PORT "24054"
#define SERVER_A_UDP_PORT "21054"
#define SERVER_B_UDP_PORT "22054"
#define SERVER_C_UDP_PORT "23054"

int main(void)
{
    int status;

    // we are going to listen on parent sockets and use child sockets to handle request
    int client_parent_socket;
    int client_child_socket;

    int monitor_parent_socket;
    int monitor_child_socket;

    // sockets used for UDP connections
    int serverA_socket;
    int serverB_socket;
    int serverC_socket;

    // *res_client holds the information of the client
    struct addrinfo hints_client, *res_client;
    struct sockaddr_storage client_addr;
    socklen_t Ctaddr_len;

    // *monitor_TCP holds the information of the monitor
    struct addrinfo hints_monitor, *res_monitor;
    struct sockaddr_storage monitor_addr;
    socklen_t Maddr_len;

    // *res_serverA hold information for server A
    struct addrinfo hints_serverA, *res_serverA;
    struct sockaddr_storage serverA_addr;
    socklen_t Aaddr_len;

    // *res_serverB hold information for server B
    struct addrinfo hints_serverB, *res_serverB;
    struct sockaddr_storage serverB_addr;
    socklen_t Baddr_len;

    // *res_serverC hold information for server C
    struct addrinfo hints_serverC, *res_serverC;
    struct sockaddr_storage serverC_addr;
    socklen_t Caddr_len;


    // ---------------------------------------------------------------


    // fill in the elements of hints_client
    memset(&hints_client, 0, sizeof (hints_client));     // make sure that the struct is empty
    hints_client.ai_family = AF_UNSPEC;                  // compatible with both IPv4 or IPv6
    hints_client.ai_socktype = SOCK_STREAM;              // TCP stream socket

     // fill in the elements of hints_monitor
    memset(&hints_monitor, 0, sizeof (hints_monitor));   // make sure that the struct is empty
    hints_monitor.ai_family = AF_UNSPEC;                 // compatible with both IPv4 or IPv6
    hints_monitor.ai_socktype = SOCK_STREAM;             // TCP stream socket */

    // fill in the elements of hints_serverA
    memset(&hints_serverA, 0, sizeof (hints_serverA));     // make sure that the struct is empty
    hints_serverA.ai_family = AF_UNSPEC;                   // compatible with both IPv4 or IPv6
    hints_serverA.ai_socktype = SOCK_DGRAM;                // UDP socket

    // fill in the elements of hints_serverB
    memset(&hints_serverB, 0, sizeof (hints_serverB));     // make sure that the struct is empty
    hints_serverB.ai_family = AF_UNSPEC;                   // compatible with both IPv4 or IPv6
    hints_serverB.ai_socktype = SOCK_DGRAM;                // UDP socket

    // fill in the elements of hints_serverC
    memset(&hints_serverC, 0, sizeof (hints_serverC));     // make sure that the struct is empty
    hints_serverC.ai_family = AF_UNSPEC;                   // compatible with both IPv4 or IPv6
    hints_serverC.ai_socktype = SOCK_DGRAM;                // UDP socket


    // ---------------------------------------------------------------


    // get information about the client
    if ((status = getaddrinfo("127.0.0.1", CLIENT_TCP_PORT, &hints_client, &res_client)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // get information about the monitor
    if ((status = getaddrinfo("127.0.0.1", MONITOR_TCP_PORT, &hints_monitor, &res_monitor)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // get information about server A
    if ((status = getaddrinfo("127.0.0.1", SERVER_A_UDP_PORT, &hints_serverA, &res_serverA)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // get information about server B
    if ((status = getaddrinfo("127.0.0.1", SERVER_B_UDP_PORT, &hints_serverB, &res_serverB)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    // get information about server C
    if ((status = getaddrinfo("127.0.0.1", SERVER_C_UDP_PORT, &hints_serverC, &res_serverC)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }


    // ---------------------------------------------------------------


    // create a TCP socket for client
    if((client_parent_socket = socket(res_client->ai_family, res_client->ai_socktype, res_client->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    // bind to the client TCP socket
    if(bind(client_parent_socket, res_client->ai_addr, res_client->ai_addrlen) < 0)
    {
        perror("bind error");
        return 0;
    }

    // create a TCP socket for monitor
    if((monitor_parent_socket = socket(res_monitor->ai_family, res_monitor->ai_socktype, res_monitor->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    // bind to the monitor TCP socket
    if(bind(monitor_parent_socket, res_monitor->ai_addr, res_monitor->ai_addrlen) < 0)
    {
        perror("bind error");
        return 0;
    }

    // create a UDP socket with server A
    if((serverA_socket = socket(res_serverA->ai_family, res_serverA->ai_socktype, res_serverA->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    // create a UDP socket with server B
    if((serverB_socket = socket(res_serverB->ai_family, res_serverB->ai_socktype, res_serverB->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    // create a UDP socket with server C
    if((serverC_socket = socket(res_serverC->ai_family, res_serverC->ai_socktype, res_serverC->ai_protocol)) < 0)
    {
        perror("socket error");
        return 0;
    }

    // ---------------------------------------------------------------

    cout << "the AWS is up and running" << endl;

    while(1)
    {

        listen(client_parent_socket, 8);
        listen(monitor_parent_socket, 8);

        // recieve paramaters from client

        struct data_info client_recv;

        //create child socket to comunicate with the client
        Ctaddr_len = sizeof(client_addr);
        client_child_socket = accept(client_parent_socket, (struct sockaddr *)&client_addr, &Ctaddr_len);

        int check_client = recv(client_child_socket, &client_recv, sizeof(client_recv), 0);

        cout << "the AWS recieved link ID=<" << client_recv.link_id << ">, size=<" << client_recv.size << ">, and power=<" << client_recv.power <<
        "> from the client using TCP over port<" << CLIENT_TCP_PORT << ">" << endl;


        // ------------------------------------------------------------


        // sending link_id , size, power to monitor
        Maddr_len = sizeof(monitor_addr);
        monitor_child_socket = accept(monitor_parent_socket, (struct sockaddr *)&monitor_addr, &Maddr_len);

        //char ack_monitor[] = "Acknowledge monitor";
        struct data_info M_send;

        M_send.link_id = client_recv.link_id;
        M_send.size = client_recv.size;
        M_send.power = client_recv.power;

        int m_send = send(monitor_child_socket, &M_send, sizeof(M_send), 0);


        // ------------------------------------------------------------


        // sending link_id to serverA
        struct data_info A_send;
        A_send.link_id = client_recv.link_id;

        // send over serverA socket with destination serverA
        sendto(serverA_socket, &A_send, sizeof(A_send), 0, res_serverA->ai_addr, res_serverA->ai_addrlen);

        cout << "the AWS sent link ID=<" << client_recv.link_id << "> to backend server<A> using UDP over port <" << SERVER_A_UDP_PORT << ">" << endl;


        // ------------------------------------------------------------


        // sending link_id to serverB
        struct data_info B_send;
        B_send.link_id = client_recv.link_id;

        // send over serverB socket with destination serverB
        sendto(serverB_socket, &B_send, sizeof(B_send), 0, res_serverB->ai_addr, res_serverB->ai_addrlen);

        cout << "the AWS sent link ID=<" << client_recv.link_id << "> to backend server<B> using UDP over port <" << SERVER_B_UDP_PORT << ">" << endl;


        // ------------------------------------------------------------

        // recieve from serverA
        Aaddr_len = sizeof(serverA_addr);
        struct data_info A_recv;
        recvfrom(serverA_socket, &A_recv, sizeof(A_recv), 0 , (struct sockaddr *)&serverA_addr, &Aaddr_len);

        cout << "the AWS recieved <" << A_recv.m << "> matches from backend server<A> using UDP over port <" << SERVER_A_UDP_PORT << ">" << endl;


        // ------------------------------------------------------------

        // recieve from serverB
        Baddr_len = sizeof(serverB_addr);
        struct data_info B_recv;
        recvfrom(serverB_socket, &B_recv, sizeof(B_recv), 0 , (struct sockaddr *)&serverB_addr, &Baddr_len);

        cout << "the AWS recieved <" << B_recv.m << "> matches from backend server<B> using UDP over port <" << SERVER_B_UDP_PORT << ">" << endl;


        // ------------------------------------------------------------


        // send no matches found to client and monitor (if no matches were found)
        if( (A_recv.m == 0) && (B_recv.m == 0) )
        {
            int c_send = send(client_child_socket, &A_recv, sizeof(A_recv), 0);
            int m_send = send(monitor_child_socket, &A_recv, sizeof(A_recv), 0);
        }


        // ------------------------------------------------------------


        // send information to serverC
        struct data_info C_send;

        if(A_recv.m == 1)
        {
            C_send.link_id = A_recv.link_id;
            C_send.size = A_recv.size;
            C_send.power = A_recv.power;
            C_send.bandwidth = A_recv.bandwidth;
            C_send.length = A_recv.length;
            C_send.velocity = A_recv.velocity;
            C_send.noise = A_recv.noise;

            int sendC = sendto(serverC_socket, &C_send, sizeof(C_send), 0, res_serverC->ai_addr, res_serverC->ai_addrlen);

        }

        else if(B_recv.m == 1)
        {
            C_send.link_id = B_recv.link_id;
            C_send.size = B_recv.size;
            C_send.power = B_recv.power;
            C_send.bandwidth = B_recv.bandwidth;
            C_send.length = B_recv.length;
            C_send.velocity = B_recv.velocity;
            C_send.noise = B_recv.noise;

            int sendC = sendto(serverC_socket, &C_send, sizeof(C_send), 0, res_serverC->ai_addr, res_serverC->ai_addrlen);

        }

        cout << "The AWS sent link id <" << C_send.link_id << "> size <" << C_send.size << "> power<" << C_send.power << "> and link information to back-end server C"
        << endl;


        // ------------------------------------------------------------


        // recieve from C
        struct data_info C_recv;
        Caddr_len = sizeof(serverC_addr);
        int recvC = recvfrom(serverC_socket, &C_recv, sizeof(C_recv), 0 , (struct sockaddr *)&serverC_addr, &Caddr_len);


        // ------------------------------------------------------------


        //send compution results to client
        struct data_info client_send;
        send(client_child_socket, &client_send, sizeof(client_send), 0);


        // ------------------------------------------------------------


        // send to monitor
        send(monitor_child_socket, &C_recv, sizeof(C_recv), 0);
        
    }

    return 0;
}
