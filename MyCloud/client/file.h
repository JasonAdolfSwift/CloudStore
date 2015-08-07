#ifndef __FILESTRUCT__H
#define __FILESTRUCT__H

#include "../public.h"
#include <list>

static bool g_tag = true;

typedef enum f_type
{
	REGU=0,
	DIRE=1
}f_type;

typedef enum ISFIND
{
	FIND=0,
	NOTFIND=1,
	ALREADY=3
}ISFIND;

class __file_node
{
public:
	__file_node(std::string &name, f_type t, std::list<__file_node> *parent)
	{
		oldname = name;
		newname = "";
		type = t;
		dirty = false;
		par_dir = parent;

		if (type == DIRE)
		{
			sub_dir = new std::list<__file_node>();
			//(*sub_dir).push_back( *(_buynode("..", DIRE, parent)) );//每次创建一个新的list时，要给它插入一个指向父list的节点
		}
		else
		{
			sub_dir = NULL;
		}
	}
public:
	std::string oldname;
	std::string newname;
	f_type type;
	std::list<__file_node> *sub_dir;
	std::list<__file_node> *par_dir;
	bool dirty;
};

class FileStruct
{
private:
	static __file_node* _buynode(std::string name, f_type t, std::list<__file_node>* parent)
	{ 
		__file_node *pnode = new __file_node(name, t, parent);
		return pnode;
	}

public://constructor
	FileStruct() {}
	FileStruct(std::string name)
	{ 
		root.push_back( *(_buynode(name, DIRE, NULL)) ); //根节点一定是一个目录,并且文件名是用户名
	}
	~FileStruct()
	{
		//待完成
	}

public://member function
	void AddFile(std::string path)
	{
		g_tag = true;
		char filePath[BUFSIZE] = {0};//从dirinfo中读取的一行
		int nbytes = strlen(path.c_str());
		int copy_size = nbytes < BUFSIZE-1 ? nbytes : BUFSIZE-1;
		strncpy(filePath, path.c_str(), copy_size);

		char currentFileName[BUFSIZE] = {0};//文件名称
		f_type fileType;
		std::list<__file_node> *parent = &root;
		std::list<__file_node> *pre_list = root.front().sub_dir;
		__file_node *cur_node = NULL;

		char * sptr = filePath;
		__file_node *tmp = NULL;

		while ( GetCurrentFileAttribute(fileType, currentFileName, sptr) )
		{
			if ( strcmp(currentFileName, ".dirinfo") == 0) continue;
			bool isFind = false;
			std::list<__file_node> &pos = FindNodeByName(currentFileName, isFind, *pre_list, cur_node);//找到文件插入的位置

			if (!isFind)
			{
				if (fileType == REGU)
				{
					pos.push_back( *(_buynode(currentFileName, fileType, parent)));
				}
				else
				{
					tmp = _buynode(currentFileName, fileType, parent);
					if (tmp == NULL)
					{
						cerr<<"In Function \"AddFile\" : buy node failed"<<endl;
					}
					pos.push_back(*tmp);

					pre_list = tmp->sub_dir;
					pre_list->push_back(*(_buynode("..", REGU, &pos)));
					parent = &pos;
				}
			}
			else
			{
				if (fileType == DIRE)
				{
					pre_list = cur_node->sub_dir;
					parent = &pos;
				}
			}

			memset(currentFileName, 0, BUFSIZE);
		}
	}

	std::list<__file_node>* GetRootDirection() { return root.front().sub_dir; }

	void _ls(std::list<__file_node>* current_pos)
	{
		std::list<__file_node>::iterator ite = (*current_pos).begin();

		for (; ite != (*current_pos).end(); ++ite)
		{
			if (strncmp((*ite).oldname.c_str(), ".", 1) == 0) continue;

			if ((*ite).type == DIRE)
			{
				printf("\033[34;48m%s  \033[0m", (*ite).oldname.c_str());
			}
			else
			{
				printf("%s  ", (*ite).oldname.c_str());
			}
		}
		printf("\n");
	}

	bool _cd(std::list<__file_node>* &current_pos, const char * pathname)
	{
		string path(pathname);
		std::list<__file_node>::iterator ite = (*current_pos).begin();

		if (strncmp(pathname, "..", 2) == 0 )
		{
			if ((*ite).par_dir != NULL)
			{
				current_pos = (*ite).par_dir;
			}
			return true;
		}
		else
		{
			for (; ite != (*current_pos).end(); ++ite)
			{
				if ((*ite).oldname==path && (*ite).type==DIRE)
				{
					current_pos = (*ite).sub_dir;
					return true;
				}
			}
		}

		return false;
	}
private:
	std::list<__file_node>& FindNodeByName(char * name, bool &isFind, std::list<__file_node> &pos, __file_node *&cur_node)
	{
		std::list<__file_node>::iterator ite = pos.begin();

		string fileName(name);

		for (; ite != pos.end(); ++ite)
		{
			if (fileName == (*ite).oldname)
			{
				cur_node = &(*ite);
				isFind = true;
				return pos;
			}
		}

		isFind = false;
		return pos;
	}

	//逐层获得文件名,并获取文件属性
	bool GetCurrentFileAttribute(f_type &type, char *curName, char *&filePath)
	{
		if (curName == NULL || filePath == NULL)
		{
			cerr<<"In Function \"GetCurrentFileAttribute\": curName or filePath can't be NULL"<<endl;
			return false;
		}

		if (g_tag)//第一次直接将head定位到第一个'/'之后，因为第一个文件是根节点，根节点是根据用户名创建的，不用在这里创建
		{
			while (*filePath != '/')
			{
				++filePath;
			}
			g_tag = false;
		}
	
		if (*filePath == '\0')
		{
			return false;
		}
		else if (*filePath == '/')
		{
			if (*(filePath + 1) == '\0')
				return false;
		}

		char * head = NULL;

		if (*filePath == '/') 
			head = filePath + 1;
		else 
			head = filePath;

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
private:
	std::list<__file_node> root;
};

#endif
