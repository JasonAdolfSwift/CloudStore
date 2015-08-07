#include "../public.h"
#include "client.h"

//\033[34;48m string \033[0m

//default constructor
CClient::CClient()
{
	//创建套接字向服务器发起连接，打印菜单，判断客户的选项，向服务器发起对应的请求即可。连接建立并且登录
	//成功之后便进入命令行模式(使用echo模式，服务器向客户端发送一段字符串，在此模式下，客户端可以输入命令)
	//输入的命令会通过套接字发送给服务器，服务器接收到客户端发来的数据之后便解析该数据，执行对应的命令之后
	//将执行结果通过套接字发送给客户端即可。如果客户端进行下载或者上传时，客户端和服务器会创建另外一个连接
	//专门用来下载和上传(下载和上传可以使用两个不同的端口实现，这样的话便会有3个端口号，分别为：UI端口，
	//下载端口，上传端口。UI端口一直连接着，直到用户退出的客户端的时候连接才会断开。而下载和上传连接只有在
	//客户端下载和上传的过程中保持连接，当客户端停止下载和上传之后便断开连接)。

	/*
		发起与服务器端的链接
	*/

	/*
		选择登录选项 1.登录 2.注册
	*/
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_sockfd == -1)
	{
		cerr<<"Create socket failed!\n";
	}

	struct sockaddr_in serAddr;
	struct sockaddr_in cliAddr;
	memset(&serAddr, 0, sizeof(serAddr));

	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(CTP);
	serAddr.sin_addr.s_addr = inet_addr("192.168.1.31");

	int res = connect(m_sockfd, (struct sockaddr*)&serAddr, sizeof(serAddr));
	if (res == -1)
	{
		cerr<<"connect failed\n";
	}
	Menu(); 
}

//This is Menu interface of client
void CClient::Menu()
{
	int choose = 0;

	cout<<"Welcome to cloud storage system"<<endl;
	cout<<"1.Login"<<endl;
	cout<<"2.Registration"<<endl;
	cout<<"Please choose what you want to do: [ ]\b\b";

	cin>>choose;

	switch(choose)
	{
		case 1: ClientLogin();break;
		case 2: ClientRegistration();break;
		default:
				cout<<"Error input"<<endl;break;
	}
}


void CClient::ClientLogin()
{
	// bool logining = true;
	// while (logining)
	// {
		Userinfo user;
		user.model = LOG;
		cout<<"Please input username:";
		//fgets(user.userName, BUFSIZE, stdin);	
		//user.userName[strlen(user.userName)-1] = '\0';
		fflush(stdin);
		cin>>user.userName;

		cout<<"Please input password:";
		//fgets(user.password, BUFSIZE, stdin);	
		//user.password[strlen(user.password)-1] = '\0';
		fflush(stdin);
		cin>>user.password;

		strcpy(m_name, user.userName);

		int res = send(m_sockfd,&user,sizeof(user),0);
		if(res == -1)
		{
			cerr<<"Send error\n";
		}
		char buff[BUFSIZE] = {0};
		//延时是否需要?
		LOGIN_STA sta = USRERR;
		recv(m_sockfd,&sta,sizeof(sta),0);
		switch(sta)
		{
			case USRERR:
					cout<<"\033[0;41;1m Can't find user \033[0m"<<endl;
					break;
			case PASSERR:
					cout<<"\033[0;41;1m Wrong password \033[0m"<<endl;
					break;
			case USRSUC:
					cout<<"\033[42;37;1m Login succeed \033[0m"<<endl;
					cout<<"\n\n\033[46;30;1m ^_^ Welcome to HerCloud ^_^\033[0m"<<endl;
					UserOperation();
					//logining = false;//表示用户已经结束了本次登录
					break;
			// case ILLEGAL:
			// 		cout<<"\033[0;41;1m Login failed \033[0m"<<endl;
			// 		logining = false;
			// 		break;
			default:
					break;
		}
	//}
}

