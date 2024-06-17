/**
 * @class LoadBalancer
 * @brief Manages web servers and a queue of requests.
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include <queue>
#include <vector>
#include <fstream>
#include "WebServer.h"
#include "Request.h"

/**
 * @class LoadBalancer
 * @brief Manages web servers and a queue of requests.
 */
class LoadBalancer
{
public:
    /**
     * @brief Constructs a new LoadBalancer object with a specified number of servers.
     * @param numServers Number of web servers to manage.
     */
    LoadBalancer(int numServers);

    /**
     * @brief Initializes the request queue with a specified number of requests.
     * @param initialQueueSize Number of initial requests.
     */
    void initializeRequests(int initialQueueSize);

    /**
     * @brief Processes requests in the queue.
     * @param currentTime The current time in clock cycles.
     * @param logFile The log file stream to write logs.
     */
    void processRequests(int currentTime, std::ofstream &logFile);

    /**
     * @brief Logs the final results of the simulation.
     * @param logFile The log file stream to write logs.
     */
    void logResults(std::ofstream &logFile);

    /**
     * @brief Logs the range for task times.
     * @param logFile The log file stream to write logs.
     */
    void logTaskTimeRange(std::ofstream &logFile);

    /**
     * @brief Get the current queue size.
     * @return The current queue size.
     */
    int getQueueSize() const;

    /**
     * @brief Get the minimum task time.
     * @return The minimum task time.
     */
    int getMinTaskTime() const;

    /**
     * @brief Get the maximum task time.
     * @return The maximum task time.
     */
    int getMaxTaskTime() const;

    /**
     * @brief Destructor to clean up resources.
     */
    ~LoadBalancer();

private:
    std::vector<WebServer> webServers; /**< List of web servers. */
    std::queue<Request> requestQueue;  /**< Queue of requests. */
    int minTaskTime;                   /**< Minimum task time. */
    int maxTaskTime;                   /**< Maximum task time. */

    /**
     * @brief Adds a new request to the queue at random times.
     * @param currentTime The current time in clock cycles.
     * @param logFile The log file stream to write logs.
     */
    void addNewRequest(int currentTime, std::ofstream &logFile);
};

#endif // LOADBALANCER_H
