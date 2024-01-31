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
		FIFORequestChannel control_chan("control", FIFORequestChannel::CLIENT_SIDE);
		channels.push_back(&control_chan);

		FIFORequestChannel *new_channel = nullptr;

		// set by getopt -c argument
		if (c_flag == true)
		{
			MESSAGE_TYPE new_chan = NEWCHANNEL_MSG;
			cout << "Client: Sending NEWCHANNEL_MSG to server." << endl; // debug
			control_chan.cwrite(&new_chan, sizeof(MESSAGE_TYPE));
			cout << "Client: Waiting for response from server for new channel name." << endl; // debug
			// create a variable to hold the new channel name from server
			char new_chan_name[MAX_MESSAGE];
			// cread the response from the server
			control_chan.cread(new_chan_name, MAX_MESSAGE);
			std::cout << "Client: Received new channel name from server: " << new_chan_name << endl; // debug
			// call the FIFORequestChannel constructor dynamically with new_chan_name
			new_channel = new FIFORequestChannel(new_chan_name, FIFORequestChannel::CLIENT_SIDE);
			channels.push_back(new_channel);
		}

		// set chan as whichever channel was most recently pushed to back
		FIFORequestChannel chan = *(channels.back());

		// example data point request
		if ((p_flag && t_flag && e_flag) == true)
		{
			buf = new char[bufferSize]; // Allocate memory dynamically
			datamsg x(p, t, e);			// replace hard-coded numbers with user values p, t, e

			memcpy(buf, &x, sizeof(datamsg));
			cout << "requesting p, t, e data from server" << endl;	// debug
			chan.cwrite(buf, sizeof(datamsg));						// question
			cout << "Sent data message request to server." << endl; // debug
			double reply;
			chan.cread(&reply, sizeof(double)); // answer
			cout << "Received data message response from server: " << reply << endl;
			std::cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;

			delete[] buf;
		}
		else if (p_flag == true)
		{
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
				cout << "Requesting first 1000 lines of data from server" << endl; // debug
				chan.cwrite(buf, sizeof(datamsg));
				double ecg1_value;
				chan.cread(&ecg1_value, sizeof(double));
				cout << "Received ecg1 data: " << ecg1_value << " from server" << endl; // debug

				// request for ecg2 (column2)
				datamsg ecg2_request(p, current_time, 2);
				cout << "Requesting first 1000 lines of data from server" << endl; // debug
				memcpy(buf, &ecg2_request, sizeof(datamsg));
				chan.cwrite(buf, sizeof(datamsg));
				double ecg2_value;
				chan.cread(&ecg2_value, sizeof(double));
				cout << "Received ecg1 data: " << ecg2_value << " from server" << endl; // debug

				// write to the file
				outfile << current_time << "," << ecg1_value << "," << ecg2_value << endl;

				// increment time to move to next row in csv
				current_time += increment_time;
			}

			// close file when writing is done
			outfile.close();

			delete[] buf;
		}

		// sending a file request message
		else if (!filename.empty())
		{
			// Initial request for the file length
			filemsg fm(0, 0);
			int len = sizeof(filemsg) + (filename.size() + 1);
			char *buf2 = new char[len];
			memcpy(buf2, &fm, sizeof(filemsg));
			strcpy(buf2 + sizeof(filemsg), filename.c_str());
			cout << "Requesting file length from server" << endl; // debug
			chan.cwrite(buf2, len);

			delete[] buf2;

			// Reading the response for the file length
			char fileSizeBuffer[sizeof(__int64_t)];
			chan.cread(fileSizeBuffer, sizeof(__int64_t));
			cout << "Recevied file length from server" << endl; // debug
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
				chan.cwrite(buff, msgSize);

				// Memory management
				delete[] buff;

				char buffer[bufferCap];
				int bytesRead = chan.cread(buffer, length);
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
		}

		// closing the channels

		MESSAGE_TYPE m = QUIT_MSG;

		cout << "Client: Sending QUIT_MSG for the new channel." << endl; // debug
		new_channel->cwrite(&m, sizeof(MESSAGE_TYPE)); // Sending QUIT_MSG for the new channel
		delete new_channel;

		usleep(100000);							// sleep for 100ms
		cout << "Client: Sending QUIT_MSG for the control channel." << endl; // debug
		chan.cwrite(&m, sizeof(MESSAGE_TYPE)); // Sending QUIT_MSG for the control channel
	}
}