#include <iostream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<stdio.h>
using namespace std;

void my_cal(string &query_str)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	strcpy(buf,query_str.c_str());
	vector<string> _vec;
	int data1=0;
	int data2=0;
	int sum=0;
	int flag=0;

	char *p=&buf[strlen(buf)-1];
	while(p!=buf)
	{
		if((*p=='=')&&(flag==0))
		{
			p++;
			data2=atoi(p);
			sum+=data2;
			flag=1;
			p--;
			p--;
		}
		if((flag==1)&&(*p=='='))
		{
			p++;
			data1=atoi(p);
			sum+=data1;
			break;
		}
		p--;
	}
	cout<<data1<<'+'<<data2<<'='<<sum<<endl;
//	while(p != buf)
//	{
//		if(*p == '&')
//		{
//			_vec.push_back(p+1);
//			*p='\0';
//		}
//		p--;
//	}
//	_vec.push_back(buf);
//
//	int size=_vec.size();
//	int i=size-1;
//	string val;
//
//	for(;i>=0;i--)
//	{
//		val+=&(_vec[i][6]);
//		if(i!=0)
//		{
//			val+="+";
//		}
//		sum+=atoi(&_vec[i][6]);
//	}	
//	cout<<val<<"="<<sum<<endl;
}

int main()
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	string method;
	string query_str;
	string content_len;

	if(getenv("REQUEST_METHOD"))
	{
		method=getenv("REQUEST_METHOD");
	}
	else
	{
		method="";
	}
	if(method=="GET")
	{
		if(getenv("QUERY_STRING"))
		{
			query_str=getenv("QUERY_STRING");
		}
		else
		{
			query_str="";
		}
	}
	else if(method=="POST")
	{
		char ch;
		int index=0;
		if(getenv("CONTENT_LENGTH"))
		{
			content_len=getenv("CONTENT_LENGTH");
			int len=atoi(content_len.c_str());
			while(len)
			{
				read(0,&ch,1);
				buf[index++]=ch;
				len--;
			}
			query_str=buf;
		}
		else
		{
			content_len="";
		}
	}
	else
	{
		return 1;
	}
	my_cal(query_str);
	return 0;
}



