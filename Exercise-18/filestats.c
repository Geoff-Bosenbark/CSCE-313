/*
Name:   Geoffrey Bosenbark
UIN:    232002930
Course: CSCE-313-503 (Kebo)
File:   Quiz 2-2-a
Desc:   Write a C program that takes a directory name as input and
        gives information about the named files in the directory.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>

// takes directory and file name as input
void print_file_info(const char *directory, const char *filename)
{
    char path[1024]; // char array for pathname
    struct stat st;  // from sys/stat.h

    // full path of the file (dir/file) (snprintf for max file size of sizeof(path))
    snprintf(path, sizeof(path), "%s/%s", directory, filename);

    // fetch metadata, store in st struct (and error handling)
    if (stat(path, &st) == -1)
    {
        perror("Error getting file stats");
        return;
    }

    /*
        - File name
        - inode number
        - number of links
        - User ID of owner
        - Group ID of owner
        - Size in bytes
        - Last access
        - Last modification
        - Last status change
        - Number of disk blocks allocated
        - Access mode in octal
        - Access mode flags
    */

    // reference stat struct for metadata identifiers
    printf("File name: %s\n", filename);
    printf("inode number: %lu\n", st.st_ino);
    printf("number of links: %lu\n", st.st_nlink);
    printf("User ID of owner: %d\n", st.st_uid);
    printf("Group ID of owner: %d\n", st.st_gid);
    printf("Size in bytes: %ld bytes\n", st.st_size);
    printf("Last access: %s", ctime(&st.st_atime));
    printf("Last modification: %s", ctime(&st.st_mtime));
    printf("Last status change: %s", ctime(&st.st_ctime));
    printf("Number of disk blocks allocated: %ld\n", st.st_blocks);
    printf("Access mode in octal: %o\n", st.st_mode & 07777);

    // print the access mode flags
    printf("Access mode flags: ");
    printf((S_ISDIR(st.st_mode)) ? "d" : "-");

    // user permissions
    printf((st.st_mode & S_IRUSR) ? "r" : "-");
    printf((st.st_mode & S_IWUSR) ? "w" : "-");
    printf((st.st_mode & S_IXUSR) ? "x" : "-");

    // group permissions
    printf((st.st_mode & S_IRGRP) ? "r" : "-");
    printf((st.st_mode & S_IWGRP) ? "w" : "-");
    printf((st.st_mode & S_IXGRP) ? "x" : "-");

    // other permissions
    printf((st.st_mode & S_IROTH) ? "r" : "-");
    printf((st.st_mode & S_IWOTH) ? "w" : "-");
    printf((st.st_mode & S_IXOTH) ? "x" : "-");
    printf("\n\n");
}

int main(int argc, char *argv[])
{
    // no directory or filename specified
    if (argc != 2)
    {
        // remove "./" from printing infront of program name in "./(program)" cmd
        const char *program = strrchr(argv[0], '/');
        program = (program != NULL) ? program + 1 : argv[0];

        printf("usage: %s directory_name\n", program);
        exit(EXIT_FAILURE);
    }

    // error handling for non-existent dir
    DIR *dir = opendir(argv[1]);
    if (!dir)
    {
        printf("can't open %s\n", argv[1]); // "can't open (directory)"
        exit(EXIT_FAILURE);
    }

    // open provided dir
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        print_file_info(argv[1], entry->d_name);
    }

    // close when done
    closedir(dir);
    return 0;
}