/****************
https://github.com/CSCE313-FA23-Kebo/lab-exercise-3-Bosenbarkgb
LE3: Basic Shell
****************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include "Tokenizer.h"

using namespace std;

int main()
{
    // save original stdin, stdout
    int og_stdin = dup(STDIN_FILENO);
    int og_stdout = dup(STDOUT_FILENO);
    string input;

    // infinite loop -- break out using command "exit"
    while (true)
    {
        // get command from user
        cout << "Enter Command> ";
        getline(cin, input);

        // break out case
        if (input == "exit")
        {
            break;
        }

        // feed input into tokenizer
        // parse input and create instances of command class
        Tokenizer token(input);

        // i must be size_t (not int) to avoid comparison errors with token.commands.size()
        // loop through each command parsed by tokenizer
        for (size_t i = 0; i < token.commands.size(); i++)
        {
            int fd[2]; // 0 = read, 1 = write
            pipe(fd);  // Create the pipe for IPC

            // child process
            if (fork() == 0)
            {
                // do not redirect output of last command
                if (i < token.commands.size() - 1)
                {
                    dup2(fd[1], STDOUT_FILENO); // Redirect stdout to write end of pipe
                    close(fd[0]);               // Close read end of pipe
                }

                int num_args = token.commands.at(i)->args.size(); // determine and store number of args
                char **args = new char *[num_args + 1];           // create / alloc mem for char* array (+ 1 for nullptr)

                // set args for commands, placing in char* array (instead of commands vector) for input into execvp
                for (int j = 0; j < num_args; j++)
                {
                    args[j] = {(char *)token.commands.at(i)->args.at(j).c_str(/*returns a const char* pointing to the string’s contents*/)};
                    /*
                        Example:
                            if args at index j is "ls", then args[j] will be a pointer to the character array {'l', 's', '\0'}.
                    */
                }
                args[num_args] = nullptr; // add nullptr to end of args to signal end of args

                execvp(args[0], args); // Execute command; replacing current child with new process specified by command
                /*
                    Example:
                        command: ls -l -a
                            args[0] = ls
                            args[1] = -l
                            args[2] = -a
                            args[3] = nullptr
                        execvp(args[0], args) --> execvp(ls, array of ptrs); array is {ls, -1, -a, nullptr}
                        execvp needs command as first parameter, and included in array in second parameter

                        Replaces current process with "ls" and passes args "-l -a" to ls
                */
            }
            else // parent (shell) process
            {
                dup2(fd[0], STDIN_FILENO); // redirect shell (parent) input to read end of pipe
                close(fd[1]);              // close write end of pipe

                // synchronize
                wait(0);
            }
        }

        // restore stdin/out
        dup2(og_stdin, STDIN_FILENO);
        dup2(og_stdout, STDOUT_FILENO);
    }
    // cout << "Exiting ... " << endl; // debug
}