#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#define DATA "hello word of socket"
//#include "sorter_thread.h"
#include <sys/stat.h>
#include <dirent.h>
#include <syscall.h>

int sendall(int socket, const void *buffer, size_t length, int flags){
    ssize_t n;
    const char *p = buffer;
    while (length > 0)
    {
        n = send(socket, p, length, flags);
        if (n <= 0)
            return -1;
        p += n;
        length -= n;
    }
	send(socket,"srisrisri", strlen("srisrisri"),0);
	char tmp [15];
	recv(socket,tmp,15,0);
    return 0;
}


int main(int argc, char* argv[]){
	int sock; //the output of method socket()
	struct sockaddr_in server;// socket struct
	char buff[1024];//used for the input
	struct hostent *hp; //used for getting hostname
	//test

	//	if(argc<7){
	//		printf("error, not even inputs");
	//		exit(1);
	//	}

	sock = socket(AF_INET, SOCK_STREAM,0); //create socket

	if(sock<0){
		perror("socket failed");
		exit(1);
	}
	int portno = atoi(argv[6]); //turns input port number to an int
	server.sin_family = AF_INET; //the address family, AF_INET seems to be standard
	printf(argv[4]);
	hp = gethostbyname(argv[4]); //get the hostname from input, and give it to hp using gethostbyname
	if(hp == 0){
		perror("get hostbyname failed");
		exit(1);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length); //copies internet address from hp to server socket
	server.sin_port = htons(portno);

	if(connect(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("connect failed");
		exit(1);
	}

	/*	while(1){
		printf("enter the message: ");
		fgets(buff, 1023, stdin);

		if(send(sock, buff,(sizeof(buff)), 0)<0){
		perror("send failed");
		exit(1);	
		}
		printf("sent message");
		}*/
	char * fileName = "movie_metadata.csv";
	FILE* fp = fopen(fileName, "r+");
	char line[10000];
	int stuff=0;
	while(fgets(line,sizeof line, fp)!=NULL){
	//	strcat(line,"srisrisri");
		sendall(sock,strdup(line),strlen(line),0);
		//printf("%s\n",line);
		stuff++;
		memset(line,0,strlen(line));	
	}
	sleep(10);
	sendall(sock,"borisonufriyev",strlen("borisonufriyev"),0);
	printf("%i\n", stuff);




//close(sock);

return 0;
}
