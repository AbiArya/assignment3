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
		}else{



			colNum = getColNum(titleRow, colName);
			if(colNum == -1){
				printf("Error has occured.\n");
				fclose(fp);        
				pthread_exit(NULL);
			}
			pthread_mutex_unlock(&lock);
			// find what column # the title is in

			Node* head = (Node*)malloc(sizeof(Node));
			int isNumeric = 1;
			int id = 0;
			head = NULL;

			int stringCount = 1;
			int stringCapacity = 10000; // current number of strings pointed to by char** string
			char** strings = (char**)malloc(stringCapacity*sizeof(char*));


			strings[id] = strdup(row);
			char* cellWithSpaces = getCellAtInd(row,colNum);


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


			// loop through each row and get the cell to sort
			while(row!=NULL){

				if(id!=0){

					row = getRow(fp); //this is malloc'd...must free somewhere

					stringCount++;
					if(row==NULL) break;

					if(stringCount == stringCapacity - 100){

						strings = moreCapacity(strings, stringCapacity);
						stringCapacity = stringCapacity*1.5;
					}

					strings[id] = strdup(row);
					cellWithSpaces = getCellAtInd(row, colNum);//row is the full row, but the parameter to the function ends up being just "Color"

				}
				if(strcmp(cellWithSpaces, "Error") == 0){ 
					printf("Error has occurred\n");
					fclose(fp);
					pthread_exit(NULL);

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
			fclose(fp);

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
				free(strings[ptr->id]);
				prev = ptr;
				ptr=ptr->next;
				if(!isNumeric){
					free(prev->word);
				}
				free(prev);
			} 
			pthread_mutex_unlock(&lock);


			free(strings);

		}







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

