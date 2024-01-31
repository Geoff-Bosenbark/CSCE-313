#include <signal.h>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    int n = 1, opt;
    while ((opt = getopt(argc, argv, "n:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            n = atoi(optarg);
            break;
        }
    }
    /* 
    1. TODO: fork n child processes and run p1-helper on each using execvp
        > note: we need to synchronize the child processes to print in the desired order
        > note: sleep(..) does not guarantee desired synchronization
        > note: check "raise" system call
        > note: check "WUNTRACED" flag for "waitpid"
    */

    vector<pid_t> c_pid;
    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0)
        {
            raise(SIGSTOP);
            char *args[] = {const_cast<char *>("./p1-helper"), nullptr}; // must be cast for execvp
            execvp(args[0], args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        c_pid.push_back(pid);
    }

    /*
    2. TODO: print children pids
    */

    // sorting pid in ascending order
    sort(c_pid.begin(), c_pid.end());

    // Iterating over sorted pids and printing them without any trailing space.
    for (size_t i = 0; i < c_pid.size(); i++)
    {
        cout << c_pid[i];
        if (i < c_pid.size() - 1) // not the last element
            cout << " ";
    }
    cout << endl;   // printing the end line after printing all pids
    fflush(stdout); // ensuring all the pids are printed before moving forward

    /*
    3. TODO: signal children with the reverse order of pids
        > note: take a look at "kill" system call
    */

    // signaling the children in reverse order of their pid
    for (auto r_itr = c_pid.rbegin(); r_itr != c_pid.rend(); ++r_itr)
    {
        kill(*r_itr, SIGCONT); // continue stopped child process
        int status;
        waitpid(*r_itr, &status, WUNTRACED); // wait for the child process to stop again
    }

    // last print statement before exit
    cout << "Parent: exiting" << endl;
    return 0;
}
