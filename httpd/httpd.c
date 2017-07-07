#include "httpd.h"

void printferr(const char* file,const char* func,const int line)
{
	printf("file:[%s] function:[%s] line:[%d]\n",file,func,line);
}

void not_found(int sock)
{
	char buf[1024];
	memset(buf,'\0',sizeof(buf));
	sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
	send(sock,buf,strlen(buf),0);
	sprintf(buf,"content-type:text/html\r\n");
	send(sock,buf,strlen(buf),0);
	sprintf(buf,"\r\n");
	send(sock,buf,strlen(buf),0);
}

void echo_errno(int sock,int errno)
{
	switch(errno)
	{
		case 404:
			not_found(sock);
			break;
		default:
			printf("file:[%s] function:[%s] line:[%d]\n",__FILE__,__func__,__LINE__);
	}
}

void usage(const char * proc)
{
	assert(proc);
	printf("usage:%s [ip] [port]\n",proc);
}


int startup(char *ip,int port)
{
	assert(ip);

	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		exit(1);
	}

	int opt=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(port);
	if (strncmp(ip,"any",3)==0)
	{
		local.sin_addr.s_addr=INADDR_ANY;
	}
	else{
		local.sin_addr.s_addr=inet_addr(ip);
	}
	printf("sock=%d\n",sock);
	if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
	{
		printferr(__FILE__,__func__,__LINE__);		
		//exit(2);
	}

	if(listen(sock,_CLIENTSIZE_)<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		exit(3);
	}
	return sock;
}

int get_line(int sock,char*line,int line_size)//一个字节一个字节读
{
	assert(line);

	char ch='a';
	int index=0;
	ssize_t ret=-1;

	while(ch != '\n'&&index<line_size-1)
	{
		ssize_t _s=recv(sock,&ch,1,0);//第一个参数是接收端套接子描述符，第二个参数是接收缓冲区，第三个参数是缓冲区大小，第四个一般为0；
		if(_s>0)
		{
			if(ch=='\r')
			{
				ret=recv(sock,&ch,1,MSG_PEEK);//先窥探下下一个字符
				if(ret>0&&ch=='\n')
				{
					recv(sock,&ch,1,0);
				}
			}
			line[index++]=ch;
		}
		else
		{
			ch='\n';
		}
	}
	line[index]='\0';
	return index;
}

void clear_header(int sock)  //清空接收缓冲区
{
	char buf[_SIZE_];
	buf[0]='\0';
	int _s=-1;
	do
	{
		_s=get_line(sock,buf,sizeof(buf));
	}while(_s>0&&strcmp(buf,"\n"));//保证空行前面读完了
}

void echo_html(int sock,char* path,ssize_t file_size)
{
	printf("open path:%s\n",path);
	int fd=open(path,O_RDONLY);
	if(fd<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}
	char *stats_line="HTTP/1.1 200 ok\r\n\r\n";
	send(sock,stats_line,strlen(stats_line),0);

	if(sendfile(sock,fd,NULL,file_size)<0)
	{
		printferr(__FILE__,__func__,__LINE__);
	}
	close(fd);
}

