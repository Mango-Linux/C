/***************************************************************************** 
Copyright: 2014-2021,Radiant Solar Technology Co., Ltd.
File name: server.c 
Author: Lei.Zhang
Description: This is a server with TCP protocol.
Version: v0.0
Date: 2021.06.04 
*****************************************************************************/ 
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include <sys/select.h>

#define	BUF_LEN 		256
#define BACKLOG 		10 
#define SERVER_PORT 	3456 

int main(int argc ,char ** argv)
{
	int c_fd = -1;
	int s_fd = -1;
	int ret = 0;
	int n_read = 0;
	int len = 0;
	char sendbuf[BUF_LEN] = {0};
	char readbuf[BUF_LEN] = {0};
	struct sockaddr_in s_addr;
	struct sockaddr_in c_addr;
	int i = 0;
	int maxfd = 0;
	fd_set set, rset;

	if(argc != 3)
	{
		printf("\tUsage: tcpServer [ip] [port]====>\n");
		return -1;
	} 

	s_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	if(s_fd == -1)
	{
		printf("\tCreate socket fail====>\n");  
		return -1;
	}

	memset(&s_addr, 0, sizeof(struct sockaddr_in));
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &s_addr.sin_addr);
	
#if 0
	inet_aton(argv[1], &s_addr.sin_addr);
	s_addr.sin_addr.s_addr = INADDR_ANY;
#endif

	if(bind(s_fd, (struct sockaddr*)&s_addr, sizeof(struct sockaddr_in)) == -1)
	{
		printf("\tbind fail====>\n");
		close(s_fd);
		return -1;
	}

	if(listen(s_fd, BACKLOG) == -1)
	{
		printf("\tlisten fail====>\n");  
		close(s_fd);
		return -1;
	}
	
	printf("\tbind and listen ok! wait accpet====>\n");

	FD_ZERO(&set);
	FD_SET(s_fd, &set);
	maxfd = s_fd;

	len = sizeof(struct sockaddr_in);
	memset(&c_addr, 0, sizeof(struct sockaddr_in));
	while(1)
	{
		rset = set;
		ret = select(maxfd+1, &rset, NULL, NULL, NULL);
		if(ret < 0)
		{
			printf("\tSelect fail====>\n");
			break;
		}

		for(i=0; i<=maxfd+1; i++)
		{
			if(FD_ISSET(i, &rset))
			{
				if(FD_ISSET(s_fd, &rset)) /*listen socket is ok */
				{
					c_fd = accept(s_fd, (struct sockaddr*)&c_addr, &len);
					if(c_fd > 0)
					{
						printf("\taccept ok,c_fd=%d,ip=%s,port=%d====>\n", c_fd, inet_ntoa(s_addr.sin_addr), ntohs(s_addr.sin_port));
						FD_SET(c_fd, &set);
						maxfd = (maxfd > c_fd ? maxfd : c_fd);
					}
				}
				else
				{
					memset(readbuf, 0,sizeof(readbuf));	
					ret = read(i,readbuf,sizeof(readbuf));
					if(ret == 0)
					{
						FD_CLR(i, &set); 
						close(i);
						printf("\tclose client socket=%d====>\n", i);
					}
					else
					{
						printf("\tread from client [%d]:%s====>\n",i,readbuf); 
						memset(sendbuf, 0, sizeof(sendbuf));
						strncpy(sendbuf, "Receive Done", sizeof(sendbuf));
						write(i, sendbuf, sizeof(sendbuf));
					}
				}
			}
		}	
	}

	close(s_fd);
	return 0;
}

