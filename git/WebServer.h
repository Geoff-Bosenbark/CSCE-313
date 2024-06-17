/**
 * @class WebServer
 * @brief Represents a web server that processes requests.
 */

#include <iostream>
#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @class WebServer
 * @brief Represents a web server that processes requests.
 */
class WebServer
{
public:
    bool isBusy;       /**< Indicates if the server is busy. */
    int timeRemaining; /**< Time remaining to complete the current request. */

    /**
     * @brief Constructs a new WebServer object.
     */
    WebServer();

    /**
     * @brief Processes a given request.
     * @param request The request to be processed.
     */
    void processRequest(Request request);

    /**
     * @brief Updates the status of the web server.
     */
    void update();
};

#endif // WEBSERVER_H