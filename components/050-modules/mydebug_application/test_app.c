#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define WR_VALUE _IOW('a','a',u_int64_t*)
#define RD_VALUE _IOR('a','b',u_int64_t*)

int main()
{
	int fd;
	u_int64_t value, number;
	printf("\nOpening Driver\n");
	fd = open("/dev/mydebug_device", O_RDWR);
	if(fd < 0) {
		printf("Cannot open device file...\n");
		return 0;
	}
	printf("Open device file... fd=%d\n", fd);

	printf("Stage 1. IOCTL funcs\n");
	printf("Enter the Value to send\n");
	scanf("%ldd",&number);
	printf("Writing Value to Driver\n");
	ioctl(fd, WR_VALUE, (u_int64_t*) &number);

	printf("Reading Value from Driver\n");
	ioctl(fd, RD_VALUE, (u_int64_t*) &value);
	printf("Value is %ld\n", value);

	printf("Stage 2. Write/Read funcs\n");
	printf("Enter the Value to send\n");
	scanf("%ldd",&number);
	printf("Writing Value to Driver\n");
	write(fd, &number, sizeof(number));

	printf("Reading Value from Driver\n");
	read(fd, &value, sizeof(value));
	printf("Value is %ld\n", value);

	printf("Closing Driver\n");
	close(fd);
}
