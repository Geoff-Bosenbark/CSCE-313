#include <sys/types.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
#include <vector>

#include <string>
#include <cstring>
#include <signal.h>

#include <chrono> // for date/time

#include "Tokenizer.h"

// all the basic colours for a shell prompt
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define WHITE "\033[1;37m"
#define NC "\033[0m"

using namespace std;

int main()
{
    // save original stdin, stdout
    int og_stdin = dup(STDIN_FILENO);
    int og_stdout = dup(STDOUT_FILENO);

    // path holders
    string newDirectory;
    string prevDirectory;

    // create vector to store pids
    vector<pid_t> pids;

    while (true)
    {
        // implement current dir, use cwd()
        char directory[MAXPATHLEN];                          // char array for cwd
        char *directoryPath = getcwd(directory, MAXPATHLEN); // stores current working directory in char array and points 'path' to directory
        string currDirectory = directoryPath;                // converts cwd to a string for use

        // implement date/time, use
        char date_time[256];
        time_t t = time(NULL);                          // get current time
        struct tm *loc_time = localtime(&t);            // format time
        strftime(date_time, 256, "%b %d %T", loc_time); // display formatted time as str

        // implement username, use getenv("USER")
        cout << GREEN << date_time << " " << GREEN << getenv("USER") << NC << ":" << BLUE << currDirectory << NC << "$"
             << " ";

        // get user inputted command
        string input;
        getline(cin, input);

        // exit and empty input handling
        if (input == "exit")
        {
            // print exit message and break out of infinite loop
            cout << RED << "Now exiting shell..." << endl
                 << "Goodbye" << NC << endl;
            break;
        }
        else if (input == "")
        {
            continue;
        }

        // add check for bg process
        for (size_t i = 0; i < pids.size(); i++)        // iterate through pids vector
        {
            int status;
            if (waitpid(pids[i], &status, WNOHANG) > 0) // WNOHANG causes waitpid() to return if child has not terminated
            {
                if (WIFSIGNALED(status) == true)        // returns true if child termindated due to recv signal that was not caught
                {
                    pids.erase(pids.begin() + i);       // remove pid at pids[index]
                    i = i - 1;                          // adjusting pid index post-removal
                }
            }
        }

        // get tokenized commands from user input
        Tokenizer token(input);
        if (token.hasError())
        {
            // continue to next prompt if input had an error
            continue;
        }

        // TODO code for chdir (cd (dir))
        // if "-"" symbol, go to prev working dir (else normal chdir)
        // var to store prev working dir (must be declared outside of infinite loop)
        
        vector<char *> cd_args;
        cd_args.push_back(const_cast<char *>(token.commands[0]->args[0].c_str())); // first arg of first cmd
        cd_args.push_back(const_cast<char *>(token.commands[0]->args[1].c_str())); // second arg of first cmd
        cd_args.push_back(NULL);

        if (strcmp(cd_args[0], "cd") == 0)              // check if first arg in cmd in cd
        {
            if (strcmp(cd_args[1], "-") != 0)           // check if second arg in cmd is NOT -
            {
                prevDirectory = currDirectory;          // store cwd in prev before changing / updating cwd to new
                newDirectory = cd_args[1];              // directory we want to move to is second arg

                if (chdir(newDirectory.c_str()) < 0)    // change to newDirectory (making it current)
                {
                    perror("chdir");
                }
            }
            else // 'cd -' handler
            {
                // if cd fails
                if (chdir(prevDirectory.c_str()) < 0)   // change cwd to pwd
                {
                    perror("cd: OLDPWD not set");       // Old Print Working Directory -- holds path of prev working directory (pwd)
                }

                // update directory variables
                swap(prevDirectory, newDirectory);
            }
        }

        // call pipe(), fork()
        // add checks for last command

        else
        {
            for (size_t i = 0; i < token.commands.size(); i++)
            {
                int fd[2];
                pipe(fd);
                pid_t pid = fork(); // create child
                if (pid < 0)
                {
                    // error check
                    perror("fork");
                    exit(2);
                }

                // in child, redirect stdout (close read side of pipe)
                if (pid == 0) // child
                {
                    if (i < token.commands.size() - 1)  // check if not last cmd
                    {
                        dup2(fd[1], STDOUT_FILENO);     // dup write end so output goes to pipe
                        close(fd[0]);                   // close read end of pipe
                    }

                    Command *currentCommand = token.commands.at(i);     // get ptr to current cmd

                    if (currentCommand->hasInput())     // check if cmd has input redirect
                    {
                        int in_fd = open(currentCommand->in_file.c_str(), O_RDONLY); // open specified input file
                        dup2(in_fd, STDIN_FILENO);      // fd to STDIN
                        close(in_fd);                   // close original fd
                    }

                    if (currentCommand->hasOutput())    // check if cmd has output redirect
                    {
                        int out_fd = open(currentCommand->out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666); // open/create output file
                        dup2(out_fd, STDOUT_FILENO);    // fd to STDOUT
                        close(out_fd);                  // close original fd
                    }

                    int num_args = currentCommand->args.size();     // get num of args in cmd
                    char **args = new char *[num_args + 1];         // holds cmd and args
                    for (int j = 0; j < num_args; j++)              // fill args array
                    {
                        args[j] = (char *)currentCommand->args.at(j).c_str();
                    }
                    args[num_args] = nullptr;   // nullptr at end, required for execvp

                    execvp(args[0], args);      // replace process image
                    perror("execvp");           // error handling
                    exit(EXIT_FAILURE);
                }

                // in parent, redirect stdin (close write side of pipe)
                else // parent
                {
                    dup2(fd[0], STDIN_FILENO);  // redirect parent input to read end of pipe
                    close(fd[STDOUT_FILENO]);   // close write end of pipe
                    int status = 0;
                    waitpid(pid, &status, 0);   // wait for child, store child status
                    if (status > 1)
                    {
                        // exit if child didn't exec properly
                        exit(status);
                    }
                }
            }
            // restore initial stdin/out
            dup2(og_stdin, STDIN_FILENO);
            dup2(og_stdout, STDOUT_FILENO);
        }
    }
}