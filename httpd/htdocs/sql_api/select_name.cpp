#include "sql_api.h"

int main()
{
	char method[1024];
	char buf[1024];
	memset(method,'\0',sizeof(method));
	memset(buf,'\0',sizeof(buf));
	string query_string;
	string content_len;
	if(getenv("REQUEST_METHOD"))
	{
		strcpy(method,getenv("REQUEST_METHOD"));
		cout<<"method:"<<method<<endl;
	}
	else
	{
		cout<<"get method faild"<<endl;
	}

	if(strcasecmp(method,"GET")==0)
	{
		query_string += getenv("QUERY_STRING");
		cout<<"query_string:"<<query_string<<endl;
	}
	else
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
			query_string+=buf;
		}
	}
	
	vector<string> ret;

	get_data(query_string,ret);
	cout<<"query_string:"<<query_string<<endl;
	cout <<"ret[0]:"<<ret[0]<<endl;
	sql_api _sa;
	_sa._connect_mysql();
	_sa._select_mysql_name(ret[0]);
	return 0;
}
