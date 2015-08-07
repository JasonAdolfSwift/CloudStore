#include "file.h"

static bool g_tag = true;

void FileStruct::AddFile(std::string path)
{
	char filePath[BUFSIZE] = {0};//从dirinfo中读取的一行
	int nbytes = strlen(path.c_str());
	int copy_size = nbytes < BUFSIZE-1 ? nbytes : BUFSIZE-1;
	strncpy(filePath, path.c_str(), copy_size);

	char currentFileName[BUFSIZE] = {0};//文件名称
	f_type fileType;
	std::list<__file_node> *parent = &root;
	std::list<__file_node> *pre_list = root.front().sub_dir;

	char * sptr = filePath;

	while ( GetCurrentFileAttribute(fileType, currentFileName, sptr) )//找到文件插入的位置
	{
		if ( strcmp(currentFileName, ".dirinfo") == 0) continue;
		bool isFind = false;
		std::list<__file_node> &pos = FindNodeByName(currentFileName, isFind, *pre_list);
		
		__file_node *tmp = NULL;

		if (fileType == DIRE)
		{
			__file_node *tmp = _buynode(currentFileName, fileType, parent);
			if (tmp == NULL)
			{
				cerr<<"In Function \"AddFile\" : buy node failed"<<endl;
			}
			pre_list = tmp->sub_dir;
		}

		if (!isFind)
		{
			if (fileType == REGU)
			{
				pos.push_back( *(_buynode(currentFileName, fileType, parent)));
			}
			else
			{
				pos.push_back(*tmp);
				parent = &pos;
			}
		}

		memset(currentFileName, 0, BUFSIZE);
	}
}

std::list<__file_node>& FileStruct::FindNodeByName(char * name, bool &isFind, std::list<__file_node> &pos)
{
	std::list<__file_node>::iterator ite = pos.begin();

	string fileName(name);

	for (; ite != pos.end(); ++ite)
	{
		if (fileName == (*ite).oldname)
		{
			isFind = true;
		}
	}

	isFind = false;
	return pos;
}

//逐层获得文件名,并获取文件属性
bool FileStruct::GetCurrentFileAttribute(f_type &type, char *curName, char *&filePath)
{
	if (curName == NULL || filePath == NULL)
	{
		cerr<<"In Function \"GetCurrentFileAttribute\": curName or filePath can't be NULL"<<endl;
		return false;
	}

	char *head = NULL;
	if (*filePath == '\0')
	{
		return false;
	}
	else if (*filePath == '/')
	{
		if (*(filePath + 1) == '\0')
			return false;
	}

	head = filePath + 1;
	
	if (g_tag)//第一次直接将head定位到第一个'/'之后，因为第一个文件是根节点，根节点是根据用户名创建的，不用在这里创建
	{
		while (*head++ != '/');
	}

	char * tail = head;

	while (*tail != '\0' && *tail != '/')
	{
		++tail;
	}
	--tail;

	int copy_size = tail - head + 1;
	strncpy(curName, head, copy_size);

	filePath = tail + 1;

	if (*filePath == '/') type = DIRE;
	else if (*filePath == '\0') type = REGU;
	else
	{
		cerr<<"In Function \"GetCurrentFileAttribute\": type error"<<endl;
	}

	return true;
}
