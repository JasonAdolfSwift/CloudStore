#include <>

static int nchildren;
static pid_t *pids;

void sig_int(int);
pid_t child_make(int, int, int);

int Tcp_listen(socklen_t *addrlen)
{
	int resListenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (resListenfd == -1)
	{
		perror("Create socket error\n");
		exit(-1);
	}

	struct sockaddr_in serAddr;
	memset(&serAddr, 0, sizeof(serAddr));

	struct sockaddr_in cliAddr;
	memset(&cliAddr, 0, sizeof(cliAddr));

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(6000);
	serAddr.sin_addr.s_addr = inet_addr(192.168.1.31);

	int bindRes = bind(resListenfd, (struct sockaddr*)&serAddr, sizeof(serAddr));
	if (bindRes == -1)
	{
		perror();
	}
}

int main(int argc, char**argv)
{
	int listenfd,i;

	socklen_t addrlen;



}