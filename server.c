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


int recvall(int socket, void *buffer, size_t length, int flags){
	ssize_t n;
	char *p = buffer;
	while (length > 0)
	{
		//	printf("IN WHILE\n");
		n = recv(socket, p, length, flags);
		if (n == 0)
			break;
		else if(n<0)
			return -1;
		p += n;
		length -= n;
		if(strstr(buffer,"srisrisri")!=NULL){
//			printf("BREAK\n");
			printf("%s\n", buffer);
			break;
		}
		//printf("IN WHILE\n");
		fflush(stdout);
	}
	return 1;
}


void * sort(void *arg){
	int * mysock = (int*)arg;
	int rval;
	char buff[10000];
	memset(buff,0,sizeof(buff));
	int loop = 0;
	char* row = NULL;
	int column = 0;
	/*	if((rval = recv(*mysock,buff,sizeof(buff),0))<0)
		perror("reading stream message error");
		else{
	//column = atoi(buff);//should be column number
	}*/

	//printf("%i\n", column);
	if((rval = recvall(*mysock,buff,sizeof(buff),0))<0)
		perror("reading stream message error");
	else{
		row = strdup(buff);//should be column number
	}
	//printf("after row\n");


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
	//printf("OUTSIDE LOOP=0\n");

	while(loop==0){
		//printf("in loop\n");
		memset(buff,0,sizeof(buff));
		if((rval = recvall(*mysock,buff,sizeof(buff),0))<0)
			perror("reading stream message error");
//		else if(rval ==1){
//			//			printf("exit");
//}			loop=1;
		else{
			if(strcmp(buff,"borisonufriyev\n")==0)
				break;
			if(id!=0){

				row = buff;

				stringCount++;
				if(row==NULL) break;

				if(stringCount == stringCapacity - 100){

					strings = moreCapacity(strings, stringCapacity);
					stringCapacity = stringCapacity*1.5;
				}

				strings[id] = strdup(row);
				cellWithSpaces = getCellAtInd(row, column);//row is the full row, but the parameter to the function ends up being just "Color"

			}
			if(strcmp(cellWithSpaces, "Error") == 0){ 
				printf("Error has occurred\n");
				//DO ERROR SHIT

			}
			char* cell = trim(cellWithSpaces);

			//inserting the data in the column we're sorting by into a linked list

			Node* ins = (Node*)malloc(sizeof(Node));
			ins->id = id;


			if(type!=2){

				if(type == 0){


					int val=0;
					sscanf(cell, "%d", &val); //parse int		
					ins->number = val;
				}
				else if(type==1){

					float val=0;
					val = (float)atof(cell);
					ins->dec = val;
				}

				head = insertAtHead(ins, head);

			}
			else{


				ins->word = strdup(cell);
				head = insertAtHead(ins, head);

			}


			id++;


		}   
	}
	/* 0 for int, 2 for float, anything else for string */


	if(isFloat){
		head = mergeSort(head, 1);
	}
	else if(isNumeric){
		head = mergeSort(head, 0);
	}
	else{
		head = mergeSort(head, 2);
	}


	Node* ptr = head;
	Node* prev = NULL;


	pthread_mutex_lock(&lock);

	int ctr=0;
	while(ptr!=NULL){
		ctr++;
		insertArr(strings[ptr->id]);
		//	free(strings[ptr->id]);
		prev = ptr;
		ptr=ptr->next;
		if(!isNumeric){
			//		free(prev->word);
		}
		//	free(prev);
	} 
	pthread_mutex_unlock(&lock);


	//	free(strings);

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
		}	
	}while(1);	


	return 0;
}

