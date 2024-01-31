#include <iostream>
// include additional necessary headers
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// 2. Create necessary synchronization primitive(s)
mutex m;               // for locking critical sections
condition_variable CV; // CV to manage thread sync
bool ACK_flag = false; // false = not ready for ACK, true = ready for ACK


// 5. Update the "query" function to synchronize the output
void query(int count)                           // Should print: the print number (starting from 0), "SYN", and the three dots "..."
{
    unique_lock<mutex> lck(m);                  // lock
    for (int i = 0; i < count; ++i)             // itr "count" times as specified by user
    {
        cout << "[" << i << "] SYN ... ";       // Print the SYN message
        ACK_flag = true;                        // Signal the other thread that it can proceed with ACK
        CV.notify_one();                        // Notify the 'response' thread
        CV.wait(lck, []{ return !ACK_flag; });  // Wait until the ACK is printed
    }
}

// 5. Update the "response" function to synchronize the output
void response(int count)                        // Should print "ACK"
{
    unique_lock<mutex> lck(m);                  // lock
    for (int i = 0; i < count; ++i)             // itr "count" times as specified by user
    {
        CV.wait(lck, []{ return ACK_flag; });   // Wait until 'query' thread prints SYN and signals
        cout << "ACK" << endl;                  // Print the ACK message
        ACK_flag = false;                       // Indicate that ACK has been printed and it's time for the next SYN
        CV.notify_one();                        // Notify the 'query' thread
    }
}

int main(int argc, char **argv)
{
    // 1. Get the "count" from cmd args
    if (argc < 2)   // check if count arg is missing
    {
        // use error handling to print usage info rather than printf
        cerr << "Usage: " << argv[0] << " <count>\n";
        exit(EXIT_FAILURE);
    }

    int count = stoi(argv[1]); // convert cmd arg[1] to int

    // 3. Create two threads, one for "SYN" and the other for "ACK"
    // 4. Provide the threads with necessary args
    thread SYN_thread(query, count);    // query as callable function and count as arg
    thread ACK_thread(response, count); // response as callable function and count as arg

    // 4a. Thread sync
    SYN_thread.join();  // wait for SYN thread to finish
    ACK_thread.join();  // wait for ACK thread to finish

    return 0;
}