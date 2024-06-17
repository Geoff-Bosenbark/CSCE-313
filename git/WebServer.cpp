/**
 * @file WebServer.cpp
 * @brief Implementation of the WebServer class.
 */

#include <iostream>
#include "WebServer.h"

/**
 * @brief Constructs a new WebServer object.
 */
WebServer::WebServer() : isBusy(false), timeRemaining(0) {}

/**
 * @brief Processes a given request.
 * @param request The request to be processed.
 */
void WebServer::processRequest(Request request)
{
    isBusy = true;
    timeRemaining = request.time;
}

/**
 * @brief Updates the status of the web server.
 */
void WebServer::update()
{
    if (isBusy)
    {
        --timeRemaining;
        if (timeRemaining <= 0)
        {
            isBusy = false;
        }
    }
}
