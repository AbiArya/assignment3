#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
int main(int argc, char* argv[]){

	//variables
	int sock;//return value of socket function
	struct sockaddr_in server;
	int mysock;
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

	//accept
	do{
		mysock = accept(sock, (struct sockaddr *)0, 0);
		if(mysock == -1)
			perror("accept failed");
		else{
			memset(buff,0,sizeof(buff));
			if((rval = recv(mysock,buff,sizeof(buff),0))<0)
				perror("reading stream message error");
			else if(rval ==0)
				printf("ending connection\n");
			else
				printf("msg: %s\n", buff);
			printf("got the message %d\n",rval);
			close(mysock);
		}	
	}while(1);	

	return 0;
}

