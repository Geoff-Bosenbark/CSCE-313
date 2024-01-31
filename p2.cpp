#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#define MAX_MESSAGE 256

long long unsigned int hash(int seed, char *buf, int nbytes)
{
    long long unsigned int H = seed;
    for (int i = 0; i < nbytes; ++i)
        H = H * 2 * buf[i] + 1;
    return H;
}

int main(int argc, char **argv)
{
    // check if user input command line args
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <message>" << std::endl;
        return 1;
    }

    // TODO: create pipe

    int p1_to_p2[2], p2_to_p1[2]; // pipes

    // Pipe for communication from P1 to P2
    if (pipe(p1_to_p2) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // Pipe for communication from P2 to P1
    if (pipe(p2_to_p1) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int pid = fork(); // create child (P2)
    if (pid == 0) // child process (P2)
    {            
        // TODO: read from parent
        close(p1_to_p2[1]); // close unused write end
        close(p2_to_p1[0]); // close unused read end

        char buf[MAX_MESSAGE];
        ssize_t bytes = read(p1_to_p2[0], buf, MAX_MESSAGE); // read from P1
        close(p1_to_p2[0]); // close read end after reading

        if (bytes > 0)
        {
            // TODO: compute hash
            // TODO: send hash to parent
            long long unsigned int h = hash(getpid(), buf, bytes);
            write(p2_to_p1[1], &h, sizeof(h)); // write to P1
        }
        close(p2_to_p1[1]); // close write end after writing
    }
    else // parent process (P1)
    {
        // TODO: write to child
        close(p1_to_p2[0]); // close unused read end
        close(p2_to_p1[1]); // close unused write end

        char *msg = argv[1];
        write(p1_to_p2[1], msg, strlen(msg));
        close(p1_to_p2[1]); // close write end after writing

        // TODO: get hash from child
        long long unsigned int hrecv;
        read(p2_to_p1[0], &hrecv, sizeof(hrecv));
        close(p2_to_p1[0]); // close read end after reading

        // TODO: calculate hash on parent side
        long long unsigned int h = hash(pid, msg, strlen(msg));

        // print hashes; DO NOT change
        printf("%llX\n", h);
        printf("%llX\n", hrecv);
    }
    return 0;
}