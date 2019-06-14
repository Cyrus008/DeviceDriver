#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
 
int8_t write_buf[1024];
int8_t read_buf[1024];
int main()
{
	printf("\n");        
        printf("*******WWW.core_embeddedLinux.com*******\n");
 	int fd = open("/dev/parth_maurya", O_RDWR);
        if(fd < 0) {
                printf("Cannot open device file...\n");
                return 0;
        	}
 
            printf("Enter the string  :");
            scanf("  %[^\t\n]s", write_buf);
            printf("Data Writing ...");
            write(fd, write_buf, strlen(write_buf)+1);
            printf("Done!\n");
            printf("Data Reading ...");
            read(fd, read_buf, 1024);
            printf("Done!\n\n");
            printf("Data = %s\n\n", read_buf);
       close(fd);
}
