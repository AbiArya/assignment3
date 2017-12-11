#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <dirent.h>
#include <netinet/in.h>
#include "client.h"
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

//I had to copy and paste the server_thread.h file for some reason when i included it, it didnt pick it up

int threadCount = 1;
char* outputfile = NULL;
pthread_mutex_t lock;
char** globArr = NULL;
int globArrEnd = 0;
int numElems = 5000;
int initialTID = -1;
int finalSort = 0;
int type = -1; //0=int, 1=float, 2=string
char* title = NULL;
int colNum = -2;


struct temp{
    	int id;
	int number;
	char* word;
	float dec;
	struct temp* next;

};

typedef struct tNode{
        pthread_t* curr;
        struct tNode *next;
}threadNode;


typedef struct temp Node;

struct trav_param{
	char* rootDirName;
	char* colName;
};

struct sort_param{
	FILE* fp;
	char* colName;

};


//prototypes
Node* insertAtHead(Node*, Node*);
char* getRow(FILE*);
int getColNum(char*, char*);
char* getCellAtInd(char*, int);
char* trim(char* str);
char** moreCapacity(char**, int);
Node* splitList(Node*);
Node* mergeInts(Node*, Node*);
Node* mergeFloats(Node*, Node*);
Node* floatMergeSort(Node*);
Node* intMergeSort(Node*);
Node* mergeStrings(Node*, Node*);
Node* stringMergeSort(Node*);
Node* mergeSort(Node*, int);
void* traverse(void*);
void* sortFile(void*);
void insertArr(char*);



char** moreCapacity(char** lines, int size){
    int newSize = size * 1.5;
    char** temp = NULL;

    temp = (char**)realloc(lines, newSize * sizeof(char*));
    if(temp){
        return temp;    
    }

    return NULL;
}


pthread_t* moreThreads(pthread_t* lines, int size){
    int newSize = size * 1.5;
    pthread_t* temp = NULL;

    temp = (pthread_t*)realloc(lines, newSize * sizeof(pthread_t));
    if(temp){
        return temp;    
    }

    return lines;
}



char* trim(char* str){
    char* ans;

    // trim leading spaces
    while(isspace((unsigned char)* str)){
        str++;
    }
    
    // check if str is all spaces
    if(*str==0){
        return str;
    }

    // trim leading spaces
    ans = str + strlen(str) -1;
    while(ans > str && isspace((unsigned char)* ans)){
        ans--;
    }

    // null terminate the new string
    *(ans+1) = 0;

    return str;
}
Node* insertAtHead(Node* new1, Node* head){
	
	if(head==NULL){
		new1->next = NULL;
		return new1;
	}
	new1->next = head;
	return new1;


}


char* isDir(char* rootDirName, char* filename){
	

	char* path = malloc(strlen(rootDirName) + 1 + strlen(filename) + 20);
	strcpy(path, rootDirName);
	strcat(path, "/");
	strcat(path, filename);
	
	int status = 0;
	struct stat buffer;

	status = stat(path, &buffer);


	if(stat == 0){
		free(path);
		return NULL;

	}

	if(S_ISREG(buffer.st_mode)){
		free(path);
		return NULL;
	}
	if(S_ISDIR(buffer.st_mode)){ //return path if directory, else return NULL
		return path;
	}
	free(path);
	return NULL;
	

}



char* getRow(FILE* fp){ // add file pointer as param, the alloced row must be freed somewhere
    
    char* line = calloc(512, sizeof(char));
    line[0] = 0;
    char* temp = NULL;
    int index = 0;
    char ch = NULL;
    int size = 512;
    
    while(ch != '\n' && ch != EOF){
        ch = getc(fp);  
        if(index >= size){
            size = size * 1.5;
	   
            temp = (char*)realloc(line, size);
	    if(temp){
            	line = temp;
	    }
       
        }
	

	//detect end of file
	if(feof(fp)){ 
		return NULL;
	}
	
        
        line[index] = ch;
        index++;
    }
    
    line[index+1] = '\0';
    return line;
}

int getColNum(char* row1, char* colName){
    int colCount = 0;
    char* row = row1;
    const char* delim = ",\r\n";
    char* token = strsep(&row,delim);
    while(token != NULL){
        if(strcmp(token, colName)==0){
	    if(title==NULL) title = strdup(row1);
            return colCount;
        }
        
        token = strsep(&row, delim);
        colCount++;
    }
    return -1;
}

char* getCellAtInd(char* row1, int colNum){ 

    const char* delim = ",\r\n";
    char* row = row1;
    char* token = strsep(&row, delim);
    int index = 0;
    const char* quoteDelim = "\"";
    

    while(token != NULL){

        if(token[0] == '\"'){ 	 

            token++;          
            char* blah = strsep(&row, quoteDelim);
            char* conc = (char*)calloc(strlen(token) + strlen(blah)+25,1);

            if(index == colNum){

                strcat(conc,token);
                strcat(conc, ",");
                strcat(conc,blah);
                return conc;
            }
            free(conc);

            token = strsep(&row, delim);
        }
        else{

            if(index == colNum){
                return token;        
            }

            token = strsep(&row, delim);

            index++;
        }
    }

    return "Error";

}








