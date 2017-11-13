#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>

#define BUFFER_LENGTH 8000
static char buffer[BUFFER_LENGTH];
int main(){
	int res, fd;

	fd = open("/dev/process_list", O_RDONLY);			//open process_list with read only permission
	if (fd < 0)
	{
		perror("device failed to open\n");
		return errno;
	}

	res = read(fd, buffer, BUFFER_LENGTH);				//read from process_list
	if (res < 0)
	{
		perror("failed to read from the device\n");
		return errno;
	}

	printf("\n%s\n", buffer);					//print the content of buffer

	close(fd);							//close the opened device

	return 0;
}
