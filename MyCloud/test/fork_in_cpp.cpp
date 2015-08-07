#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>

class CTest
{
public:
	CTest()
	{
		printf("constructor\n");
	}

	CTest(CTest &rhs)
	{
		printf("Copy constructor\n");
	}

	~CTest()
	{
		printf("Destructor\n");
	}
};

int main()
{
	CTest a;

	pid_t pid = fork();

	return 0;
}