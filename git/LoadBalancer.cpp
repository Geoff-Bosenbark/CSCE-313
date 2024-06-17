/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer class.
 */

#include "LoadBalancer.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>

/**
 * @brief Constructs a new LoadBalancer object with a specified number of servers.
 * @param numServers Number of web servers to manage.
 */
LoadBalancer::LoadBalancer(int numServers) : minTaskTime(INT_MAX), maxTaskTime(INT_MIN)
{
    for (int i = 0; i < numServers; ++i)
    {
        webServers.push_back(WebServer());
    }
    std::srand(std::time(nullptr));
}

/**
 * @brief Initializes the request queue with a specified number of requests.
 * @param initialQueueSize Number of initial requests.
 */
void LoadBalancer::initializeRequests(int initialQueueSize)
{
    for (int i = 0; i < initialQueueSize; ++i)
    {
        std::string ipIn = "192.168.1." + std::to_string(std::rand() % 255 + 1);
        std::string ipOut = "10.0.0." + std::to_string(std::rand() % 255 + 1);
        int time = std::rand() % 100 + 1;
        requestQueue.push(Request(ipIn, ipOut, time));

        if (time < minTaskTime)
            minTaskTime = time;
        if (time > maxTaskTime)
            maxTaskTime = time;
    }
}

/**
 * @brief Processes requests in the queue.
 * @param currentTime The current time in clock cycles.
 * @param logFile The log file stream to write logs.
 */
void LoadBalancer::processRequests(int currentTime, std::ofstream &logFile)
{
    int processedRequests = 0;

    for (auto &server : webServers)
    {
        if (!server.isBusy && !requestQueue.empty())
        {
            server.processRequest(requestQueue.front());
            logFile << "Server " << (&server - &webServers[0]) << " is processing request from "
                    << requestQueue.front().ipIn << " to " << requestQueue.front().ipOut
                    << " for " << requestQueue.front().time << " cycles." << std::endl;
            requestQueue.pop();
            processedRequests++;
        }
        server.update();
    }

    logFile << "Time " << currentTime << ": Processed " << processedRequests << " requests, "
            << "Queue size: " << requestQueue.size() << std::endl;

    addNewRequest(currentTime, logFile);
}

/**
 * @brief Adds a new request to the queue at random times.
 * @param currentTime The current time in clock cycles.
 * @param logFile The log file stream to write logs.
 */
void LoadBalancer::addNewRequest(int currentTime, std::ofstream &logFile)
{
    if (std::rand() % 10 < 1)
    { // 10% chance to add a new request
        std::string ipIn = "192.168.1." + std::to_string(std::rand() % 255 + 1);
        std::string ipOut = "10.0.0." + std::to_string(std::rand() % 255 + 1);
        int time = std::rand() % 100 + 1;
        requestQueue.push(Request(ipIn, ipOut, time));
        logFile << "New request added at time " << currentTime << ": from "
                << ipIn << " to " << ipOut << " for " << time << " cycles." << std::endl;

        if (time < minTaskTime)
            minTaskTime = time;
        if (time > maxTaskTime)
            maxTaskTime = time;
    }
}

/**
 * @brief Logs the final results of the simulation.
 * @param logFile The log file stream to write logs.
 */
void LoadBalancer::logResults(std::ofstream &logFile)
{
    logFile << "\nFinal Queue Size: " << requestQueue.size() << std::endl;
}

/**
 * @brief Logs the range for task times.
 * @param logFile The log file stream to write logs.
 */
void LoadBalancer::logTaskTimeRange(std::ofstream &logFile)
{
    logFile << "Task Time Range: " << minTaskTime << " - " << maxTaskTime << " cycles" << std::endl;
}

/**
 * @brief Get the current queue size.
 * @return The current queue size.
 */
int LoadBalancer::getQueueSize() const
{
    return requestQueue.size();
}

/**
 * @brief Get the minimum task time.
 * @return The minimum task time.
 */
int LoadBalancer::getMinTaskTime() const
{
    return minTaskTime;
}

/**
 * @brief Get the maximum task time.
 * @return The maximum task time.
 */
int LoadBalancer::getMaxTaskTime() const
{
    return maxTaskTime;
}

/**
 * @brief Destructor to clean up resources.
 */
LoadBalancer::~LoadBalancer()
{
    // Log file is handled in main.cpp
}
