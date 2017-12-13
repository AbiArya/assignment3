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
#include "client.h"

//									MAKE SURE TO JOIN ALL THREADS BEFORE YOU DUMP



/* COLUMN Num IS STILL HARDCODED */



void dump(int*, int);

void insertArr(char* str){

	if(globArrEnd + 10 > numElems){

		globArr = moreCapacity(globArr, numElems);
		numElems *= 1.5;
	}


	globArr[globArrEnd] = strdup(str);
	globArrEnd++;




}


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
       //send(socket,"srisrisri", strlen("srisrisri"),0);
                return 0;
                }



char* recvall(int socket, char *buffer, size_t length, int flags,char *rval){
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
	buffer[strlen(buffer)]='\0';
	char *stuff = buffer;
	char * print;
	if(strlen(rval)!=0){
		remainder = calloc(  strlen(rval) + strlen(buffer) + 5, sizeof(char));
		
		strcat(remainder, rval);
		strcat(remainder,buffer);
	}else{
		remainder=malloc(strlen(buffer)+5);
		strcpy(remainder,buffer);

	}

	while((stuff=strstr(remainder,"srisrisri"))!=NULL){
		print = calloc( (stuff-remainder) +3, sizeof(char));
		strncpy(print,remainder,(stuff-remainder));
		print[strlen(print)]='\0';

		pthread_mutex_lock(&lock);
		insertArr(print);
		pthread_mutex_unlock(&lock);
		remainder = remainder + (stuff-remainder)+9;
	}

	return remainder;
}


int recvallcommand(int socket, char *buffer, size_t length, int flags){
	int  n =0;
	char *p = buffer;
	while (n!=1)
	{
		n = recv(socket, p, length, flags);
		if (n == 1)
			break;
		else if(n<0)
			return -1;
		p += n;
		length -= n;
	}
	buffer[strlen(buffer)]='\0';

	if(strcmp(buffer,"d")==0)
		return 1;
	else
		return 0;

}


void * sort(void *arg){
	int * mysock = (int*)arg;
	char * rval="";
	char buff[10001];
	memset(buff,0,sizeof(buff));
	int loop = 0;
	char* row = NULL;
	char leftover[10001];
	
	int id = 1;

	memset(leftover,0,sizeof(leftover));

	while(loop==0){


		memset(buff,0,sizeof(buff));
		if(strcmp((rval = recvall(*mysock,buff,10000,0,leftover)),"ERROR")==0)
			perror("reading stream message error");

		else{
			if(strcmp(buff,"borisonufriyev")==0)
				break;
			if(id!=0){
				//memset(leftover, 0, sizeof(leftover));
				strcpy(leftover,rval);
				row = buff;
				if(row==NULL) break;

			}



		}   
	}

	close(*mysock);
	pthread_exit(0);
}


/*



 ***need to generate output file name to write to****



*/



void dump(int * sock, int colNum){

	//printf("%s", globArr[0]);
	int type = -1;
	int isFloat = 0;
	int isNumeric = 1;
	int ctr;
	Node* head = NULL;

	pthread_mutex_lock(&lock);
	for(ctr=0;ctr<globArrEnd;ctr++){//take each string in globArr, make a node, sort the linked list, loop through the list and fprintf


		char* cellWithSpaces = getCellAtInd(strdup(globArr[ctr]), colNum);


		if(ctr == 0){

			int i = 0;
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

		if(strcmp(cellWithSpaces, "Error") == 0){
			printf("Error has occurred\n");
			pthread_exit(NULL);
		}
		char* cell = trim(cellWithSpaces);



		//inserting the data in the column we're sorting by into a linked list

		Node* ins = (Node*)malloc(sizeof(Node));
		ins->id = ctr;


		if(type==0){

			int val;
			sscanf(cell, "%d", &val); //parse int		
			ins->number = val;
		}
		else if(type==1){
			float val;
			val = (float)atof(cell);
			ins->dec = val;
		}
		else if(type==2){

			ins->word = strdup(cell);

		}
		else{
			printf("type not right\n");
			return;

		}

		head = insertAtHead(ins, head);
	}


	if(type==1){
		head = mergeSort(head, 1);
	}
	else if(type==0){
		head = mergeSort(head, 0);
	}
	else if(type==2){
		head = mergeSort(head, 2);
	}
	else{
		printf("\ninvalid sort type\n");

	}

	Node* ptr = head;
	Node* prev = NULL;

	
	while(ptr!=NULL){
		sendall(*sock,globArr[ptr->id],strlen(globArr[ptr->id]),0);
		free(globArr[ptr->id]);
		prev = ptr;
		ptr=ptr->next;
		free(prev);
	}
	globArrEnd = 0;
	sleep(10);
	sendall(*sock,"borisonufriyev", strlen("borisonufriyev"),0);
	pthread_mutex_unlock(&lock);
	close(*sock);
}


int main(int argc, char* argv[]){



	threadNode* head = NULL;
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
	printf("Received connections from: ");
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
				printf("%s,", ipstr);//ip address of the connection, store or print this, whatever
			memset(buff,0,sizeof(buff));
			if((rval = recvallcommand(*mysock,buff,1,0))<0)
				perror("reading stream message error");
			else if(rval==1){
				//MAKE SURE TO JOIN
				//call dump
				char number [4];
				recv(*mysock, number, 2, 0);
				number[strlen(number)]='\0';
				fflush(stdout);				
				threadNode* tmp = head;
				while(tmp!=NULL){

					pthread_join(*(tmp->curr), NULL);
					tmp = tmp->next;


				}
				dump(mysock,atoi(number)); //CHANGE THE 1 TO THE COLNUM



			}else{


				pthread_t newthread;


				if(head == NULL){

					head = malloc(sizeof(threadNode));
					head->curr = &newthread;
					head->next = NULL;
				}
				else{
					threadNode* ins = malloc(sizeof(threadNode));
					ins->curr = &newthread;
					ins->next = head;
					head = ins;


				}

				pthread_create(&newthread, NULL, sort, (void*)mysock);
			}
			
		}	
	}while(1);	


	return 0;
}

