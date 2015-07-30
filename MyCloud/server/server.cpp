#include "public.h"
#include "server.h"
#include "processpool.h"


static char * g_str = NULL;

//extern int CServer::m_epollfd;

//Public:
CServer::CServer():m_userstat(false)
{

	// while (1)
	// {
	// 	int len = sizeof(cliAddr);
	// 	cout<<"Waitting for connect\n";

	// 	//可以在这里添加进程池或线程池来管理链接，主线程/主进程只创建监听队列并调用accpet()，
	// 	//然后便将链接分发给线程池/进程池中进行处理。

		 

	// 	int cfd = accept(m_sockfd, (struct sockaddr*)&cliAddr, (socklen_t *)&len);
	// 	if (cfd == -1)
	// 	{
	// 		cerr<<"Accept Error!\n";
	// 	}

	// 	int n = recv(cfd, &info, sizeof(info), 0);
	// 	if ( n == -1)
	// 	{
	// 		cerr<<"Recv Error\n";	
	// 	}

	// 	CheckLogin(info, cfd);
	// }
}

CServer::~CServer()
{}

void CServer::init(int epollfd, int sockfd, const sockaddr_in& client_addr)
{
	m_epollfd = epollfd;
	m_sockfd = sockfd;
	m_address = client_addr;
}

void CServer::process()
{
	if (!m_userstat)
	{
		CheckLogin(m_sockfd);
	}
	else
	{
		UserInterface(m_sockfd);
	}
}

//Private:
void CServer::CheckLogin(int cfd)
{
	char buffer[BUFSIZE] = {0};
	char userName[BUFSIZE] = {0};

	Userinfo info;

	int n = recv(cfd, &info, sizeof(info), 0);
	if ( n == -1)
	{
		cerr<<"Recv Error\n";	
	}

	if (info.model == LOG)
	{
		ifstream userfile;
		userfile.open("/home/Swift/Desktop/MyCloud/userinfo");
		if ( !userfile )
		{
			cerr<<"Open userinfo failed"<<endl;
			exit(1);
		}

		LOGIN_STA statu = USRERR;//用户登录状态
		while ( !userfile.eof() )
		{
			userfile.getline(buffer, 127);

			char * pstr = buffer;
			bool nameStatu = false;
			bool passStatu = false;
		
			char name[BUFSIZE] = {0};
			char passwd[BUFSIZE] = {0};

		
			pstr = strtok(buffer, ":");
			strcpy(name, pstr);

			pstr = strtok(NULL, ":");
			strcpy(passwd, pstr);
		
			if (strcmp(name, info.userName) == 0)
			{
				statu = PASSERR;
				if (strcmp(passwd, info.password) == 0)
				{
					statu = USRSUC;
					strcpy(userName, name);
					m_userstat = true;
					break;
				}
			}
			memset(buffer, 0, 128);
		}//end of while(!userfile.eof())

		send(cfd, &statu, sizeof(statu), 0);

		if (m_userstat)
		{
			MakeDirectionText(userName, userName);
			char title[] = "\n\n\033[46;30;1m ^_^ Welcome to HerCloud ^_^\033[0m";
			int nbytes = send(cfd, title, strlen(title), 0);
			assert (nbytes != -1);
		}

		userfile.close();
	}//end of if (info.model == LOG)

	else//用户注册
	{
		ofstream userfile;
		//ios_base::out会清空当前文件所有数据,然后再向文件写数据。ios_base::app会以添加的方式打开文件
		userfile.open("/home/Swift/Desktop/MyCloud/userinfo", ios_base::app);

		if (!userfile)
		{
			cerr<<"Open userinfo failed\n";
			exit(1);
		}

		char buffer[BUFSIZE] = {0};

		strcpy(buffer, info.userName);
		strcat(buffer, ":");
		strcat(buffer, info.password);
		strcat(buffer, "\n");

		userfile<<buffer;
		userfile.close();
	}
}

void CServer::UserInterface(int cfd)
{
	char userCommand[BUFSIZE] = {0};
	char echoBuffer[BUFSIZE*10] = {0};
	int nbytes = 0;
	bool running = true;

	while (running)
	{
		nbytes = recv(cfd, userCommand, BUFSIZE-1, 0);
		if (nbytes == -1)
		{
			if (errno != EAGAIN)//发生了错误,并且错误不是EAGAIN,说明recv真正失败,需要结束本次链接。
			{
				removefd(m_epollfd, m_sockfd);
			}
			break;
		}
		else if (nbytes == 0)//说明recv没有读到任何数据
		{
			removefd(m_epollfd, m_sockfd);
		}
		else
		{
			if (strncmp(userCommand, "exit", 4) == 0)
			{
				running = false;
				m_userstat = false;
			}

			cout<<userCommand<<endl;
			if (running)
			{
				//temp operation !!!!
				nbytes = send(cfd, "OK", 2, 0);
				if (nbytes == -1)
				{
					cerr<<"UserInterface send failed\n";
				}
			}
		}

		memset(userCommand, 0, BUFSIZE);
		memset(echoBuffer, 0, BUFSIZE*10);
	}
}


