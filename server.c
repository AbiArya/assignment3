#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include "sorter_thread.h"

void * sort(void *arg){
	int * mysock = (int*)arg;
	int rval;
	char buff[1024];
        memset(buff,0,sizeof(buff));
	int i=0;
	while(i==0){
//		printf("in loop\n");
		memset(buff,0,sizeof(buff));
	        if((rval = recv(*mysock,buff,sizeof(buff),0))<0)
	 	       perror("reading stream message error");
	        else if(rval ==0){
//			printf("exit");
			i=1;
		}else
        	        printf("msg: %s\n", buff);
		if(strcmp(buff,"director_name\n")==0)
			printf("yes");
		else
			printf("no");
        }
        close(*mysock);
	pthread_exit(0);
}

int main(int argc, char* argv[]){
	threadNode *global = malloc(sizeof(threadNode));
	threadNode *ptr = global;
	//variables
	int sock;//return value of socket function
	struct sockaddr_in server, cli_addr;
	//int mysock;
	char buff[1024];
	int rval;
	// create socket
	sock = socket(AF_INET, SOCK_STREAM,0);
	if(sock < 0){
		perror("Failed to create a socket");
		exit(1);
	}
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(atoi(argv[2])); //we want to take this from input

	//call bind
	
	if(bind(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("Bind failed");
		exit(1);
	}
	

	//listen
	listen(sock, 5);
	int clilen = sizeof(cli_addr);
	//accept
	do{
		int *mysock = malloc(sizeof(int));
		*mysock = accept(sock, (struct sockaddr *)&cli_addr, &clilen);
		
		if(*mysock == -1)
			perror("accept failed");
		else{
			struct sockaddr_in *s = &cli_addr;
			char ipstr[INET6_ADDRSTRLEN];
			inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
		//	printf("%s\n", ipstr);//ip address of the connection, store or print this, whatever
			pthread_t newthread;
			if(ptr->curr == NULL){
			//	pthread_t newthread;
				ptr->curr = &newthread;
				ptr->next = malloc(sizeof(threadNode));
				ptr = ptr->next;
			}
			pthread_create(&newthread, NULL, sort, (void*)mysock);			
		}	
	}while(1);	
	
	
	return 0;
}

