#include "common.h"
#include "FIFORequestChannel.h"
#include <sys/wait.h> // for "wait()"

using namespace std;

int main () 
{
    int buffer_capacity = 2;
	int fileSize = 5;
	int length = fileSize;
	int remainingBytes = fileSize;
	off_t offset = 0;

	while (remainingBytes != 0)
	{
		if (remainingBytes < buffer_capacity)
		{
			length = remainingBytes;
		}
		// make a file transfer
		//filemsg(offset, length);

		// to prevent decrement from going negative
		if (remainingBytes >= buffer_capacity)
		{
			remainingBytes -= buffer_capacity;
		}
		else
		{
			remainingBytes = 0;
		}
		offset = fileSize - remainingBytes;

        cout << "File size: " << fileSize << endl;
        cout << "Length: " << length << endl;
        cout << "Remaining: " << remainingBytes << endl;
        cout << "Offset: " << offset << endl;
        cout << endl;
	}
}