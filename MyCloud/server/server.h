#ifndef SERVER_H
#define SERVER_H

#include "../public.h"

class CServer
{
public:
	CServer();
	~CServer();
	void init(int epollfd, int sockfd, const sockaddr_in& client_addr);
	void process();
private:
	void CheckLogin(int cfd);
	void UserInterface(int cfd);
	static void MakeDirectionText(char *userName, char *path);
	static char* mystrtok(char *str, const char *delim);
	void CheckLoginOrRegister(int cfd);
	void UserLogin(int cfd, Userinfo &userinfo);
	void UserRegister(int cfd, Userinfo &userinfo);
	void SendDirectionText(int cfd, char *userName);
private:
	//int m_listenfd;//监听套接字

	int m_sockfd;//已连接套接字
	bool m_userstat;//本次服务对应的客户端状态

	//string m_serAddr;

	sockaddr_in m_address;
	static const int BUFFER_SIZE = 1024;
	static int m_epollfd;
};

int CServer::m_epollfd = -1;

#endif
