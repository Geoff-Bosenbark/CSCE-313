/****************
LE2: Introduction to Unnamed Pipes
****************/

// ORIGINAL SUBMISSION CODE
// #include <unistd.h> // pipe, fork, dup2, execvp, close
// using namespace std;

// int main()
// {
//     // lists all the files in the root directory in the long format
//     // arguemnts for ls
//     char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};

//     // translates all input from lowercase to uppercase
//     // arguments for tr
//     char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

//     // Save original stdin and stdout: dup(stdin); dup(stdout);
//     int stdin = dup(STDIN_FILENO);
//     int stdout = dup(STDOUT_FILENO);

//     int fd[2]; // 2 file descriptors for the pipe: fd[0] is read end, fd[1] is write end
//     /*
//     pipe():
//     If successful, two file descriptors are stored in PIPEDES;
//     bytes written on PIPEDES[1] can be read from PIPEDES[0].
//     */

//     // create pipe -- stores descriptors for read/write

//     pipe(fd);

//     // Create child to run first command

//     if (fork() == 0) // creates first child process
//     {
//         // In child, redirect output to write end of pipe
//         dup2(fd[1], STDOUT_FILENO);     // fd[1] --> write, STDOUT_FILENO --> output

//         // Close both ends to clean up and prevent unexpected occurrences
//         close(fd[0]);   // ls doesn't read, only writes
//         close(fd[1]);   // close because STDOUT_FILENO points to location of the pipe's write end, fd[1]

//         // In child, execute the command
//         execvp(cmd1[0], cmd1); // cmd1[0] = ls = program to be executed -- cmd1 is array of args
//     }

//     // Create another child to run second command

//     if (fork() == 0) // creates second child process
//     {
//         // In child, redirect input to the read end of the pipe
//         dup2(fd[0], STDIN_FILENO); // fd[0] --> read, STDIN_FILENO --> input

//         // Close both ends to clean up and prevent unexpected occurrences
//         close(fd[1]); // tr doesn't write, only reads
//         close(fd[0]); // close because STDIN_FILENO points to location of the pipe's read end, fd[0]

//         // In child, execute the command
//         execvp(cmd2[0], cmd2); // cmd2[0] = tr = program to be executed -- cmd2 is array of args
//     }

//     // close pipe's read/write ends in parent
//     close(fd[0]);
//     close(fd[1]);

//     // Reset the input and output file descriptors of the parent.
//     // Overwrite in/out with what was saved, dup2();
//     dup2(stdin, STDIN_FILENO);
//     dup2(stdout, STDOUT_FILENO);

//     // close original in/out
//     close(stdin);
//     close(stdout);
// }

// REVISiON 1 (STIL NOT CORRECT, BUT PASSES)
// #include <unistd.h>   // pipe, fork, dup2, execvp, close, wait
// #include <sys/wait.h> // wait
// #include <iostream>

// int main()
// {
//     char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};
//     char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

//     int fd[2];
//     if (pipe(fd) < 0)
//     {
//         std::cerr << "Pipe failed" << std::endl;
//         return -1;
//     }

//     pid_t child1 = fork();
//     if (child1 < 0)
//     {
//         std::cerr << "Fork failed" << std::endl;
//         return -1;
//     }

//     if (child1 == 0) // In child1
//     {
//         dup2(fd[1], STDOUT_FILENO); // Redirect output to write end of pipe in child1
//         close(fd[0]);               // Child1 does not need to read from pipe
//         close(fd[1]);               // Already written to STDOUT, not needed anymore

//         execvp(cmd1[0], cmd1);
//         std::cerr << "Failed to execute " << cmd1[0] << std::endl;
//         _exit(1);
//     }

//     // Only parent gets here and creates child2
//     pid_t child2 = fork();
//     if (child2 < 0)
//     {
//         std::cerr << "Fork failed" << std::endl;
//         return -1;
//     }

//     if (child2 == 0) // In child2
//     {
//         dup2(fd[0], STDIN_FILENO); // Redirect input to read end of pipe in child2
//         close(fd[0]);              // Already read from STDIN, not needed anymore
//         close(fd[1]);              // Child2 does not need to write to pipe

//         execvp(cmd2[0], cmd2);
//         std::cerr << "Failed to execute " << cmd2[0] << std::endl;
//         _exit(1);
//     }

