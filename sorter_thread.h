#ifndef _first_h
#define _first_h

 
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



#endif
