#include "RoundRobin.h"
#include "Process.h"

/*
This is a constructor for RoundRobin Scheduler, you should use the extractProcessInfo function first
to load process information to process_info and then sort process by arrival time;

Also initialize time_quantum
*/
RoundRobin::RoundRobin(string file, int time_quantum) : time_quantum(time_quantum)
{
	this->extractProcessInfo(file);																				// load process info into processVec
	this->set_time_quantum(time_quantum);																		// set time quantum
	sort(begin(processVec), end(processVec), [](const shared_ptr<Process> &p1, const shared_ptr<Process> &p2) { // sort processVec by arrival time
		return p1->get_arrival_time() < p2->get_arrival_time();
	});
}

// Schedule tasks based on RoundRobin Rule
// the jobs are put in the order the arrived
// Make sure you print out the information like we put in the document
void RoundRobin::schedule_tasks()
{
	// for tracking system time, will increment every second
	int sysTime = 0;

	// while there is still a process in either the queue or the vector
	while (!processQ.empty() || !processVec.empty())
	{

		// while there is still a process in the vector and the system time is higher than the first process' arrival time
		while (!processVec.empty() && sysTime >= processVec.front()->get_arrival_time())
		{
			processQ.push(processVec.front());		// add process to queue
			processVec.erase(processVec.begin());	// remove process from processVec
		}

		// process in queue
		if (!processQ.empty())
		{
			if (processQ.front()->get_remaining_time() == 0)	// process doesn't need anymore time
			{
				processQ.front()->Run(0);	// update remaining time and mark process as completed
			}

			// calculate time current process needs to run (up to quantum)
			int executionTime = min(this->get_time_quantum(), processQ.front()->get_remaining_time());

			// run process execution time is reached
			for (int i = 0; i < executionTime; i++)
			{
				print(sysTime, processQ.front()->getPid(), false);	// display process info, false because still running
				processQ.front()->Run(1);							// run process for 1 second, reducing remaining time
				sysTime++;											// incrememt system time

				// check for new process arrival
				while (!processVec.empty() && sysTime >= processVec.front()->get_arrival_time())
				{
					processQ.push(processVec.front());		// add process to queue
					processVec.erase(processVec.begin());	// remove process from processVec
				}
			}

			if (processQ.front()->is_Completed()) // process completed?
			{
				print(sysTime, processQ.front()->getPid(), true); // print process
				processQ.pop();									  // remove from queue
			}
			else // process not completed, has time remaining
			{
				processQ.push(processQ.front()); // move process to back of queue
				processQ.pop();
			}
		}
		else
		{
			print(sysTime, -1, false); // no process to run; PID = NOP
			sysTime++;				   // increment sysTime
		}
	}
}

/***************************
ALL FUNCTIONS UNDER THIS LINE ARE COMPLETED FOR YOU
You can modify them if you'd like, though :)
***************************/

// Default constructor
RoundRobin::RoundRobin()
{
	time_quantum = 0;
}

// Time quantum setter
void RoundRobin::set_time_quantum(int quantum)
{
	this->time_quantum = quantum;
}

// Time quantum getter
int RoundRobin::get_time_quantum()
{
	return time_quantum;
}

// Print function for outputting system time as part of the schedule tasks function
void RoundRobin::print(int system_time, int pid, bool isComplete)
{
	string s_pid = pid == -1 ? "NOP" : to_string(pid);
	cout << "System Time [" << system_time << "].........Process[PID=" << s_pid << "] ";
	if (isComplete)
		cout << "finished its job!" << endl;
	else
		cout << "is Running" << endl;
}

// modified with shared_ptr for better clean up
// Read a process file to extract process information
// All content goes to proces_info vector
void RoundRobin::extractProcessInfo(string file)
{
	// open file
	ifstream processFile(file);
	if (!processFile.is_open())
	{
		perror("could not open file");
		exit(1);
	}

	// read contents and populate process_info vector
	string curr_line, temp_num;
	int curr_pid, curr_arrival_time, curr_burst_time;
	while (getline(processFile, curr_line))
	{
		// use string stream to seperate by comma
		stringstream ss(curr_line);
		getline(ss, temp_num, ',');
		curr_pid = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_arrival_time = stoi(temp_num);
		getline(ss, temp_num, ',');
		curr_burst_time = stoi(temp_num);
		shared_ptr<Process> p(new Process(curr_pid, curr_arrival_time, curr_burst_time)); // used shared_ptr

		processVec.push_back(p);
	}

	// close file
	processFile.close();
}