int portno;
char* hostname;


void insertArr(char* str){

	if(globArrEnd + 10 > numElems){

		globArr = moreCapacity(globArr, numElems);
		numElems *= 1.5;
	}


	globArr[globArrEnd] = strdup(str);
	globArrEnd++;




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
		remainder = malloc(sizeof(char) * (strlen(rval) + strlen(buffer)));
		strcat(remainder, rval);
		strcat(remainder,buffer);
	}else{
		remainder=malloc(strlen(buffer));
		strcpy(remainder,buffer);

	}
	
	while((stuff=strstr(remainder,"srisrisri"))!=NULL){
		print = malloc(sizeof(char)*(stuff-remainder) +1);
		strncpy(print,remainder,(stuff-remainder));
		print[strlen(print)]='\0';

		
		insertArr(strdup(print));
		//free(print); //causes weird printing error
		remainder = remainder + (stuff-remainder)+9;
	}


	return remainder;
}


int main(int argc, char* argv[]){
    globArr = (char**)malloc(sizeof(char*) * 5000);

    char* dirName = NULL;
    char* colName = NULL;
	char* outputDir = "./\0"; // This is the directory where the file should be outputted
	int toOut = 0;

	int i = 0;
	
	

    // check that the args make sense
    if(argc < 7){
        printf("Not enough parameters. \n");
        return 0;
    }
	while(i<argc){
		if(strcmp(argv[i],"-o")==0){
			outputDir=argv[i+1];
			toOut=1;
		}
		else if(strcmp(argv[i],"-d")==0){
			dirName=argv[i+1];
		}
		i=i+1;
	}    

	DIR * tmpb = opendir(outputDir);
	if(!tmpb){
		printf("Sorry, that output directory does not exist\n");
		return 0;
	}
	closedir(tmpb);

    if(argc >= 7){
        colName = argv[2];
        hostname = argv[4];
        portno = atoi(argv[6]);
    }

    if(pthread_mutex_init(&lock, NULL)==-1) printf("mutex failed\n"); 
    

    if(dirName == NULL){
        dirName = (char*)malloc(256);
        dirName = getcwd(dirName, 256);

    }

    struct trav_param* param = (struct trav_param*)malloc(sizeof(struct trav_param));
    param->rootDirName = dirName;
    param->colName = colName;
    printf("\n\ncolname: %s\n\n", colName);
    traverse(param);
    free(param);
	struct sockaddr_in server;
	struct hostent *hp; 

	int* sock;
	sock = socket(AF_INET, SOCK_STREAM,0);

	if(socket<0){
		perror("Socket Creation failed\n");
		exit(1);
	}
	server.sin_family = AF_INET; 
	hp = gethostbyname(hostname); 
	if(hp == 0){
		perror("Getting hostname failed");
		exit(1);
	}

	memcpy(&server.sin_addr, hp->h_addr, hp->h_length); //copies internet address from hp to server socket
	server.sin_port = htons(9999);
    
    // connect to socket
	if(connect(sock, (struct sockaddr *)&server, sizeof(server))<0){
		perror("Connection failed");
		exit(1);
	}
	char * finalName = malloc(sizeof(outputDir) + sizeof(colName) + 24);
	if(toOut==1){
		if(outputDir[strlen(outputDir)-1]!='/')
			strcat(outputDir,"/"); 	
		strcpy(finalName, outputDir);
	}
	strcat(finalName, "AllFiles-sorted-");
	strcat(finalName, colName);
	strcat(finalName,".csv");

	FILE* h = fopen(finalName, "w+");

	char * rval = "";
	char buff[10001];
	char leftover[100001];
	memset(leftover,0,sizeof(leftover));


while(1){


		memset(buff,0,sizeof(buff));
		if(strcmp((rval = recvall(*sock,buff,10000,0,leftover)),"ERROR")==0)
			perror("reading stream message error");

		else{
			if(strstr(buff,"borisonufriyev")!=NULL){
				break;
			}
				
				strcpy(leftover,rval);
				//row = buff;
				//if(row==NULL) break;
				fprintf(h, "%s\n", rval);

		}   
	}






	fclose(h);

	close(sock);


	

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
    char colLine[100];

    pthread_mutex_lock(&lock);
    char num = colNum + '0'; 
    pthread_mutex_unlock(&lock);

    // send col num
    colLine[0] = num;
    strcat(colLine, "srisrisri");
    sendall(sock,strdup(colLine),strlen(colLine),0);

    // loop through each row and send to socket 
    while(row!=NULL){
        

        char line[10000];
	memset(line,0,sizeof(line));
        strcat(line, row);
        strcat(line,"srisrisri");
        sendall(sock,strdup(line),strlen(line),0);        
        row = getRow(fp);

    }
    // send eof signal
    sleep(5);
    sendall(sock,"borisonufriyev",strlen("borisonufriyev"),0);

    fclose(fp);
    printf("sent message");
    close(sock);


    pthread_exit(NULL);
    return 0;
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
