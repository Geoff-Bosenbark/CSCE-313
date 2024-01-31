/*
	Original author of the starter code
	Tanzir Ahmed
	Department of Computer Science & Engineering
	Texas A&M University
	Date: 2/8/20

	Please include your Name, UIN, and the date below
	Name: Geoffrey Bosenbark
	UIN: 232002930
	Date: 09/05/2023
*/
#include "common.h"
#include "FIFORequestChannel.h"
#include <chrono>	  // for time keeping functions
#include <sys/wait.h> // for "waitpid()"

using namespace std;

int main(int argc, char *argv[])
{
	int opt;
	int p = 0;
	double t = 0.0;
	int e = 0;
	int bufferSize = MAX_MESSAGE;
	// dynamic buffer size
	char *buf = nullptr;

	// for datapoint conditional
	bool p_flag = false;
	bool t_flag = false;
	bool e_flag = false;
	bool c_flag = false;

	// vector to hold channels
	// allows functions to be ran by latest pushed channel
	vector<FIFORequestChannel *> channels;

	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1)
	{
		switch (opt)
		{
		case 'p': // person
			p = atoi(optarg);
			p_flag = true;
			break;
		case 't': // time
			t = atof(optarg);
			t_flag = true;
			break;
		case 'e': // ecg
			e = atoi(optarg);
			e_flag = true;
			break;
		case 'f': // file
			filename = optarg;
			break;
		case 'm': // buffer size
			bufferSize = atoi(optarg);
			break;
		case 'c': // new channel
			c_flag = true;
			break;
		}
	}

	/* STEP 1
	Run the server as a child process: (15 pts)
		Run the server process as a child of the client process using fork() and exec(...) such that you do not need two terminals.
		The outcome is that you open a single terminal, run the client which first runs the server and then connects to it.
		To make sure that the server does not keep running after the client dies, send a QUIT_MSG to the server for each open channel and call the wait(...) function to wait for its end.
	*/
	/*
	Outline for order of operations:
		1. fork a new process. If this is successful, two processes will be running: a parent (client) and a child (server).
		2. The child process immediately runs the server-side binary and waits for communications. The parent process goes on to create a channel named "control" to communicate with the server.
		3. Once the channel is established, the client sends requests to the server over this channel, and the server responds.
	*/

	/*
	Requesting a New Channel: (15 pts)
		Ask the server to create a new channel by sending a special NEWCHANNEL_MSG request and joining that channel.
		Use the command format shown in the example above.
		After the channel is created, you need to process the request the user passed in through the CLI options.
	*/

	// create new process for server
	pid_t pid = fork();

	if (pid == 0) // child process
	{
		// Convert the bufferSize integer to a string and store it
		char buffer_arg[50];
		sprintf(buffer_arg, "%d", bufferSize);

		// create array for server executable and args
		const char *args[] = {"./server", "-m", buffer_arg, NULL};

		execvp(args[0], (char *const *)args);

		// If execvp fails
		perror("execvp failed");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) // fork failed
	{
		perror("fork failed");
		exit(EXIT_FAILURE);
	}
	else // parent process (client)
	{
		// dynamically create channel & push into channels vector
        FIFORequestChannel* control_chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
        channels.push_back(control_chan);

		// create new channel (so that it can be accessed outside of if block scope)
        FIFORequestChannel* new_channel = nullptr;

		// set by getopt -c argument
		if (c_flag == true)
		{
			MESSAGE_TYPE new_chan = NEWCHANNEL_MSG;
			// out << "Client: Sending NEWCHANNEL_MSG to server." << endl; // debug
			control_chan->cwrite(&new_chan, sizeof(MESSAGE_TYPE));
			// cout << "Client: Waiting for response from server for new channel name." << endl; // debug
			// create a variable to hold the new channel name from server
			char new_chan_name[MAX_MESSAGE];
			// cread the response from the server
			control_chan->cread(new_chan_name, MAX_MESSAGE);
			// cout << "Client: Received new channel name from server: " << new_chan_name << endl; // debug
			// call the FIFORequestChannel constructor dynamically with new_chan_name
            new_channel = new FIFORequestChannel(new_chan_name, FIFORequestChannel::CLIENT_SIDE);
            channels.push_back(new_channel);
		}

		// set chan as whichever channel was most recently pushed to back
        FIFORequestChannel* chan = channels.back();

		/* STEP 2
		Requesting Data Points: (15 pts)
			First, request one data point from the server and display it to stdout by running the client using the following command line format:
			$ ./client -p <patient no> -t <time in seconds> -e <ecg number>

			You must use the Linux function getopt(...) to collect the command line arguments.
			You cannot scan the input from the standard input using cin or scanf.
			After demonstrating one data point, request the first 1000 data points for a patient (both ecg1 and ecg2), collect the responses, and put them in a file called x1.csv.
			Compare the file against the corresponding data points in the original file and check that they match.
			For collecting the first 1000 data points of a given patient, use the following command line format: $ ./client -p <patient number>
		*/

		// example data point request
		if ((p_flag && t_flag && e_flag) == true)
		{
			buf = new char[bufferSize]; // Allocate memory dynamically
			datamsg x(p, t, e);			// replace hard-coded numbers with user values p, t, e

			memcpy(buf, &x, sizeof(datamsg));
			// cout << "requesting p, t, e data from server" << endl;	// debug
			chan->cwrite(buf, sizeof(datamsg));						// question
			// cout << "Sent data message request to server." << endl; // debug
			double reply;
			chan->cread(&reply, sizeof(double)); // answer
			// cout << "Received data message response from server: " << reply << endl; // debug
			std::cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;

			delete[] buf;
		}
		else if (p_flag == true)
		{
			// request 1000 data points
			// iterate over 1000 lines (columns t, e1, e2)
			//	send request for ecg1
			// 	send request for ecg2
			// write lines to received/x1.csv --> make sure that column format matches (time, e1, e2)

			// open/create x1 file
			ofstream outfile("received/x1.csv");
			if (!outfile.is_open())
			{
				cerr << "Failed to open x1.csv for writing." << endl;
				exit(EXIT_FAILURE);
			}

			// data points separated by 0.004 seconds
			double increment_time = 0.004;
			double current_time = 0;

			buf = new char[bufferSize]; // Allocate memory dynamically

			// iterate through first 1000 rows of data
			for (int i = 0; i < 1000; i++)
			{
				// char buf[MAX_MESSAGE];

				// request for ecg1 (column1)
				datamsg ecg1_request(p, current_time, 1);
				memcpy(buf, &ecg1_request, sizeof(datamsg));
				// cout << "Requesting line " << i << "'s data from server" << endl; // debug
				chan->cwrite(buf, sizeof(datamsg));
				double ecg1_value;
				chan->cread(&ecg1_value, sizeof(double));
				// cout << "Received ecg1 data: " << ecg1_value << " from server" << endl; // debug

				// request for ecg2 (column2)
				datamsg ecg2_request(p, current_time, 2);
				// cout << "Requesting line " << i << "'s data from server" << endl; // debug
				memcpy(buf, &ecg2_request, sizeof(datamsg));
				chan->cwrite(buf, sizeof(datamsg));
				double ecg2_value;
				chan->cread(&ecg2_value, sizeof(double));
				// cout << "Received ecg1 data: " << ecg2_value << " from server" << endl; // debug

				// write to the file
				outfile << current_time << "," << ecg1_value << "," << ecg2_value << endl;

				// increment time to move to next row in csv
				current_time += increment_time;
			}

			// close file when writing is done
			outfile.close();

			delete[] buf;
		}

		/* STEP 3
		Requesting Files: (35 points)
			Request a file from the server side using the following command format again using getopt(...) function: $ ./client -f <file name>

			The file does not need to be one of the .csv files currently existing in the BIMDC directory.
			You can put any file in the BIMDC/ directory and request it from the directory.

			The steps for requesting a file are as follows:
			1. Send a file message to get its length.
			2. Send a series of file messages to get the content of the file.
			3. Put the received file under the received/ directory with the same name as the original file.

			Note:
			Make sure to treat the file as binary because we will use this same program to transfer any type of file (not necessarily made of ASCII text).
			Putting the data in an STL string will not work because C++ strings are NULL terminated.
		*/

		// sending a file request message
		else if (!filename.empty())
		{
			// Initial request for the file length
			filemsg fm(0, 0);
			int len = sizeof(filemsg) + (filename.size() + 1);
			char *buf2 = new char[len];
			memcpy(buf2, &fm, sizeof(filemsg));
			strcpy(buf2 + sizeof(filemsg), filename.c_str());
			// cout << "Requesting file length from server" << endl; // debug
			chan->cwrite(buf2, len);

			delete[] buf2;

			// Reading the response for the file length
			char fileSizeBuffer[sizeof(__int64_t)];
			chan->cread(fileSizeBuffer, sizeof(__int64_t));
			// cout << "Recevied file length from server" << endl; // debug
			int fileSize = *reinterpret_cast<__int64_t *>(fileSizeBuffer);

			// Open the output file in binary
			ofstream outputFile("received/" + filename, ios::binary);

			__int64_t offset = 0;			   // starting offset
			int remainingBytes = fileSize;	   // bytes left to be transferred
			const int bufferCap = MAX_MESSAGE; // buffer capacity

			// Start timer before file transfer
			auto start = chrono::high_resolution_clock::now();

			// Iterate through data until all bytes transferred
			while (remainingBytes > 0)
			{
				// The length of data to request is the minimum of buffer capacity and remaining bytes.
				int length = min(bufferCap, remainingBytes);

				// Make a file transfer request for the next chunk
				filemsg fmsg(offset, length);
				int msgSize = sizeof(filemsg) + filename.size() + 1;
				char *buff = new char[msgSize];
				memcpy(buff, &fmsg, sizeof(filemsg));
				strcpy(buff + sizeof(filemsg), filename.c_str());
				// cout << "Requesting next chunk of data from server" << endl;
				chan->cwrite(buff, msgSize);

				// Memory management
				delete[] buff;

				char buffer[bufferCap];
				int bytesRead = chan->cread(buffer, length);
				// Write the received chunk to the output file
				outputFile.write(buffer, bytesRead);

				// Update remaining bytes and offset
				remainingBytes -= bytesRead;
				offset += bytesRead;
			}

			// Close file after the entire file read
			outputFile.close();

			// Stop timer after file transfer completes
			auto end = chrono::high_resolution_clock::now();
			chrono::duration<double> diff = end - start;
			std::cout << "File transfer took: " << diff.count() << " seconds." << endl;

			/*
			Closing Channels: (5 pts)
				You must also ensure that there are NO open connections at the end and NO temporary files remaining in the directory either.
				The server should clean up these resources as long as you send QUIT_MSG at the end for the new channels created.
				The given client.cpp already does this for the control channel.
			*/
		}

		// closing the channels
        MESSAGE_TYPE m = QUIT_MSG;

        if (c_flag) 
		{
            // cout << "Client: Sending QUIT_MSG for the new channel." << endl; // debug
            new_channel->cwrite(&m, sizeof(MESSAGE_TYPE)); // Sending QUIT_MSG for the new channel
            delete new_channel;
        }

        usleep(100000); // sleep for 100ms
        // cout << "Client: Sending QUIT_MSG for the control channel." << endl; // debug
        chan->cwrite(&m, sizeof(MESSAGE_TYPE));
        delete control_chan;
	}

	return 0;
}

