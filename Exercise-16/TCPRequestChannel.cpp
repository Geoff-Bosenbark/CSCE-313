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
