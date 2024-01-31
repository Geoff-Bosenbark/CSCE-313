#include "FIFORequestChannel.h"

using namespace std;

/*--------------------------------------------------------------------------*/
/*		CONSTRUCTOR/DESTRUCTOR FOR CLASS	R e q u e s t C h a n n e l		*/
/*--------------------------------------------------------------------------*/

FIFORequestChannel::FIFORequestChannel (const string _name, const Side _side) : my_name( _name), my_side(_side) {
	pipe1 = "fifo_" + my_name + "1";
	pipe2 = "fifo_" + my_name + "2";
		
	if (_side == SERVER_SIDE) {
		wfd = open_pipe(pipe1, O_WRONLY);
		rfd = open_pipe(pipe2, O_RDONLY);
	}
	else {
		rfd = open_pipe(pipe1, O_RDONLY);
		wfd = open_pipe(pipe2, O_WRONLY);
	}
}

FIFORequestChannel::~FIFORequestChannel () { 
	close(wfd);
	close(rfd);

	remove(pipe1.c_str());
	remove(pipe2.c_str());
}

/*--------------------------------------------------------------------------*/
/*			MEMBER FUNCTIONS FOR CLASS	R e q u e s t C h a n n e l			*/
/*--------------------------------------------------------------------------*/

int FIFORequestChannel::open_pipe (string _pipe_name, int mode) {
	mkfifo (_pipe_name.c_str (), 0600);
	int fd = open(_pipe_name.c_str(), mode);
	if (fd < 0) {
		EXITONERROR(_pipe_name);
	}
	return fd;
}

/*
cread is used to read data from the read-end of the FIFO.

The function accepts two parameters: 
1. a pointer 'msgbuf' to the buffer where the received data will be stored and 
2. an integer 'msgsize' which specifies the number of bytes to read.

The function uses the read system call to read from rfd (the read file descriptor of the named pipe) and stores the received data in the provided buffer.
It returns the number of bytes actually read. If the function returns a number less than msgsize, it indicates that there might have been fewer bytes available
in the FIFO than requested. If it returns -1, there was an error during the reading process.
*/

int FIFORequestChannel::cread (void* msgbuf, int msgsize) {
	return read (rfd, msgbuf, msgsize); 
}


/*
cwrite is used to write data to the write-end of the FIFO.

The function accepts two parameters: 
1. a pointer msgbuf to the buffer containing the data to be written and
2. an integer msgsize which specifies the number of bytes to write.
It uses the write system call to write to wfd (the write file descriptor of the named pipe) from the provided buffer.
It returns the number of bytes that were actually written. If the function returns a number less than msgsize, it means that not all of the bytes were written. 
A return value of -1 indicates an error during the writing process.
*/


int FIFORequestChannel::cwrite (void* msgbuf, int msgsize) {
	return write (wfd, msgbuf, msgsize);
}

string FIFORequestChannel::name () {
	return my_name;
}
