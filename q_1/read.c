#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define MEM_SIZE 50

int8_t read_buf[MEM_SIZE];



int main()
{
    int fd;
    fd = open("/dev/read_device", O_RDWR);
        if(fd > 0) {
                printf("Device File Opened successfully in the path /dev/char_driver_device...\n");
        }
        else {
                printf("Cannot open device file...\n");
                return 0;
        }
    printf("Data reading ...");
    read(fd,read_buf,MEM_SIZE);
    printf("Done!\n.....The data is %s\n",read_buf);
    close(fd);
    printf("Devicefile Closed..\n");
    return 0; 
}  