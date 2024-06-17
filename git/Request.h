/**
 * @class Request
 * @brief Represents a web request with IP addresses and processing time.
 */

#include <iostream>

#ifndef REQUEST_H
#define REQUEST_H

using namespace std;

/**
 * @class Request
 * @brief Represents a web request with IP addresses and processing time.
 */
class Request
{
public:
    std::string ipIn;  /**< Incoming IP address. */
    std::string ipOut; /**< Outgoing IP address. */
    int time;          /**< Processing time for the request. */

    /**
     * @brief Constructs a new Request object.
     * @param ipIn Incoming IP address.
     * @param ipOut Outgoing IP address.
     * @param time Processing time for the request.
     */
    Request(string ipIn, string ipOut, int time);
};

#endif // REQUEST_H