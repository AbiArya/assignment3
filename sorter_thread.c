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



/*

   TO DO:



OPTIMIZATIONS:
1) Instead of calling getColNum for each file, once we verify that the column exists in the titleRow, we can just use the global colNum variable
-possible problem if the columns are in a different order in each csv file


2) Store the type of data as well, so we don't have to loop to find that each time
-possible problem is if two files have the same titlerow but the column is a diff data type...shouldn't happen




 ***empty columns and 0-value columns are treated as the same by int sort






*/

int main(int argc, char* argv[]){

	// check that the args make sense
	if(argc < 3){
		printf("Not enough parameters. \nMust be in form \"-c <column name>\"\n");
		return 0;
	}

	//boolean flags to check for duplicate flags
	int d = 0;
	int o = 0;
	int c = 0;
	char* dirName = NULL;
	char* outputName = NULL;
	char* colName = NULL;




	if (argc > 2){

		if(argc == 3){

			if(strcmp(argv[1], "-c") != 0){
				printf("-c must be included\n");
				return 0;
			}
			colName = argv[2];
			c=1;

		}

		else if(argc == 5){

			if(strcmp(argv[1], "-d") != 0 && strcmp(argv[1], "-o") != 0 && strcmp(argv[1], "-c") != 0){  //get first command line flag
				printf("Incorrect params.\n");
				return 0;
			}
			if(strcmp(argv[1], "-d") == 0){

				dirName = argv[2];
				d=1;
			}
			else if(strcmp(argv[1], "-c") == 0){

				colName = argv[2];
				c=1;	
			}
			else if(strcmp(argv[1], "-o") == 0){

				outputName = argv[2];
				o=1;
			}



			if(strcmp(argv[3], "-d") != 0 && strcmp(argv[3], "-o") != 0 && strcmp(argv[3], "-c") != 0){
				printf("Incorrect params.\n"); return 0;
				return 0;
			}
			if(strcmp(argv[3], "-d") == 0){
				if(d) { printf("Incorrect params.\n"); return 0;}                
				dirName = argv[4];
				d=1;
			}
			else if(strcmp(argv[3], "-c") == 0){
				if(c) { printf("Incorrect params.\n"); return 0;}
				colName = argv[4];
				c=1;	
			}
			else if(strcmp(argv[3], "-o") == 0){
				if(o) { printf("Incorrect params.\n"); return 0;}
				outputName = argv[4];
				o=1;
			}
		}
		else if(argc == 7){
			if(strcmp(argv[1], "-d") != 0 && strcmp(argv[1], "-o") != 0 && strcmp(argv[1], "-c") != 0){  //get first command line flag
				printf("Incorrect params.\n");
				return 0;
			}
			if(strcmp(argv[1], "-d") == 0){

				dirName = argv[2];
				d=1;
			}
			else if(strcmp(argv[1], "-c") == 0){

				colName = argv[2];
				c=1;	
			}
			else if(strcmp(argv[1], "-o") == 0){

				outputName = argv[2];
				o=1;
			}



			if(strcmp(argv[3], "-d") != 0 && strcmp(argv[3], "-o") != 0 && strcmp(argv[3], "-c") != 0){
				printf("Incorrect params.\n");
				return 0;
			}
			if(strcmp(argv[3], "-d") == 0){
				if(d) { printf("Incorrect params.\n"); return 0;}                
				dirName = argv[4];
				d=1;
			}
			else if(strcmp(argv[3], "-c") == 0){
				if(c) { printf("Incorrect params.\n"); return 0;}
				colName = argv[4];
				c=1;	
			}
			else if(strcmp(argv[3], "-o") == 0){
				if(o) { printf("Incorrect params.\n"); return 0;}
				outputName = argv[4];
				o=1;
			}


			if(strcmp(argv[5], "-d") != 0 && strcmp(argv[5], "-o") != 0 && strcmp(argv[5], "-c") != 0){
				printf("Incorrect params.\n");
				return 0;
			}
			if(strcmp(argv[5], "-d") == 0){
				if(d) { printf("Incorrect params.\n"); return 0;}
				dirName = argv[6];
				d=1;
			}
			else if(strcmp(argv[5], "-c") == 0){
				if(c) { printf("Incorrect params.\n"); return 0;}
				colName = argv[6];	
				c=1;
			}
			else if(strcmp(argv[5], "-o") == 0){
				if(o) { printf("Incorrect params.\n"); return 0;}
				outputName = argv[6];
				o=1;
			}


		}
		else{
			printf("Incorrect params.\n");
			return 0;
		}
	}

	if(c == 0){
		printf("-c must be provided\n");
		return 0;
	}

	if(dirName == NULL){
		dirName = (char*)malloc(256);
		dirName = getcwd(dirName, 256);

	}
	if(outputName == NULL){
		outputName = (char*)malloc(256);
		outputName = getcwd(outputName, 256);

	}


	initialTID = syscall(__NR_gettid);
	printf("Initial TID: %d\n", initialTID);
	printf("TIDs of all child threads: ");

	outputfile = (char*)malloc(strlen(outputName) + 25 + strlen(colName) );

	strcpy(outputfile, outputName);
	strcat(outputfile, "/");
	strcat(outputfile, "AllFiles-sorted-");
	strcat(outputfile, colName);
	strcat(outputfile, ".csv");

	//global array that stores sorted version of each file will be initialized to start with 5000 strings
	globArr = (char**)malloc(sizeof(char*) * 5000);

	int ctr;

	if(pthread_mutex_init(&lock, NULL)==-1) printf("mutex failed\n"); 


	struct trav_param* param = (struct trav_param*)malloc(sizeof(struct trav_param));
	param->rootDirName = dirName;
	param->colName = colName;
	traverse(param);
	free(param);
	printf("\nTotal number of threads: %d\n", threadCount);
	pthread_mutex_lock(&lock);

	Node* head=NULL;
	if(colNum < 0){
		printf("invalid column or no csvs found in this dir\n");
		pthread_mutex_unlock(&lock);
		return 0;
	}
	FILE* fp = fopen(outputfile, "wb");
	fprintf(fp, "%s", globArr[0]);

		//COPY STARTING FROM HERE

	for(ctr=1;ctr<globArrEnd;ctr++){//take each string in globArr, make a node, sort the linked list, loop through the list and fprintf


		char* cellWithSpaces = getCellAtInd(strdup(globArr[ctr]), colNum);

		if(strcmp(cellWithSpaces, "Error") == 0){
			printf("Error has occurred\n");
			fclose(fp);
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
			return 0;

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
		fprintf(fp, "%s", globArr[ptr->id]);
		free(globArr[ptr->id]);
		prev = ptr;
		ptr=ptr->next;
		free(prev);
	} 


			//COPY ENDS HERE

	pthread_mutex_unlock(&lock);


	return 0;
}


void* traverse(void* param){

	if(syscall(__NR_gettid) != initialTID){
		printf("%ld,", syscall(__NR_gettid));
		fflush(stdout);
	}

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


				pthread_create(&thread, NULL, sortFile, new);
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


void* sortFile(void* param){

	struct sort_param* sort = (struct sort_param*) param;
	char* colName = sort->colName;
	//char* fileName = sort->fileName;
	FILE* fp = sort->fp;
	char* titleRow = NULL;

	printf("%ld,", syscall(__NR_gettid));
	fflush(stdout);

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
	if(finalSort==0){ //only insert the titleRow once
		insertArr(titleRow);
		finalSort=1;
	}
	if(colNum == -2){
		colNum = getColNum(titleRow, colName);
	}
	if(colNum == -1){
		printf("Error has occured.\n");
		fclose(fp);        
		pthread_exit(NULL);
	}
	pthread_mutex_unlock(&lock);
	// find what column # the title is in




	char* row = getRow(fp); // make file pointer and pass as param
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

	pthread_exit(NULL);
	return 0;
}


void insertArr(char* str){

	if(globArrEnd + 10 > numElems){

		globArr = moreCapacity(globArr, numElems);
		numElems *= 1.5;
	}


	globArr[globArrEnd] = strdup(str);
	globArrEnd++;




}




