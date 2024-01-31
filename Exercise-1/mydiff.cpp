

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const char *PIPES[] = {"./pipe1", "./pipe2"};

vector<string> parse_command(string command)
{
    vector<string> args;
    string arg;
    for (char c : command)
    {
        if (c == ' ')
        {
            if (arg.length() > 0)
                args.push_back(arg);
            arg = "";
        }
        else
        {
            arg += c;
        }
    }
    if (arg.length() > 0)
        args.push_back(arg);
    return args;
}

void redirect_stdout(int pipeid)
{
    /* TODO: Open the correct pipe, and redirect stdout to the pipe */

    /* ******************************************* */
    int fd = open(PIPES[pipeid], O_WRONLY); // write only
    // error handling
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }
    // error handling
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("dup2");
        exit(EXIT_FAILURE);
    }
    // close when done
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: ./mydiff <command1> <command2>" << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> commands[2] = {parse_command(argv[1]), parse_command(argv[2])};
    pid_t pids[2];
    ifstream fstreams[2];
    int fd[2]; // array to store pipe file descriptors

    for (int i = 0; i < 2; ++i)
    {
        /* TODO: Create a named pipe here with the right permissions */

        /* ********************************************************* */
        if (mkfifo(PIPES[i], 0666) == -1) // 0666 assigns read, write permissions
        {
            perror("mkfifo");
            exit(EXIT_FAILURE);
        }

        /* TODO: Create a new process */
        /* ************************** */
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) // child process
        {
            redirect_stdout(i);
            /* TODO: Execute the command commands[i] here. */

            /***********************************************/
            std::vector<char*> args(commands[i].size() + 1);
            for (size_t j = 0; j < commands[i].size(); ++j)
            {
                args[j] = const_cast<char*>(commands[i][j].c_str());
            }
            args[commands[i].size()] = NULL;

            execvp(args[0], args.data());
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        else
        {
            // parent process
            pids[i] = pid;
            // create streams for reading from pipes (no need to modify)
            fstreams[i].open(PIPES[i], ifstream::in | ifstream::binary);
        }
    }

    /* compare lines and print differences
    DO NOT modify this section.
    */
    string line1, line2;
    while (fstreams[0] && fstreams[1])
    {
        getline(fstreams[0], line1);
        getline(fstreams[1], line2);
        if (line1 != line2)
        {
            cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
            cout << line1 << endl;
            cout << "======================" << endl;
            cout << line2 << endl;
            cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
        }
    }

    while (fstreams[0])
    {
        getline(fstreams[0], line1);
        cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
        cout << line1 << endl;
        cout << "======================" << endl;
        cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
    }

    while (fstreams[1])
    {
        getline(fstreams[1], line2);
        cout << "<<<<<<<<<<<<<<<<<<<<<<" << endl;
        cout << "======================" << endl;
        cout << line2 << endl;
        cout << ">>>>>>>>>>>>>>>>>>>>>>" << endl;
    }

    for (int i = 0; i < 2; ++i)
    {
        /* TODO: Cleanup: close any open pipes and wait for any child processes */

        /* *********************************************************** */
        close(fd[i]);
        waitpid(pids[i], NULL, 0);
        unlink(PIPES[i]);
    }
}
