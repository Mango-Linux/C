/***************************************************************************** 
Copyright: 1987-2021,Hacker League Technology Co., Ltd.
File name: client.c 
Author: Mango.Linux
Description: This is a client with TCP protocol.
Version: v0.0
Date: 2021.06.04 
*****************************************************************************/ 
#include <stdio.h>
#include <sys/types.h>        
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define	BUF_LEN 		256
#define BACKLOG 		10 
#define SERVER_PORT 	3456 

int main(int argc , char**argv)
{
	int n_read = 0;	
	int c_fd = -1;
	char readbuf[BUF_LEN] = {0}; 	
	char sendbuf[BUF_LEN] = {0};
	struct sockaddr_in c_addr;

	if(argc != 3)
	{
		printf("\tUsage: tcpClient [ip] [port]====>\n");
		return -1;
	}	   
	
	c_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(c_fd == -1)
	{
		printf("\tCreate socket fail====>\n");
		return -1;
	}	

	printf("\tCreate client socket [%d]====>\n", c_fd);

	memset(&c_addr, 0, sizeof(struct sockaddr_in));
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(atoi(argv[2]));
	inet_pton(AF_INET, argv[1], &c_addr.sin_addr);

	if(connect(c_fd, (struct sockaddr*)&c_addr, sizeof(struct sockaddr_in)) == -1)
	{
		printf("\tConnect error====>\n");  
		close(c_fd);
		return -1;
	}

#if 0		
	 while(1)
	 { 
		memset(sendbuf, 0, sizeof(sendbuf));	  
		printf("\tPlease input message:");
		gets(sendbuf);
		fflush(stdout);
		write(c_fd, sendbuf, sizeof(sendbuf));		   
	}		
#else
	while(1)
	{
		if(fork() == 0)
		{               
		     while(1)
			 { 
				memset(sendbuf, 0, sizeof(sendbuf));      
				printf("\tPlease input message:");
				gets(sendbuf);
				fflush(stdout);
				write(c_fd, sendbuf, sizeof(sendbuf));         
		    }	    
		}

		while(1)
		{
			memset(readbuf, 0, sizeof(readbuf));
			n_read =read(c_fd, readbuf, sizeof(readbuf));          
			if(n_read == 0)
			{
				printf("\tserver out====>\n");     
			}
			else if(n_read == -1)
			{
				perror("\tread error====>\n");
				close(c_fd);
			}	   
			else if(n_read > 0)
			{
				printf("read from server:%s====>\n",readbuf);
				fflush(stdout);    
			}	   
		}
	}	   
#endif
   close(c_fd);
	
   return 0;
}	
