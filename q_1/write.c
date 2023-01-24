#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define MEM_SIZE 50

int8_t write_buf[MEM_SIZE];



int main()
{
    int fd;
    fd = open("/dev/write_device", O_RDWR);
        if(fd > 0) {
                printf("Device File Opened successfully in the path /dev/char_driver_device...\n");
        }
        else {
                printf("Cannot open device file...\n");
                return 0;
        }

    printf("Enter the string to write in the Device_file :");
    scanf("  %[^\t\n]s", write_buf);
    printf("Data Writing ...");
    write(fd, write_buf, strlen(write_buf)+1);
    printf("Done!\n"); 
    close(fd);
    printf("Devicefile Closed..\n");
    return 0;   
}