/*
	BREAKDOWN OF FILEMSG REQUEST

	filemsg fmsg(offset, length);
		Creating an instance of a filemsg structure.
		This structure contains two pieces of information: an offset and a length.
		The purpose of this structure is to specify which part of a file you're interested in.
		The offset is the starting byte from which you want data, and length is the number of bytes you want from that starting position.

	int msgSize = sizeof(filemsg) + filename.size() + 1;
		Calculating the total size of the message you're going to send. The size includes:
		1. The size of the filemsg structure.
		2. The size of the filename string.
		3. An additional byte for the null-terminator (\0) of the string.

	char *buff = new char[msgSize];
		Dynamically allocating memory for a character buffer of size msgSize.
		This buffer contains both the filemsg data and the filename string.

	memcpy(buff, &fmsg, sizeof(filemsg));
		Copying the data from the filemsg structure into the beginning of the buff memory.
		Uses the memcpy function, which takes three arguments:
		The destination (where to copy to).
		The source (where to copy from).
		How many bytes to copy.

	strcpy(buff + sizeof(filemsg), filename.c_str());
		This line copies the filename string into the buffer immediately after the filemsg data.
		buff + sizeof(filemsg) calculates the position right after the filemsg in the buffer.
		filename.c_str() returns a pointer to the underlying character array of the filename string.
		strcpy copies the string, including its null-terminator.

	chan->cwrite(buff, msgSize);
		Sending the buffer (which now contains the filemsg data followed by the filename) to some communication channel represented by chan->
		The cwrite method = "channel write". Specifying the buffer buffer 'buff' and its size 'msgSize' as arguments.
*/