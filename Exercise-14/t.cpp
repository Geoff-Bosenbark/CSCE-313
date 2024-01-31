#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/param.h>
#include <ctime>
#include <chrono>
#include <time.h>

#include <vector>
#include <string>
#include <string.h>
#include <fcntl.h>

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
    string oldPath;
    string newPath;
    vector<pid_t> listPids;

    for (;;)
    {
        // need date/time, username, and absolute path to current dir
        char directory[MAXPATHLEN];
        char *path = getcwd(directory, MAXPATHLEN);
        char buffer[256];

        time_t timer = time(NULL);
        struct tm *timeInfo = localtime(&timer);
        strftime(buffer, 256, "%b %d %T", timeInfo);

        string absolutePath = path;
        cout << YELLOW << buffer << " " << YELLOW << getenv("USER") << ":" << YELLOW << absolutePath << "$" << NC << " ";
        // cout << YELLOW << ctime(&timer) << " "<< YELLOW << getenv("USER") << ":"<<YELLOW << absolutePath <<"$"<<NC<<" ";

        // get user inputted command
        string input;
        getline(cin, input);
        // cout<<"Input: "<<input<<endl;

        // Save original stdin and stdout
        int in = dup(0);
        int out = dup(1);
        // vector<char *> arguments;
        vector<char *> cdArgumentChecker;
        int fd[2];

        if (input == "exit" || input == "Exit")
        { // print exit message and break out of infinite loop
            cout << RED << "Now exiting shell..." << endl
                 << "Goodbye" << NC << endl;
            break;
        }

        if (input == "")
        {
            continue;
        }

        // Checking for background processes
        for (long unsigned int i = 0; i < listPids.size(); i++)
        {
            int status;
            // cout << "BG" << endl;
            if (waitpid(listPids[i], &status, WNOHANG) > 0) // Greater then 0 means child died
            {
                if (WIFSIGNALED(status) == true)
                {
                    // cout << "Erased " << endl;
                    listPids.erase(listPids.begin() + i);
                    i = i - 1;
                }
                // cout<<"Checking pid for sleep "<<endl;
                // listPids.erase(listPids.begin() + i);
                // i = i - 1;
            }
        }

        // get tokenized commands from user input
        Tokenizer token(input);
        if (token.hasError())
        { // continue to next prompt if input had an error
            continue;
        }

        cdArgumentChecker.push_back(const_cast<char *>(token.commands[0]->args[0].c_str()));
        cdArgumentChecker.push_back(const_cast<char *>(token.commands[0]->args[1].c_str()));
        cdArgumentChecker.push_back(NULL);

        char currentDirectory[MAXPATHLEN];
        char *pathDirectory = getcwd(currentDirectory, MAXPATHLEN);

        string currentPathDirectory = pathDirectory;

        if (strcmp(cdArgumentChecker[0], "cd") == 0)
        {

            if (strcmp(cdArgumentChecker[1], "-") != 0)
            {
                oldPath = currentPathDirectory;
                string newPathDirectory;

                newPathDirectory = cdArgumentChecker[1];
                newPath = newPathDirectory;

                if (chdir(newPathDirectory.c_str()) < 0)
                {
                    perror("chdir");
                };
            }
            else // cd - case
            {
                if (chdir(oldPath.c_str()) < 0)
                {
                    perror("cd: OLDPWD not set");
                };
                string temp;
                temp = oldPath;
                oldPath = newPath;
                newPath = temp;
            }
        }

        else
        {
            for (long unsigned int i = 0; i < token.commands.size(); i++)
            {
                vector<char *> arguments;

                // int fd[2];
                if (pipe(fd) == -1)
                {
                    return 0;
                }

                int pid = fork();

                if (pid == 0)
                {

                    for (long unsigned int j = 0; j < token.commands[i]->args.size(); j++)
                    {
                        arguments.push_back(const_cast<char *>(token.commands[i]->args[j].c_str()));
                    }

                    arguments.push_back(NULL);
                    int opener;

                    if (token.commands.at(i)->hasInput())
                    {
                        opener = open(token.commands.at(i)->in_file.c_str(), O_RDONLY, S_IRUSR);
                        dup2(opener, STDIN_FILENO);
                    }

                    if (token.commands.at(i)->hasOutput())
                    {
                        // cout<<token.commands.at(i)->out_file.c_str()<<endl;
                        opener = open(token.commands.at(i)->out_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
                        dup2(opener, STDOUT_FILENO);
                    }

                    if (i < token.commands.size() - 1)
                    {
                        dup2(fd[1], STDOUT_FILENO);
                    }

                    close(fd[STDIN_FILENO]);

                    if (execvp(arguments[0], arguments.data()) < 0)
                    {
                        perror("execvp");
                        exit(1);
                    };
                }
                else
                {
                    // Redriect the SHELL(PARENT)'s input to the read end of the pipe
                    dup2(fd[0], STDIN_FILENO);

                    // Close the write end of the pipe
                    close(fd[STDOUT_FILENO]);

                    if (token.commands.at(i)->isBackground())
                    {
                        cout << "Pid pushed back" << endl;
                        listPids.push_back(pid);
                    }
                    else
                    {
                        int status;
                        waitpid(pid, &status, 0);
                    }
                    // int status;
                    // waitpid(pid, &status, 0);
                }
            }

            // use dup(2) to restore the stdin and stdout
            dup2(in, 0);
            dup2(out, 1);
        }
    }
    return 0;
}