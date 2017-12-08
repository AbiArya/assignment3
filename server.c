#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "sorter_thread.h"
#include "mergesort.c"
#include <sys/syscall.h>
#include <sys/types.h>
#include <pthread.h>
//#include "sorter_thread.h"
#include "client.h"
void insertArr(char* str){

	if(globArrEnd + 10 > numElems){

		globArr = moreCapacity(globArr, numElems);
		numElems *= 1.5;
	}


	globArr[globArrEnd] = strdup(str);
	globArrEnd++;




}


char* recvall(int socket, void *buffer, size_t length, int flags,char *rval){
	ssize_t n;
	char *p = buffer;
	char * remainder;
	while (strstr(buffer,"srisrisri")==NULL)
	{
		n = recv(socket, p, length, flags);
		if (n == 0)
			break;
		else if(n<0)
			return "ERROR";
		p += n;
		length -= n;
	}

	char *stuff = buffer;
	char * print;
	if(strlen(rval)!=0){
		remainder = malloc(sizeof(char) * (strlen(rval) + strlen(buffer)));
		strcat(remainder, rval);
		strcat(remainder,buffer);
	}else{
		remainder=buffer;

	}
	
	while((stuff=strstr(remainder,"srisrisri"))!=NULL){
		print = malloc(sizeof(char)*(stuff-remainder));
		strncpy(print,remainder,(stuff-remainder));

		while(((!isdigit(print[strlen(print)-1])) && (print[strlen(print)-1]!=','))){
                        	char * tmpstring = malloc(sizeof(char) * strlen(print));
                        	strncpy(tmpstring,print,strlen(print)-1);
                        	print = tmpstring;
                
        	}


		printf("%s\n",print);
		printf("DONEZOOO\n");
		insertArr(strdup(print));
		remainder = remainder + (stuff-remainder)+9;
	}
	/*while(strlen(remainder)>0){
		if(!isdigit(remainder[strlen(remainder)-1]) && (remainder[strlen(remainder)-1]!=',')){
			char * tmpstring = malloc((sizeof(char) * strlen(remainder)) -1);
			strncpy(tmpstring,remainder,strlen(remainder)-1);
			remainder = tmpstring;
		}else{
			break;
		}
	}*/

	return remainder;
}


void * sort(void *arg){
	int * mysock = (int*)arg;
	char * rval="";
	char buff[10000];
	memset(buff,0,sizeof(buff));
	int loop = 0;
	char* row = NULL;
	int column = 1;
	int Number;
	char *colNum = malloc(sizeof(char)*2);
	char * leftover = malloc(sizeof(char)*10000);
	leftover="";
	/*	if((rval = recv(*mysock,buff,sizeof(buff),0))<0)
		perror("reading stream message error");
		else{
	//column = atoi(buff);//should be column number
	}*/

	//printf("%i\n", column);
	if(((Number = recv(*mysock,colNum,sizeof(colNum),0)))<0)
		perror("reading stream message error");
	else{
		row = strdup(buff);//should be column number
	}
	//printf("after row\n");
	printf("%i\n", atoi(colNum));

	Node* head = (Node*)malloc(sizeof(Node));
	int isNumeric = 1;
	int id = 0;
	head = NULL;

	int stringCount = 1;
	int stringCapacity = 10000; // current number of strings pointed to by char** string
	char** strings = (char**)malloc(stringCapacity*sizeof(char*));


	strings[id] = strdup(row);
	char* cellWithSpaces = getCellAtInd(row,column);
	id++;

	//discern data type of sorting column
	int i=0;

	int isFloat=0;

	pthread_mutex_lock(&lock);
	if(type == -1){

		while(i < strlen(cellWithSpaces)){

			if(!isdigit(cellWithSpaces[i])){
				if(cellWithSpaces[i] == '.'){
					isFloat = 1;
					i++;
					continue;
				}
				isNumeric = 0;
				break;
			} 
			i++;

		}

		if(isFloat) type = 1;
		else if(isNumeric) type = 0;
		else type = 2;

	}
	pthread_mutex_unlock(&lock);
	memset(leftover,0,strlen(leftover));

	while(loop==0){


		memset(buff,0,sizeof(buff));
		if(strcmp((rval = recvall(*mysock,buff,sizeof(buff),0,leftover)),"ERROR")==0)
			perror("reading stream message error");

		else{
			if(strstr(buff,"borisonufriyev")!=NULL)
				break;
			if(id!=0){
				//memset(leftover, 0, sizeof(leftover));
				leftover=strdup(rval);
				row = buff;
				if(row==NULL) break;

				if(stringCount == stringCapacity - 100){

					strings = moreCapacity(strings, stringCapacity);
					stringCapacity = stringCapacity*1.5;
				}

				//	insertArr(strdup(row));
			}



		}   
	}

	pthread_mutex_lock(&lock);
	printf(rval);

	close(*mysock);
	pthread_exit(0);
}


void dump(int * sock){

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
	globArr = (char**)malloc(sizeof(char*) * 5000);
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
			memset(buff,0,sizeof(buff));
			/*	if((rval = recv(*mysock,buff,sizeof(buff),0))<0)
				perror("reading stream message error");
				else if(strcmp(buff,"dumpthatshit\n")){
			//call dump
			continue;

			}*/
			pthread_t newthread;
			if(ptr->curr == NULL){
				//	pthread_t newthread;
				ptr->curr = &newthread;
				ptr->next = malloc(sizeof(threadNode));
				ptr = ptr->next;
			}
			pthread_create(&newthread, NULL, sort, (void*)mysock);			
			//		sort((void*) mysock);
		}	
	}while(1);	


	return 0;
}

