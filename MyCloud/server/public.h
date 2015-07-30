#ifndef PUBLIC_H
#define PUBLIC_H

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <assert.h>
#include <dirent.h>


///////////////////////
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 

///////////////////////
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
////////////////////////

#define  CTP       6500
#define  DLP       6501
#define  ULP       6502

#define  BUFSIZE   128

using namespace std;

typedef enum MOD 
{
	LOG=0,REG=1
}MOD;


typedef enum LOGIN_STA
{
	USRSUC=0,
	PASSERR=1,
	USRERR=2
}LOGIN_STA;

typedef struct Userinfo
{
	char userName[BUFSIZE];
	char password[BUFSIZE];
	MOD model;
}Userinfo;


#endif
