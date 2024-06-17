/**
 * @file Request.cpp
 * @brief Implementation of the Request class.
 */

#include <iostream>
#include "Request.h"

/**
 * @brief Constructs a new Request object.
 * @param ipIn Incoming IP address.
 * @param ipOut Outgoing IP address.
 * @param time Processing time for the request.
 */
Request::Request(std::string ipIn, std::string ipOut, int time) : ipIn(ipIn), ipOut(ipOut), time(time) {}