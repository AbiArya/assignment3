#ifndef _first_h
#define _first_h

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

//int threadCount = 1;
//int initialTID = -1;
pthread_mutex_t lock;
//int colNum = -2;
//char* title = NULL;


/*struct trav_param{
	char* rootDirName;
	char* colName;
};*/

/*struct sort_param{
	FILE* fp;
	char* colName;

};*/

//prototypes
char* getRow(FILE*);
int getColNum(char*, char*);
void* sendFile(void*);
void* traverse(void*);
int sendall(int, const void*, size_t, int);


// methods

/*char* isDir(char* rootDirName, char* filename){
	

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
	

}*/

/*pthread_t* moreThreads(pthread_t* lines, int size){
    int newSize = size * 1.5;
    pthread_t* temp = NULL;

    temp = (pthread_t*)realloc(lines, newSize * sizeof(pthread_t));
    if(temp){
        return temp;    
    }

    return lines;
}*/

/*char* getRow(FILE* fp){ // add file pointer as param, the alloced row must be freed somewhere

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
}*/




#endif