//     // Parent closes unused file descriptors and waits for children to finish
//     close(fd[0]);
//     close(fd[1]);

//     waitpid(child1, NULL, 0);
//     waitpid(child2, NULL, 0);

//     return 0;
// }

// REVISION 2: CORRECT & PASSES
// #include <unistd.h>
// #include <sys/wait.h>
// #include <iostream>

// int main()
// {
//     // Commands to be executed
//     char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};
//     char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

//     // Pipe file descriptors
//     int fd[2];

//     if (pipe(fd) < 0)
//     {
//         std::cerr << "Pipe failed" << std::endl;
//         return 1;
//     }

//     pid_t child_pid = fork();

//     if (child_pid < 0)
//     {
//         std::cerr << "First fork failed" << std::endl;
//         return 1;
//     }

//     if (child_pid == 0)
//     {                 // Child process to execute ls
//         close(fd[0]); // Child does not need to read from pipe

//         // Redirect stdout to write end of pipe
//         dup2(fd[1], STDOUT_FILENO);
//         close(fd[1]);

//         // Execute ls command
//         execvp(cmd1[0], cmd1);
//         std::cerr << "Execution of " << cmd1[0] << " failed" << std::endl;
//         _exit(1); // Exit if exec fails
//     }
//     else
//     {
//         close(fd[1]); // Parent does not need to write to pipe

//         pid_t grandchild_pid = fork();

//         if (grandchild_pid < 0)
//         {
//             std::cerr << "Second fork failed" << std::endl;
//             return 1;
//         }

//         if (grandchild_pid == 0)
//         { // Grandchild process to execute tr
//             // Redirect stdin to read end of pipe
//             dup2(fd[0], STDIN_FILENO);
//             close(fd[0]);

//             // Execute tr command
//             execvp(cmd2[0], cmd2);
//             std::cerr << "Execution of " << cmd2[0] << " failed" << std::endl;
//             _exit(1); // Exit if exec fails
//         }
//         else
//         {
//             close(fd[0]);                     // Close read end of pipe in the parent
//             waitpid(child_pid, NULL, 0);      // Wait for child to finish
//             waitpid(grandchild_pid, NULL, 0); // Wait for grandchild to finish
//         }
//     }
//     return 0;
// }

// int main()
// {

//     // Commands to be executed
//     char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};
//     char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

//     // Pipe file descriptors
//     int fds[2];
//     pipe(fds);
//     // child process #1
//     if (fork() == 0)
//     {
//         // Reassign stdin to fds[0] end of pipe.
//         dup2(fds[0], STDIN_FILENO);
//         close(fds[1]);
//         close(fds[0]);
//         // Execute the second command.
//         // child process #2
//         if (fork() == 0)
//         {
//             // Reassign stdout to fds[1] end of pipe.
//             dup2(fds[1], STDOUT_FILENO);
//             close(fds[0]);
//             close(fds[1]);
//             // Execute the first command.
//             execvp(cmd1[0], cmd1);
//         }
//         wait(NULL);
//         execvp(cmd2[0], cmd2);
//     }
//     close(fds[1]);
//     close(fds[0]);
//     wait(NULL);
// }

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

int main()
{
    // Commands to be executed
    char *cmd1[] = {(char *)"ls", (char *)"-al", (char *)"/", nullptr};
    char *cmd2[] = {(char *)"tr", (char *)"a-z", (char *)"A-Z", nullptr};

    // Pipe file descriptors
    int fds[2];
    pipe(fds); // Create the pipe

    // child process #1
    if (fork() == 0)
    {
        close(fds[0]); // Close read end as we are writing.
        dup2(fds[1], STDOUT_FILENO); // Redirect stdout to write end of pipe.
        close(fds[1]); // Close write descriptor after duplicating.
        execvp(cmd1[0], cmd1); // Execute first command.
    }

    // child process #2
    if (fork() == 0)
    {
        close(fds[1]); // Close write end as we are reading.
        dup2(fds[0], STDIN_FILENO); // Redirect stdin to read end of pipe.
        close(fds[0]); // Close read descriptor after duplicating.
        execvp(cmd2[0], cmd2); // Execute second command.
    }

    // Parent process
    close(fds[0]); // Close read end in the parent.
    close(fds[1]); // Close write end in the parent.
    wait(NULL); // Wait for the first child.
    wait(NULL); // Wait for the second child.
    
    return 0;
}
