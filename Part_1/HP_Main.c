#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HP.h"
#include "BF.h"

#define FILENAME "HP_File"

int main(int argc, char** argv) {
	//variables for file descriptor
	int fd;

	//variables for HP_info
	HP_info *info;

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

	//Initialize block level for heap file
	HP_Init();

	//Open file with records
	if((fptr = fopen(argv[1],"r")) == NULL){
		printf("Error! opening file");
		return 1;
	}

	//Create new heap file
	if((fd = HP_CreateFile(FILENAME, 'i', "id", sizeof(int))) == -1){
		printf("Failure\n");
		return 1;
	}

	//open heap file
	info = HP_OpenFile(FILENAME);

	//check if heap file opened correctly
	if(info->fileDesc == -1){
		printf("Failed to open file");
		return 1;
	}

//	name = malloc(15 * sizeof(char));
//	surname = malloc(25 * sizeof(char));
//	address = malloc(50 * sizeof(char));

	//read all records
	while(feof(fptr) == 0){
		fscanf(fptr,"{%d,\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"}\n", &id, name, surname, address);
		HP_InsertEntry(*info, CreateRecord(id, name, surname, address));
	}

	//close file for records
	fclose(fptr);

	printf("Printing All Records\n");
	if(HP_GetAllEntries(*info, NULL) < 0){
		printf("Something went wrong\n");
		return 1;
	}

//	free(name);
//	free(surname);
//	free(address);


	value = 10;
	val = &value;

	printf("\nDeleting Record With ID %d\n", value);
	if(HP_DeleteEntry(*info, val)){
		printf("Something went wrong\n");
		return 1;
	}

	printf("\nPrinting Record With ID %d\n", value);
	if(HP_GetAllEntries(*info, val) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	value = 900;
	val = &value;

	printf("\nDeleting Record With ID %d\n", value);
	if(HP_DeleteEntry(*info, val)){
		printf("Something went wrong\n");
		return 1;
	}

	//Inserting new record
	id = 1000;
	strcpy(name, "name_1000");
	strcpy(surname, "surname_1000");
	strcpy(address, "address_1000");

	HP_InsertEntry(*info, CreateRecord(id, name, surname, address));

	id = 155;
	strcpy(name, "name_155");
	strcpy(surname, "surname_155");
	strcpy(address, "address_155");

	HP_InsertEntry(*info, CreateRecord(id, name, surname, address));

	value = 1000;
	val = &value;

	printf("\nPrinting Record With ID %d\n", value);
	if(HP_GetAllEntries(*info, val) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	printf("Printing All Records\n");
	if(HP_GetAllEntries(*info, NULL) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	//close heap file
	if(HP_CloseFile(info) == -1){
		printf("Failed to close file");
		return 1;
	}

	return 0;
}
