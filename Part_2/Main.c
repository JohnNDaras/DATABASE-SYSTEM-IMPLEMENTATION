#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HT.h"
#include "SHT.h"
#include "BF.h"

#define PFILENAME "Primary"
#define SFILENAME "Secondary"

int main(int argc, char** argv) {
	//variables for file descriptor
	int fd;

	//variables for HP_info
	HT_info *info;

	//variables for SHP_info
	SHT_info *sinfo;

	//variables regarding records
	Record rec;
	SecondaryRecord *srec;
	int id;
	char name[15];
	char surname[25];
	char address[50];

	//variables for records file
	FILE *fptr;

	//other variables
	int value;
	void *val;

	//Initialize block level for hash file
	HT_Init();

	//Open file with records
	if((fptr = fopen(argv[1],"r")) == NULL){
		printf("Error! opening file\n");
		return 1;
	}

	//Create new hash file
	if((fd = HT_CreateIndex(PFILENAME, 'i', "id", sizeof(int), 128)) == -1){
		printf("Failure\n");
		return 1;
	}

	//Create new hash file
	if((fd = SHT_CreateSecondaryIndex(SFILENAME, "surname", strlen("surname") + 1, 50, PFILENAME)) == -1){
		printf("Failure\n");
		return 1;
	}

	//open heap file
	info = HT_OpenIndex(PFILENAME);

	//check if heap file opened correctly
	if(info->fileDesc == -1){
		printf("Failed to open file");
		return 1;
	}

	//open heap file
	sinfo = SHT_OpenSecondaryIndex(SFILENAME);

	//check if heap file opened correctly
	if(sinfo->fileDesc == -1){
		printf("Failed to open file");
		return 1;
	}

	//read all records
	while(feof(fptr) == 0){
		fscanf(fptr,"{%d,\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"}\n", &(id), name, surname, address);

		rec.id = id;
		strcpy(rec.name,name);
		strcpy(rec.surname,surname);
		strcpy(rec.address, address);

		HT_InsertEntry(*info, rec);

		if((srec = FindBlock(*info, rec)) != NULL){
			SHT_SecondaryInsertEntry(*sinfo, *srec);
		}
	}

	//close file for records
	fclose(fptr);

	strcpy(surname, "surname_256");
	val = &(surname);

	SHT_SecondaryGetAllEntries(*sinfo, *info, val);

	printf("Printing All Records\n");
	if(HT_GetAllEntries(*info, NULL) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	id = 10;
	val = &id;

	printf("\nPrinting Records with ID %d\n", id);
	if(HT_GetAllEntries(*info, val) < 0){
		printf("Something went wrong\n");
		return 1;
	}

	//close hash file
	if(HT_CloseIndex(info) == -1){
		printf("Failed to close file");
		return 1;
	}

	//close hash file
	if(SHT_CloseSecondaryIndex(sinfo) == -1){
		printf("Failed to close file");
		return 1;
	}

	HashStatistics(PFILENAME);

	HashStatistics(SFILENAME);

	return 0;
}

