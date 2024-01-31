/*
Name:   Geoffrey Bosenbark
UIN:    232002930
Course: CSCE-313-503 (Kebo)
File:   Quiz 2-2-b
Desc:   Write a C program that prints the type of file for each path provided as a command-line argument
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    // struct to hold info
    struct stat st;

    // case for no args
    if (argc < 2)
    {
        printf("usage: path1 [path2 ...]\n");
        return 1;
    }

    // loop through cmd args
    for (int i = 1; i < argc; i++)
    {
        // use lstat to handle symbolic links properly
        if(lstat(argv[i], &st) == 0)
        {
            // check for all cases, print file type
            if(S_ISREG(st.st_mode))
            {
                printf("%s: regular file\n", argv[i]);
            }
            else if(S_ISDIR(st.st_mode))
            {
                printf("%s: directory\n", argv[i]);
            }
            else if(S_ISLNK(st.st_mode))
            {
                printf("%s: symbolic link\n", argv[i]);
            }
            else if(S_ISCHR(st.st_mode))
            {
                printf("%s: character special file\n", argv[i]);
            }
            else if(S_ISBLK(st.st_mode))
            {
                printf("%s: block special file\n", argv[i]);
            }
            else if(S_ISFIFO(st.st_mode))
            {
                printf("%s: FIFO (named pipe)\n", argv[i]);
            }
            else if(S_ISSOCK(st.st_mode))
            {
                printf("%s: socket\n", argv[i]);
            }
            else // anything else
            {
                printf("%s: unknown\n", argv[i]);
            }
        }
        else // if lstat fails
        {
            printf("%s: path error\n", argv[i]);
        }
    }

    return 0;
}