/***
 * Timing updates from Thomas Cousins Spring 2023
 */

#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <ctime>
#include <random>

using namespace std;

// Total number of books available (the shared resource)
int books_available = 100;

// Mutex for locking
mutex mut;

void print_checkout(int i, int quantity)
{
    printf("Student %d checked out %d books\n", i, quantity);
}

void print_checkin(int i, int quantity)
{
    printf("Student %d checked in %d books\n", i, quantity);
}

// Conditional variable for the allocating threads to wait upon
condition_variable checkout_ready;

/* TODO:
    Use mut and checkout_ready for synchronization, and call print_checkout() while owning lock
*/
void books_checkout(int threadId, int quantity)
{
    unique_lock<mutex> lck(mut); // set lock
    checkout_ready.wait(lck, [quantity]() { return quantity <= books_available; }); // predicate: are there enough books available?
    books_available -= quantity;        // decrement available books by quantity just checked out
    print_checkout(threadId, quantity); // print checkout info
}

/* TODO:
    Use mut and checkout_ready for synchronization, and call print_checkin() while owning lock
*/
void books_checkin(int threadId, int quantity)
{
    lock_guard<mutex> lck(mut);         // set lock; lock_guard instead of unique because no wait
    books_available += quantity;        // increment available books by quantity that is checked in
    print_checkin(threadId, quantity);  // print checkin info
    checkout_ready.notify_all();        // notify all threads that are waiting for books
}

/**
 * thread function that takes in a thread id and the number of books to checkout and return.
 * the manager function is in charge of checkouts and checkins of books for the threads
 * @param threadId thread id.
 * @param quantity number of books to be allocated.
 */
void manager(int threadId, int quantity)
{
    books_checkout(threadId, quantity);

    int64_t sleep_ms = (rand() % 2000) + 1;                 // sleep upto 2 seconds
    this_thread::sleep_for(chrono::milliseconds(sleep_ms)); // keep the books for some time

    books_checkin(threadId, quantity);
}
int main()
{
    // The number of books to checkout by eah thread
    int cart[6] = {90, 90, 40, 50, 30, 5};
    srand(time(NULL));

    // Creating an array of threads
    thread *student_threads = new thread[6];

    /* TODO: Create the six student threads that will use the function (manager) for a respective number of books (cart[i]) */
    for (int i = 0; i < 6; i++)
    {
        student_threads[i] = thread(manager, i, cart[i]);
    } 

    /* TODO: join all the threads before exiting */
    for (int i = 0; i < 6; i++)
    {
        student_threads[i].join();
    }

    delete[] student_threads;
    return 0;
}