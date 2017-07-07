#include "httpd.h"
#define _MAX_ 64
//void* threadrun(void *sock)
//{
//	int new_sock=(int)sock;
//	accept_request(new_sock);
//	return NULL;
//}

void epoll_http(int listen_sock)
{
	int epoll_fd=epoll_create(256);
	if(epoll_fd<0)
	{
		perror("epoll_create");
		exit(1);
	}
	
	struct epoll_event ev;
	struct epoll_event ret_ev[_MAX_];
	int ret_num=_MAX_;
	int read_num=-1;
	ev.events=EPOLLIN;
	ev.data.fd=listen_sock;
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_sock,&ev)<0)
	{
		perror("epoll_ctl");
		exit(2);
	}

	int i=0;
	int timeout=5000;
	struct sockaddr_in client;
	socklen_t len=sizeof(client);

	while(1)
	{
		switch(read_num=epoll_wait(epoll_fd,ret_ev,ret_num,timeout))
		{
			case 0:
				printf("time out\n");
				break;
			case -1:
				perror("epoll");
				exit(3);
			default:
				{
					for(i=0;i<read_num;++i)
					{
						if(ret_ev[i].data.fd==listen_sock&&(ret_ev[i].events&EPOLLIN))
						{
							int fd=ret_ev[i].data.fd;
							int new_sock=accept(fd,(struct sockaddr*)&client,&len);
							if(new_sock<0)
							{
								perror("accept");
								continue;
							}
							ev.events=EPOLLIN;
							ev.data.fd=new_sock;
							printf("sock   %d\n",new_sock);
							if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,new_sock,&ev)<0)
							{
								perror("epoll_ctl");
								close(listen_sock);
							}
						}
						else
						{
							if(ret_ev[i].events&EPOLLIN)
							{
								accept_request(ret_ev[i].data.fd);
							}
						}
					}
				}
		}
	}
}

int main(int argc,char *argv[])
{
	if(argc != 3)
	{
		usage(argv[0]);
		exit(1);
	}

	char *ip=argv[1];
	int port=atoi(argv[2]);
	int listen_sock=startup(ip,port);

	epoll_http(listen_sock);
	close(listen_sock);
	
//	struct sockaddr_in client;
//	socklen_t len=sizeof(client);
//	
//	while(1)
//	{
//		int new_sock=accept(listen_sock,(struct sockaddr*)&client,&len);
//		if(new_sock<0)
//		{
//			printferr(__FILE__,__func__,__LINE__);
//			continue;
//		}
//
//		pthread_t tid;
//      pthread_create(&tid,NULL,threadrun,(void*)new_sock);
//		pthread_detach(tid);
//	}
	return 0;
}
