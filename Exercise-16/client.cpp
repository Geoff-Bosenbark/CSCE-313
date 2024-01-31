#include <fstream>
#include <iostream>
#include <thread>
#include <sys/time.h>
#include <sys/wait.h>

#include "BoundedBuffer.h"
#include "common.h"
#include "Histogram.h"
#include "HistogramCollection.h"
#include "TCPRequestChannel.h"

// ecgno to use for datamsgs
#define EGCNO 1

using namespace std;

// used in worker_thread and histogram_thread functions
struct patient_data
{
    int pno;
    double val;
};

void patient_thread_function(BoundedBuffer *request_buffer, int p, int n)
{
    datamsg data(p, 0.0, 1);
    for (int i = 0; i < n; i++)
    {
        request_buffer->push((char *)&data, sizeof(datamsg));
        data.seconds += 0.004;
    }
}

void file_thread_function(BoundedBuffer *request_buffer, string file_name, string server_ip, string port_no, int m)
{
    // If no file name is provided, return early.
    if (file_name.empty())
    {
        return;
    }

    // Initialize a TCPRequestChannel for this thread.
    TCPRequestChannel tcp_chan(server_ip, port_no);

    // Initialize the filemsg structure.
    filemsg set_fmsg(0, 0);

    // Use vector to initialize the buffer.
    vector<char> buff(sizeof(set_fmsg) + file_name.size() + 1);
    memcpy(buff.data(), &set_fmsg, sizeof(set_fmsg));          // Copy fmsg to the front of buff.
    strcpy(buff.data() + sizeof(set_fmsg), file_name.c_str()); // Copy file name into buff after fmsg.

    // Write buffer to tcp_chan.
    tcp_chan.cwrite(buff.data(), buff.size());

    // Read file size from tcp_chan.
    u_int64_t file_size;
    tcp_chan.cread(&file_size, sizeof(file_size));

    // Setting size of the file in the received directory.
    FILE *file_ptr = fopen(("received/" + file_name).c_str(), "w+");
    fseek(file_ptr, file_size, SEEK_SET); // Set file size.
    fclose(file_ptr);

    // Set up file pointer/offset.
    filemsg *fmsg = (filemsg *)buff.data();
    u_int64_t offset = file_size;

    // Send requests to read chunks until the entire file is read.
    while (offset > 0)
    {
        fmsg->length = min(offset, (u_int64_t)m);       // Determine size of the requested chunk.
        request_buffer->push(buff.data(), buff.size()); // Send chunk request to bounded buffer.
        fmsg->offset += fmsg->length;                   // Update offset in buffer for the next request.
        offset -= fmsg->length;                         // Reduce the offset by the size of bytes in the request to keep track of bytes remaining.
    }
}

void worker_thread_function(BoundedBuffer *request_buffer, BoundedBuffer *response_buffer, string server_ip, string port_no, int m)
{
    TCPRequestChannel chan(server_ip, port_no);

    // initialize buffers
    char write_buffer[MAX_MESSAGE];        // Buffer for cwrite; store requests popped from request_buffer; don't hardcode w/ MAX_MESSAGE
    vector<char> read_buffer(4096);        // Buffer for cread; used to receive responses from server

    // process requests until QUIT_MSG received
    while (true)
    {
        // initialize response and request variables
        double server_response = 0;                                        // used for DATA_MSG handling
        int request_size = request_buffer->pop(write_buffer, MAX_MESSAGE); // pop request from request_buffer and store size

        MESSAGE_TYPE *msg = (MESSAGE_TYPE *)write_buffer; // identify type of message in buffer
        datamsg *dmsg = (datamsg *)write_buffer;          // used for DATA_MSG handling

        chan.cwrite(write_buffer, request_size); // send msg from write_buffer to server

        // DATA_MSG handler
        if (*msg == DATA_MSG)
        {
            // read server's response to request
            chan.cread(&server_response, sizeof(double));

            // construct patient data using server response
            patient_data patient;
            patient.pno = dmsg->person;
            patient.val = server_response;

            // push constructed patient data to response_buffer
            response_buffer->push((char *)&patient, sizeof(patient_data));
        }

        // FILE_MSG handler
        else if (*msg == FILE_MSG)
        {
            // extract filemsg details
            filemsg *fmsg = (filemsg *)write_buffer; // extract fmsg details from buffer
            string file_name = (char *)(fmsg + 1);   // extract file name from fmsg

            // read server's response to request
            chan.cread(read_buffer.data(), m);

            // Update file
            FILE *file_ptr = fopen(("received/" + file_name).c_str(), "r+"); // Open specified file in update mode
            fseek(file_ptr, fmsg->offset, SEEK_SET);                         // Move file_ptr to specified offset
            fwrite(read_buffer.data(), 1, fmsg->length, file_ptr);           // Write received data to file
            fclose(file_ptr);                                                // Close the file
        }

        // QUIT_MSG handler
        else if (*msg == QUIT_MSG)
        {
            break;
        }
    }
}

