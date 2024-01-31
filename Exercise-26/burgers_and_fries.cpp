#include <iostream>
#include <thread>
#include <semaphore.h>
#include <mutex>
#include <unistd.h>
using namespace std;
#define MAX_THREADS 100

#define BURGER 0
#define FRIES 1
const char *type_names[] = {"BURGER", "FRIES"};
#define pii pair<int, int>

int k;

int order_count = 0;            // track order count
int previous_order_type = -1;   // for comparing previous orders
int waiting_count[2] = {0, 0};  // waiting counts for BURGERS and FRIES

sem_t sem_burger, sem_fries;    // semaphore for burgers and fries
mutex order_mutex;              // mutex for locking orders

// Do not change
void process_order()
{
    sleep(2);
}

void place_order(int type)
{
    /**
     *  Add logic for synchronization before order processing
     *  Check if already k orders in process;
     *     if true -> wait (print waiting)
     *     otherwise place this order (print order)
     *  Use type_names[type] to print the order type
     */

    order_mutex.lock(); // Lock mutex

    if (order_count >= k)
    {
        // If max number of orders are being processed, increment waiting count and wait
        waiting_count[type]++;
        cout << "Waiting: " << type_names[type] << endl;
        order_mutex.unlock(); // Unlock to prevent deadlock

        // Wait for semaphore signal that a cook has become available
        if (type == BURGER)
        {
            sem_wait(&sem_burger);
        }
        else
        {
            sem_wait(&sem_fries);
        }

        order_mutex.lock();    // Re-lock mutex to safely modify after wait
        waiting_count[type]--; // Decrement waiting count since order is no longer waiting
    }

    order_count++; // Increment order count to process the next order
    order_mutex.unlock(); // Unlock after updating
    cout << "Order: " << type_names[type] << endl;  // Print that the order is being processed

    process_order();      // Do not remove, simulates preparation

    /*
     *  Add logic for synchronization after order processed
     *  Allow next order of the same type to proceed if there is any waiting; if not, allow the other type to proceed.
     */

    order_mutex.lock(); // Lock to update after order processing
    order_count--;      // Decrement order count since order is finished

    // Check if there are any waiting orders of the same type
    if (waiting_count[type] > 0)
    {
        // If there are, post to the same type semaphore
        sem_post(type == BURGER ? &sem_burger : &sem_fries);
    }
    else if (waiting_count[1 - type] > 0)
    {
        // If there are waiting orders of the other type, post to the other type semaphore
        sem_post(type == BURGER ? &sem_fries : &sem_burger);
    }
    else
    {
        // If there are no waiting orders, update the previous order type
        previous_order_type = type;
    }

    order_mutex.unlock(); // Unlock after updating
}

int main()
{
    // Initialize necessary variables, semaphores etc.
    sem_init(&sem_burger, 0, 0);
    sem_init(&sem_fries, 0, 0);

    // Read data: done for you, do not change
    pii incoming[MAX_THREADS];
    int _type, _arrival;
    int t;
    cin >> k;
    cin >> t;
    for (int i = 0; i < t; ++i)
    {
        cin >> _type >> _arrival;
        incoming[i].first = _type;
        incoming[i].second = _arrival;
    }

    // Create threads: done for you, do not change
    thread *threads[MAX_THREADS];
    for (int i = 0; i < t; ++i)
    {
        _type = incoming[i].first;
        threads[i] = new thread(place_order, _type);
        if (i < t - 1)
        {
            int _sleep = incoming[i + 1].second - incoming[i].second;
            sleep(_sleep);
        }
    }

    // Join threads: done for you, do not change
    for (int i = 0; i < t; ++i)
    {
        threads[i]->join();
        delete threads[i];
    }

    // destroy semaphores
    sem_destroy(&sem_burger);
    sem_destroy(&sem_fries);

    return 0;
}