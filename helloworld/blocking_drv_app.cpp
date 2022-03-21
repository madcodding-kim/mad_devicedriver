#include <errno.h>  /* for errno */
#include <fcntl.h>  /* for open */
#include <stdio.h>  /*    standard    I/O    */
#include <stdlib.h> /*    for    exit    */
#include <unistd.h> /*    for    read    */
#include <string.h>
#define MAX_BYTES 1024 * 4
#define PROC_ENTRY_FILENAME    "/proc/sleep"

int main(int argc, char *argv[])
{
    int fd;                 /*    The    file    descriptor    for    the    file    to    read    */
    size_t bytes;           /*    The    number    of    bytes    read    */
    char buffer[MAX_BYTES]; /*    The    buffer    for    the    bytes    */

    /*    Open    the    file    for    reading    in    non    blocking    mode    */
    fd = open(PROC_ENTRY_FILENAME , O_RDWR | O_NONBLOCK);

    if (fd == -1)
    {
        puts(errno == EAGAIN ? "Open would block" : "Open failed");
        exit(-1);
    }
    if(argc >= 2)
    {
        if(write(fd, argv[1], strlen(argv[1])) < 0)
        {
            puts("write error");
        }
    }
    
    do
    {
        /*    Read    characters    from    the    file    */
        bytes = read(fd, buffer, MAX_BYTES);
        /*    If    there's    an    error,    report    it    and    die    */
        if (bytes == -1)
        {
            if (errno == EAGAIN)
                puts("Normally    I'd    block,    but    you    told    me    not    to");
            else
                puts("Another    read    error");
            exit(-1);
        }
        /*    Print    the    characters    */
        if (bytes > 0)
        {
            for (int i = 0; i < bytes; i++)
                putchar(buffer[i]);
        }
    } 
    while (bytes > 0);
    
    return 0;
}