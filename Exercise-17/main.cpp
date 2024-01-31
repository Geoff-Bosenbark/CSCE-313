#include <iostream>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

int main(int argc, char *argv[])
{
    int option = 0; // default option: execute the command ls -l and terminate normally

    // Get flag from command line
    int opt;
    while ((opt = getopt(argc, argv, "n:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            option = atoi(optarg);
            break;
        }
    }

    // create new process
    pid_t pid = fork();

    if (pid < 0) // failed fork
    {
        cout << "Fork failed" << endl;
        return 1;
    }
    else if (pid == 0) // child process is 0
    {
        cout << "Hello from the child process!" << endl;
        cout << "Parent process ID: " << getppid() << endl; //getppid() --> system call for obtaining parent ID

        if (option % 2 == 0) // if the option number is even, execute the command ls -l and terminate normally
        {
            std::cout << "The child process will execute the command: ls -l after 6 seconds" << std::endl;
            sleep(6); // makes sleep for 6 seconds

            char *args[] = {(char *)"ls", (char *)"-l", NULL}; // array of char ptrs for execvp to execute ls -l
            execvp(args[0], args); // Executes command ls -l
        }
        else // if the option number is odd, terminate with a kill signal
        {
            std::cout << "The child process is exiting" << std::endl;
            kill(getpid(), SIGINT);
        }
    }
    else if (pid > 0) // parent process
    {
        // cout << "Parent: Start" << endl; // debug
        int status;

        waitpid(pid, &status, 0); // waitpid system call --> parent waits for child process state to change

        // cout << "Parent: Waiting for child..." << endl; // debug

        cout << "\nHello from the parent process!" << endl;
        cout << "Child process ID: " << pid << endl;


        if (WIFEXITED(status)) // child process normal exit
        {
            cout << "The child process exited normally" << endl;
        }

        if (WIFSIGNALED(status)) // child process kill signal
        {
            cout << "The child process exited due to the kill signal" << endl;
        }

        // cout.flush(); // (debug) used flush to fix cout issues
        // cout << "Parent: End" << endl; // debug
    }

    return 0;
}