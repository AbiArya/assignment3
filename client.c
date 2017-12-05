#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <dirent.h>

#include "client.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>




int portno;
char* hostname;

int main(int argc, char* argv[]){
    
    char* dirName = NULL;
    char* colName = NULL;

    // check that the args make sense
    if(argc < 7){
        printf("Not enough parameters. \n");
        return 0;
    }
    
    if(argc == 7){
        colName = argv[3];
        hostname = argv[5];
        portno = atoi(argv[7]);
    }

    if(pthread_mutex_init(&lock, NULL)==-1) printf("mutex failed\n"); 
    

    if(dirName == NULL){
        dirName = (char*)malloc(256);
        dirName = getcwd(dirName, 256);

    }

    struct trav_param* param = (struct trav_param*)malloc(sizeof(struct trav_param));
    param->rootDirName = dirName;
    param->colName = colName;
    traverse(param);
    free(param);

	return 0;
}

void* traverse(void* param){


	pthread_t* threads = malloc(sizeof(pthread_t)*10);
    int tct=0;
	int tsize = 10;

	struct trav_param* trav = (struct trav_param*) param;
	char* rootDirName = trav->rootDirName; 
	char* colName = trav->colName;


	DIR* root = opendir(rootDirName);
	if(root == NULL){
		printf("\n\ninvalid directory\n\n");
		pthread_exit(NULL);
	}
	struct dirent* dp = NULL;

	while((dp = readdir(root)) != NULL) {
		
		if((dp->d_name)[0] == '.') continue;
		char* isdir = isDir(rootDirName, dp->d_name);

		if(isdir){

			
			pthread_t thread;
			pthread_mutex_lock(&lock);
			threadCount++;
			pthread_mutex_unlock(&lock);
			struct trav_param* new = (struct trav_param*)malloc(sizeof(struct trav_param));
			new->rootDirName = isdir;
			new->colName = colName;

			pthread_create( &thread, NULL, traverse, new);
			if(tsize == tct){
				
				threads = moreThreads(threads, tsize);
				tsize = tsize*1.5;
			}
			threads[tct] = thread;
			tct++;
			
			
		}

        	FILE* fp = fopen(dp->d_name, "r" );
		
            if(!fp){ //get absolute path
                char* newfile = malloc(strlen(rootDirName) + 1 + strlen(dp->d_name) + 10);
                strcpy(newfile, rootDirName);
                strcat(newfile, "/");
                strcat(newfile, dp->d_name);
                fp = fopen(newfile, "r");
                free(newfile);
            }



		if(strstr(dp->d_name, ".csv")){  //isCSV
   
            if(!strstr(dp->d_name, "-sorted")){

                pthread_t thread;
                pthread_mutex_lock(&lock);
                threadCount++;
                pthread_mutex_unlock(&lock);
                struct sort_param* new = (struct sort_param*)malloc(sizeof(struct sort_param));
                new->fp = fp;
                new->colName = colName;


                pthread_create(&thread, NULL, sendFile, new);
                if(tsize == tct){

                    threads = moreThreads(threads, tsize);
                    tsize = tsize*1.5;
                }
                threads[tct] = thread;
                tct++;


            }
		}
				
	}//end while

	int tm = 0;
	while(tm < tct){ 

		pthread_join(threads[tm], NULL);
		tm++;

	}

	if(syscall(__NR_gettid) != initialTID){
		free(trav);
		free(threads);
		pthread_exit(NULL);
	}
	free(threads);
	return NULL;
}



void* sendFile(void* param){
    
    struct sort_param* sort = (struct sort_param*) param;
    char* colName = sort->colName;
    //char* fileName = sort->fileName;
    FILE* fp = sort->fp;
    char* titleRow = NULL;



    // get the first line of the csv
    rewind(fp);

    titleRow = getRow(fp); 
    // check if requested column is in the csv

    char* checkTitle = strstr(titleRow, colName);
    if(checkTitle == NULL){
        printf("\nRequested column name is not in the csv.\n");
        fclose(fp);        
        pthread_exit(NULL);

    }


    pthread_mutex_lock(&lock);
    if(colNum == -2){
        colNum = getColNum(titleRow, colName);
    }
    if(colNum == -1){
        printf("Error has occured.\n");
        fclose(fp);        
        pthread_exit(NULL);
    }
    pthread_mutex_unlock(&lock);
    
    // create a socket
	int sock; //the output of method socket()
	struct sockaddr_in server;// socket struct
	char buff[1024];//used for the input
	struct hostent *hp; //used for getting hostname


    sock = socket(AF_INET, SOCK_STREAM,0); //create socket

	if(sock<0){
		perror("socket failed");
		exit(1);
	}
	server.sin_family = AF_INET; //the address family, AF_INET seems to be standard
	hp = gethostbyname(hostname); //get the hostname from input, and give it to hp using gethostbyname
	if(hp == 0){
		perror("get hostbyname failed");
		exit(1);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length); //copies internet address from hp to server socket
	server.sin_port = htons(9999);
    
    // connect to socket
	if(connect(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("connect failed");
		exit(1);
	}

    char* row = getRow(fp);
    // loop through each row and send to socket 
    while(row!=NULL){
        row = getRow(fp);
        // send column number 

        // sending each line to socket goes here
        /*
        printf("enter the message: ");
        fgets(buff, 1023, stdin);

        if(send(sock, buff,(sizeof(buff)), 0)<0){
            perror("send failed");
            exit(1);	
        }
        */


        // send end signal

    }
    fclose(fp);

    printf("sent message");
    close(sock);


    pthread_exit(NULL);
    return 0;
}



