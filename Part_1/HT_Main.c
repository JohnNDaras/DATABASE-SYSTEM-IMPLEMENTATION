#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HT.h"
#include "BF.h"

#define FILENAME "HT_File"

int main(int argc, char** argv) {
	//variables for file descriptor
	int fd;

	//variables for HP_info
	HT_info *info;

	//variables regarding records
	Record *rec;
	int id;
	char name[15];
	char surname[25];
	char address[50];

	//other variables
	int value;
	void *val;

	FILE *fptr;

	//Initialize block level for hash file
	HT_Init();

	//Open file with records
	if((fptr = fopen(argv[1],"r")) == NULL){
		printf("Error! opening file");
		exit(1);
	}

	//Create new hash file
	if((fd = HT_CreateIndex(FILENAME, 'i', "id", sizeof(int), 10)) == -1){
		printf("Failure\n");
		return 1;
	}

	//open heap file
	info = HT_OpenIndex(FILENAME);

	//check if heap file opened correctly
	if(info->fileDesc == -1){
		printf("Failed to open file");
		return 1;
	}

	//read all records
	while(feof(fptr) == 0){
		fscanf(fptr,"{%d,\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"}\n", &id, name, surname, address);
		HT_InsertEntry(*info, CreateRecord(id, name, surname, address));
	}

	//close file for records
	fclose(fptr);

	printf("Printing All Records\n");
	if(HT_GetAllEntries(*info, NULL) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	value = 10;
	val = &value;

	printf("\nDeleting Record With ID %d\n", value);
	if(HT_DeleteEntry(*info, val)){
		printf("Something went wrong\n");
		return 1;
	}

	printf("\nPrinting Record With ID %d\n", value);
	if(HT_GetAllEntries(*info, val) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	printf("\nPrinting All Records\n");
	if(HT_GetAllEntries(*info, NULL) < 0){
		printf("Something went wrong\n");
		return 1;
	}

        //close hash file
	if(HT_CloseIndex(info) == -1){
		printf("Failed to close file");
		return 1;
	}

	HashStatistics(FILENAME);

	return 0;
}

