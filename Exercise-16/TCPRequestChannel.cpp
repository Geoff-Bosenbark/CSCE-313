#include "TCPRequestChannel.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>

using namespace std;

TCPRequestChannel::TCPRequestChannel(const std::string _ip_address, const std::string _port_no)
{
    /*
    Suggested implementation approach:
       - Use structures in the call to bind(...)
       - This struct has members for specifying the port and addresses which you must bind your socket to
       - Must convert the IP address to the network address structure that can be used within the struct to connect to the server

       System calls:
       - int inet_pton(int af, const char *restrict src, void* restrict dst);
       - af is address famil (AF_INET is the address family for IPV4)
       - src is the IP address of the server passed in a string (think a.b.c.d type string address in dotted decimal notation)
       - dst is the in_addr structure (network address structure) that inet_pton converts src to. Upon successful execution of the system call, dst holds the binary representation of the IP address
       - When creating struct, for server's call to bind, you can specify INADDR_ANY for the in_addr number. This allows the server to bind to all available addresses.

    if (server)
    {
        create socket on specified port, specifying socket requirements: domain, type, protocol
        bind the socket to address, sets up listening on port for any connections to the service
        mark the socket as listening (Listen)
    }

    if (client)
    {
        create socket on specified port, specifying socket requirements: domain, type, protocol
        connect to ip address of server
    }
    */

    // initialize addrinfo struct
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // set family to IPv4
    hints.ai_socktype = SOCK_STREAM; // set socket type to TCP

    if (_ip_address.empty()) // Server setup
    {
        hints.ai_flags = AI_PASSIVE;                                                  // Use my IP for binding
        getaddrinfo(NULL, _port_no.c_str(), &hints, &result);                         // get addr info for any addr and specified port
        sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // create the socket
        bind(sockfd, result->ai_addr, result->ai_addrlen);                            // bind the socket w/ addr and port
        listen(sockfd, 10);                                                           // Listen for up to 10 incoming connections
    }
    else // Client setup
    {
        getaddrinfo(_ip_address.c_str(), _port_no.c_str(), &hints, &result);          // get addr info from specified IP address and port
        sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // create the socket
        connect(sockfd, result->ai_addr, result->ai_addrlen);                         // connect the socket to addr and specified port
    }

    freeaddrinfo(result); // Free memory allocated for addr info
}

TCPRequestChannel::TCPRequestChannel(int _sockfd) : sockfd(_sockfd) {}

TCPRequestChannel::~TCPRequestChannel()
{
    // close sockfd
    close(sockfd);
}

int TCPRequestChannel::accept_conn()
{
    // struct sockaddr_storage
    // implementing accept(...) - return value is the sockfd of the client
    struct sockaddr_storage connector_addr; // Connector's address information
    socklen_t addr_size = sizeof connector_addr;
    int new_fd = accept(sockfd, (struct sockaddr *)&connector_addr, &addr_size); // accept and store
    return new_fd;                                                               // return fd for socket in the newly accepted connection
}

int TCPRequestChannel::cread(void *msgbuf, int msgsize)
{
    return recv(sockfd, msgbuf, msgsize, 0);
}

int TCPRequestChannel::cwrite(void *msgbuf, int msgsize)
{
    return send(sockfd, msgbuf, msgsize, 0);
}