void CServer::MakeDirectionText(char *userName, char *path)
{
	assert(userName != NULL && path != NULL);

	static bool tag = true;

	char filePath[BUFSIZE] = "/home/Swift/Desktop/MyCloud/server/dir/";	//将每个文件的路径保存在这个文本中	
	strcat(filePath, userName);
	strcat(filePath, "/dirinfo");
	int fd = open(filePath, O_WRONLY|O_APPEND|O_CREAT, 0666);

	char pathBuf[BUFSIZE] = {0};											//用来保存每个文件的路径    

	// if ( getcwd(userPath, BUFSIZE) == NULL)
	// {
	// 	perror("Getcwd Error!\n");
	// 	return ;
	// }

	// char filePath[BUFSIZE] = {0};											
	// strcpy(filePath, userPath);
	// strcat(filePath, "/");
	// strcat(filePath, userName);
	// strcat(filePath, "/");
	// strcat(filePath, "dirinfo");

	char userPath[BUFSIZE] = {0};					//server端当前工作目录

	if ( tag )
	{
		strcpy(userPath, "/home/Swift/Desktop/MyCloud/server/dir/");
		strcat(userPath, userName);
		tag = false;
	}
	else
	{
		if ( getcwd(userPath, BUFSIZE) == NULL)
		{
			perror("getcwd error\n");
			return ;
		}
		strcat(userPath, "/");
		strcat(userPath, path);
	}

	chdir(userPath);

	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;

	if ( (dp = opendir(userPath)) == NULL )
	{
		perror("Opendir Error\n");
		printf("%s\n", userPath);
		return ;
	}

	memset(userPath, 0, BUFSIZE);// temp

	while ( (entry = readdir(dp)) != NULL )
	{
		if ( getcwd(userPath, BUFSIZE) == NULL )
		{
			perror("getcwd error!\n");
			return ;
		}
		lstat(entry->d_name, &statbuf);

		if ( S_ISDIR(statbuf.st_mode) )
		{
			if ( strcmp(".", entry->d_name)==0 || strcmp("..", entry->d_name)==0 )
			{
				continue;
			}

			MakeDirectionText(userName, entry->d_name);//如果是目录，就递归调用MakeDirectionText,遍历子目录的文件
		}

		strcat(userPath, "/");
		strcat(userPath, entry->d_name);
		char *pstr = mystrtok(userPath, "/");

		while ( (pstr = mystrtok(NULL, "/")) != NULL )
		{
			if ( strncmp(pstr, userName, strlen(userName)) == 0 )
			{
				strcpy(pathBuf, pstr);
				strcat(pathBuf, "\n");
				break;
			}
		}

		if (pstr != NULL)
		{
			int nbytes = write(fd, pathBuf, strlen(pathBuf));

			memset(userPath, 0, BUFSIZE);
			memset(pathBuf, 0, BUFSIZE);

			if (nbytes < 0)
			{
				perror("write to DirectionText error\n");
				return ;
			}
		}
	}

	chdir("..");
	closedir(dp);
	close(fd);

	// tag--;
	// if (tag == 0)
	// {
	// 	close(fd);
	// }
}

char* CServer::mystrtok(char *str, const char *delim)
{
	if (delim == NULL) return NULL;

	if (str == NULL) str = g_str;

	size_t len = strlen(delim);

	char *rt_ptr = NULL;

	while (str++ != NULL)
	{
		if (strncmp(str, delim, len) == 0)
		{
			g_str = str+1;
			rt_ptr = str+1;
			break;
		}
	}

	return rt_ptr;
}

int main()
{
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		cerr<<"Create socket failed!\n";
	}

	struct sockaddr_in serAddr;
	struct sockaddr_in cliAddr;
	memset(&serAddr, 0, sizeof(serAddr));

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(CTP);
	serAddr.sin_addr.s_addr = inet_addr("192.168.1.31");

	int res = bind(listenfd, (struct sockaddr*)&serAddr, sizeof(serAddr));

	if (res == -1)
	{
		cerr<<"Bind Error!";
		exit(1);
	}

	listen(listenfd, 5);


	//////////////////////////////////////
	processpool< CServer > *pool = processpool< CServer >::create(listenfd);

	if ( pool != NULL )
	{
		pool->run();
		delete pool;
	}	
	return 0;
}