void histogram_thread_function(BoundedBuffer *response_buffer, HistogramCollection *hc)
{
    // buffer to hold popped requests
    char pdata_buffer[sizeof(patient_data)];

    // Pop data from response_buffer until none left
    while (true)
    {
        // Retrieve data from response_buffer
        response_buffer->pop(pdata_buffer, sizeof(patient_data)); // pop data and store it in pdata_buffer
        patient_data patient = *(patient_data *)pdata_buffer;     // cast data as patient object

        // Exit case
        if (patient.pno < 0)
        {
            break;
        }

        // update histogram collection with data
        hc->update(patient.pno, patient.val);
    }
}

int main(int argc, char *argv[])
{
    int n = 1000;          // default number of requests per "patient"
    int p = 10;            // number of patients [1,15]
    int w = 100;           // default number of worker threads
    int h = 20;            // default number of histogram threads
    int b = 20;            // default capacity of the request buffer (should be changed)
    int m = MAX_MESSAGE;   // default capacity of the message buffer
    string f = "";         // name of file to be transferred
    string server_ip = ""; // default Server IP address
    string port_no = "";   // default Port number

    // read arguments
    int opt;
    while ((opt = getopt(argc, argv, "n:p:w:h:b:m:f:a:r:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            n = atoi(optarg);
            break;
        case 'p':
            p = atoi(optarg);
            break;
        case 'w':
            w = atoi(optarg);
            break;
        case 'h':
            h = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 'm':
            m = atoi(optarg);
            break;
        case 'f':
            f = optarg;
            break;
        case 'a':
            server_ip = optarg;
            break;
        case 'r':
            port_no = optarg;
            break;
        }
    }

    // initialize TCPRequestChannel with IP and port specified
    TCPRequestChannel *chan = new TCPRequestChannel(server_ip, port_no);
    BoundedBuffer request_buffer(b);
    BoundedBuffer response_buffer(b);
    HistogramCollection hc;

    // making histograms and adding to collection
    for (int i = 0; i < p; i++)
    {
        Histogram *h = new Histogram(10, -2.0, 2.0);
        hc.add(h);
    }

    // record start time
    struct timeval start, end;
    gettimeofday(&start, 0);

    /* create all threads here */
    vector<thread> patient_threads, worker_threads, histogram_threads;
    thread file_thread;
    bool file_transfer = (f != "");

    // create patient or file thread(s)
    if (!file_transfer)
    {
        for (int i = 0; i < p; i++)
        {
            // creates thread that executes patient_thread_function with specified args
            patient_threads.emplace_back(patient_thread_function, &request_buffer, i + 1, n);
        }
    }
    else // only one file thread
    {
        // creates thread that executes file_thread_function with specified args
        file_thread = thread(file_thread_function, &request_buffer, f, server_ip, port_no, m);
    }

    // Create worker threads
    for (int i = 0; i < w; i++)
    {
        worker_threads.emplace_back(worker_thread_function, &request_buffer, &response_buffer, server_ip, port_no, m);
    }

    // Create histogram threads
    for (int i = 0; i < h; i++)
    {
        // creates thread that executes histogram_thread_function with specified args
        histogram_threads.emplace_back(histogram_thread_function, &response_buffer, &hc);
    }

    /* join all threads here */
    // iterate over all thread arrays, calling join
    // order threads join is important. Precedence: (patient threads, worker threads, histogram threads)
    if (!file_transfer)
    {
        for (auto &thread : patient_threads)
        {
            thread.join(); // synchronize thread completion
        }
    }
    else
    {
        file_thread.join(); // synchronize thread completion
    }

    // Signal worker threads to quit
    MESSAGE_TYPE qmsg = QUIT_MSG;
    for (size_t i = 0; i < worker_threads.size(); i++)
    {
        request_buffer.push((char *)&qmsg, sizeof(MESSAGE_TYPE));
    }
    for (auto &thread : worker_threads)
    {
        thread.join(); // synchronize thread completion
    }

    // Signal histogram threads to quit
    patient_data terminatePair{-1, -1};
    for (size_t i = 0; i < histogram_threads.size(); i++)
    {
        response_buffer.push((char *)&terminatePair, sizeof(patient_data));
    }
    for (auto &thread : histogram_threads)
    {
        thread.join(); // synchronize thread completion
    }

    // record end time
    gettimeofday(&end, 0);

    // print the results
    if (!file_transfer)
    {
        hc.print();
    }
    int secs = ((1e6 * end.tv_sec - 1e6 * start.tv_sec) + (end.tv_usec - start.tv_usec)) / ((int)1e6);
    int usecs = (int)((1e6 * end.tv_sec - 1e6 * start.tv_sec) + (end.tv_usec - start.tv_usec)) % ((int)1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;

    // quit and close control channel
    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite((char *)&q, sizeof(MESSAGE_TYPE));
    cout << "All Done!" << endl;
    delete chan;
}