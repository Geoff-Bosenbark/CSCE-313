/**
 * @file main.cpp
 * @brief Main program for the load balancer simulation.
 */

#include <iostream>
#include <fstream>
#include "WebServer.h"
#include "Request.h"
#include "LoadBalancer.h"

/**
 * @brief Main function to set up the number of servers and run the load balancer.
 * @return int Exit status.
 */
int main()
{
    int numServers = 10;        // Fixed number of servers
    int simulationTime = 10000; // Fixed simulation time

    std::ofstream logFile("loadbalancerlog.txt");
    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file." << std::endl;
        return 1;
    }

    LoadBalancer loadBalancer(numServers);
    loadBalancer.initializeRequests(numServers * 100);
    logFile << "Number of servers: " << numServers << std::endl;
    logFile << "Simulated Clock Cycles: " << simulationTime << std::endl;
    logFile << "Initial Queue Size: " << loadBalancer.getQueueSize() << "\n"
            << std::endl;

    for (int time = 0; time < simulationTime; ++time)
    {
        loadBalancer.processRequests(time, logFile);
    }

    loadBalancer.logResults(logFile);
    loadBalancer.logTaskTimeRange(logFile);

    std::cout << "\nSuccess!\nAnalytical data has been output to loadbalancerlog.txt in the current directory\n"
              << endl;

    logFile.close();
    return 0;
}
