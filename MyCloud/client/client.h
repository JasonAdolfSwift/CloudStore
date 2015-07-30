#include "public.h"

class CClient
{
public:
	CClient();
	~CClient(); 
private:
	
	int m_sockfd;

	//This is Menu interface of client
	void Menu();
	void ClientLogin();
	void ClientRegistration();
	void UserOperation();
};