void execute_cgi(int sock,const char* path,const char* method,char* query_string)
{
	char line[_SIZE_];
	int line_num=-1;
	int content_length=-1;
	int line_size=_SIZE_;

	int input_cgi[2];
	int output_cgi[2];
	char method_env[_SIZE_];
	char query_string_env[_SIZE_*2];
	char content_len_env[_SIZE_];
	char sockenv[20];
	
	memset(method_env,'\0',sizeof(method_env));
	memset(query_string_env,'\0',sizeof(query_string_env));
	memset(content_len_env,'\0',sizeof(content_len_env));
	memset(sockenv,'\0',sizeof(sockenv));
	
	if(strcasecmp(method,"GET")==0)
	{
		clear_header(sock);
	}
	else  
	{
		do
		{
			line_num=get_line(sock,line,line_size);	
			if(line_num>0&&strncasecmp(line,"content-length: ",16))
			{
				content_length=atoi(&line[16]);
			}
		}while(line_num>0&&strcmp(line,"\n")!=0);

		if(content_length==-1)
		{
			printferr(__FILE__,__func__,__LINE__);
			return;
		}
	}
	sprintf(line,"HTTP/1.0 200 ok\r\n\r\n");
	send(sock,line,strlen(line),0);   

	//把输入输出重定向成sock（浏览器），这里我们用管道
	if(pipe(input_cgi)<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}
	if(pipe(output_cgi)<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}

	pid_t id;
	if((id=fork())<0)
	{	
		printferr(__FILE__,__func__,__LINE__);
		return;
	}
	else if(id == 0)
	{
		close(input_cgi[1]);
		close(output_cgi[0]);
			
		dup2(input_cgi[0],0); 
		dup2(output_cgi[1],1);


		sprintf(sockenv,"SOCKENV=%d",sock);
		putenv(sockenv);//把字符串加到当前环境中,用来改变或增加环境变量的内容

		sprintf(method_env,"REQUEST_METHOD=%s",method);
		putenv(method_env);//把字符串加到当前环境中,用来改变或增加环境变量的内容
		if(strcasecmp(method,"GET")==0)  
		{
			sprintf(query_string_env,"QUERY_STRING=%s",query_string);
			putenv(query_string_env);
		}
		else if(strcasecmp(method,"POST")==0)
		{
			sprintf(content_len_env,"CONTENT_LENGTH=%d",content_length);
			putenv(content_len_env);
		}
		else
		{}
		//execl(path,"sql_cgi_insert",NULL);  
		execl(path,"path",NULL);  

		close(input_cgi[0]);
		close(output_cgi[1]);
	}
	else
	{
		close(input_cgi[0]);
		close(output_cgi[1]);

		char ch;
		if(strcasecmp(method,"POST")==0)
		{
			while(content_length>0)
			{
				recv(sock,&ch,1,0);
				write(input_cgi[1],&ch,1);
				content_length--;
			}
		}
			
		while(read(output_cgi[0],&ch,1)>0)
		{
			send(sock,&ch,1,0);
		}

		waitpid(id,NULL,0);

		close(input_cgi[1]);
		close(output_cgi[0]);
	}	
}

void accept_request(int sock)
{
	int i,j;
	int cgi=0;
	char* query_string=NULL;
	char buf[_SIZE_];
	char method[_SIZE_/2];
	char url[_SIZE_];
	char path[_SIZE_/2];

	memset(buf,'\0',sizeof(buf));
	memset(method,'\0',sizeof(method));
	memset(url,'\0',sizeof(url));
	memset(path,'\0',sizeof(path));

//	while(get_line(sock,buf,sizeof(buf))>0)
//	{
//		printf("%s",buf);
//	}
//	fflush(stdout);

	if(get_line(sock,buf,sizeof(buf))<0)
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}
//  获取方法
	i=j=0;
	while(!isspace(buf[i])&&i<sizeof(buf)&&j<sizeof(method)-1) //判断是否空格
	{
		method[j]=buf[i]; 
		i++;
		j++;
	}

	while(isspace(buf[i]))  //跳过空格
	{
		i++;
	}
	
	//get url
	j=0;
	while(!isspace(buf[i])&&i<sizeof(buf)&&j<sizeof(url)-1)
	{
		url[j]=buf[i];
		i++;
		j++;
	}

	if((strcasecmp(method,"POST")!=0)&&(strcasecmp(method,"GET")!=0))
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}

	if(strcasecmp(method,"POST")==0)
	{
		cgi=1;
	}

	j=0;
	if(strcasecmp(method,"GET")==0)
	{
		query_string=url;
		while(*query_string!='\0'&&*query_string!='?')
		{
			query_string++;
		}

		if(*query_string=='?')
		{
			*query_string='\0';
			query_string++;
			cgi=1;
			printf("GET:%s\n",query_string);
		}
	}
	printf("url:%s\n",url);
	sprintf(path,"htdocs%s",url);

	printf("######path:%s\n",path);

	if(path[strlen(path)-1]=='/')
	{
		strcat(path,_DEFPATH_);
	}

	printf("mypath:%s\n",path);

	struct stat st;
	if(stat(path,&st)<0) //文件不存在
	{
		printferr(__FILE__,__func__,__LINE__);
		return;
	}
	else
	{
		if(S_ISDIR(st.st_mode))//根目录
		{
			strcpy(path,"htdocs/");
			strcat(path,_DEFPATH_);
			printf("S_ISDIR");
			printf("S_ISDIR path:%s\n",path);
		}
		if((st.st_mode&S_IXUSR)||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH))
		{
			cgi=1;
		}
		if(cgi)
		{
			printf("cgicgicgicgicgicg\n");
			printf("mmmmmmmmmmmmmmmmmpath:%s\n",path);
			execute_cgi(sock,path,method,query_string);
		}
		else
		{
			printf("@@@@@@@@@@@@@@@@@@\n");
			clear_header(sock);
			echo_html(sock,path,st.st_size);
		}
		
	}
	close(sock);
}

