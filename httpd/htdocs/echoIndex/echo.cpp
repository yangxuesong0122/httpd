#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/socket.h>

using namespace std;

void my_cal(string &query_str,int sockIndex)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	strcpy(buf,query_str.c_str());
	const char* article=NULL;
	
	char path[50];

	char *ptr=&buf[strlen(buf)-1];
	while(ptr!=buf)
	{
		if(*ptr=='=')
		{
			ptr++;
			article=ptr;
			break;
		}
		ptr--;
	}

//	cout<<"article:"<<article<<endl;
	
	memset(path,'\0',sizeof(path));
	sprintf(path,"htdocs/echoIndex/%s",article);
	
//	cout<<path<<endl;

	int filefd=open(path,O_RDONLY);
	if(filefd<0)
	{
		cout<<"open faild"<<endl;
		perror("open:");
	}

	struct stat stat_buf;
	fstat(filefd,&stat_buf);

	char *stat_line="HTTP/1.1 200 ok\r\n\r\n";
	send(sockIndex,stat_line,strlen(stat_line),0);

	sendfile(sockIndex,filefd,NULL,stat_buf.st_size);
	close(filefd);
}

int main()
{
	string method;
	string query_str;
	string sock;
	int sockIndex=0;

//	cout<<"$$$$$$$$$$$$$$$$"<<endl;

	if(getenv("REQUEST_METHOD"))
	{
		method = getenv("REQUEST_METHOD");
	//	cout<<method<<endl;
	}
	else{
		method="";
	}

	if(getenv("SOCKENV"))
	{
		sock=getenv("SOCKENV");
		sockIndex=atoi(sock.c_str());
	//	cout<<sockIndex<<endl;
	}

	if(method=="GET")
	{
		if(getenv("QUERY_STRING"))
		{
			query_str+=getenv("QUERY_STRING");
	//		cout<<query_str<<endl;
		}
		else
		{
			query_str="";
		}
	}

	my_cal(query_str,sockIndex);
	return 0;
}