//用户注册，输入用户名和密码，在服务器端注册用户。
void CClient::ClientRegistration()
{
	Userinfo user;
	user.model = REG;
	cout<<"Please input username:";
	cin>>user.userName;
	//fgets(user.userName, BUFSIZE, stdin);	
	//user.userName[strlen(user.userName)-1] = '\0';

	cout<<"Please input password:";
	cin>>user.password;
	//fgets(user.password, BUFSIZE, stdin);	
	//user.password[strlen(user.password)-1] = '\0';
	
	int res = send(m_sockfd,&user,sizeof(user),0);
	if(res == -1)
	{
		cerr<<"Reg Send error\n";
	}	
}


void CClient::UserOperation()
{
	char command[BUFSIZE] = {0};
	char echobuf[BUFSIZE*10] = {0};
	bool running = true;
	int nbytes = 0;

	CreateDirectionText();
	MakeDirectionTree();
	close(m_sockfd);
	current_pos = file_tree.GetRootDirection();

	while (running)
	{
		
		//fgets(command, BUFSIZE, stdin);
		//command[strlen(command)-1] = '\0';
		//
		//gets(command);
		
		printf("[HerCloud]$");
		fflush(stdin);
		gets(command);
		
		if (strncmp(command, "exit", 4) == 0)
		{
			running = false;
		}

		if (strncmp(command, "ls", 2) == 0)
		{
			ls();
		}
		else if (strncmp(command, "cd", 2) == 0)
		{
			char paramter[BUFSIZE] = {0};

			if (!GetCDParameters(command, paramter))
			{
				memset(command, 0, BUFSIZE);
				continue;
			}

			if ( !cd(paramter) )
			{
				printf("In Function \" UserOperation\" : direction named %s dosen't exisit\n", paramter);
			}
		}
		
		/*
		nbytes = send(m_sockfd, command, strlen(command), 0);
		if (nbytes == -1)
		{
			cerr<<"UserOperation send info failed\n";
			running = false;
		}
		
		if (running)
		{
			
			nbytes = recv(m_sockfd, echobuf, BUFSIZE*10, 0);
			if (nbytes == -1)
			{
				cerr<<"UserOperation recv info failed\n";
				running = false;
			}
			
			if (nbytes > 0) cout<<echobuf<<endl;
		}
		*/

		memset(command, 0, BUFSIZE);
	}
}

void CClient::MakeDirectionTree()
{
	new (&file_tree) FileStruct(m_name);

	ifstream userfile;
	userfile.open("./.mydirinfo", ios::in);
	char buffer[BUFSIZE] = {0};

	while (!userfile.eof())
	{
		userfile.getline(buffer, BUFSIZE-1);
		if (buffer[0] == '\0') break;

		file_tree.AddFile(buffer);

		memset(buffer, 0, BUFSIZE);
	}
}

void CClient::CreateDirectionText()
{
	int nbytes = 0;
	char recvBuff[BUFSIZE] = {0};

	int fd = open("./.mydirinfo", O_WRONLY|O_CREAT|O_TRUNC, 0666);	

	while ( (nbytes = recv(m_sockfd, recvBuff, 64, 0)) > 0)
	{
		int writeBytes = write(fd, recvBuff, nbytes);
		if (writeBytes != nbytes)
		{
			cerr<<"write direction information error"<<endl;
		}
		if (nbytes < 64)
		{
			break;
		}
	}

	close(fd);
}

void CClient::ls()
{
	file_tree._ls(current_pos);
}
	
bool CClient::cd(const char *pathname)
{
	return file_tree._cd(current_pos, pathname);
}

bool CClient::GetCDParameters(const char *command, char *arr)
{
	if (command == NULL || arr == NULL)
	{
		return false;
	}
	const char *ptr = command;
	const char *tail = NULL;
	int count = 0;

	while (*ptr++ != '\0')
	{
		if (*ptr == ' ')
		{
			++count;
			if (count > 1) return false;

			strcpy(arr, ptr+1);
		}
	}

	return true;
}
