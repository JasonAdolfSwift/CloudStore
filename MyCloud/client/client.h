#include "../public.h"
#include "file.h"

class CClient
{
public:
	CClient();
	~CClient()
	{
		shutdown(m_sockfd, SHUT_RDWR);
	} 

public:
	void ls();
	bool cd(const char *pathname);
private:
	int m_sockfd;
	char m_name[BUFSIZE];
	FileStruct file_tree;
	std::list<__file_node>* current_pos;

private:
	//This is Menu interface of client
	void Menu();
	void ClientLogin();
	void ClientRegistration();
	void UserOperation();
	void CreateDirectionText();
	void MakeDirectionTree();
	bool GetCDParameters(const char *command, char *arr);
};
