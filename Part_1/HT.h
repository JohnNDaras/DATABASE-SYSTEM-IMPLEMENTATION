#include "Record.h"

//Data structure with information about heap file
typedef struct {
	int fileDesc;
	char attrType;
	char *attrName;
	int attrLength;
	long int numBuckets;
}HT_info;

//Data structure with information about the hash table
typedef struct IndElem{
	int first;			//first block of every index in the hash table
	int last;			//last block of every index in the hash table
}Index;

//Initialize block level for hash file
void HT_Init(void);

//Creates new hash file with name = filename
//Returns 0 if succeeded else -1
int HT_CreateIndex(char *, char, char*, int, int);

//Opens hash file with name = filename
//Returns the new data structure if succeeded else NULL
HT_info* HT_OpenIndex(char *);

//Closes hash file with name = filename
//Returns 0 if succeeded else -1
int HT_CloseIndex(HT_info *);

//Inserts new record in hash file
//Returns block's number that new record was inserted to if succeeded else -1
int HT_InsertEntry(HT_info, Record);

//Deletes record with id = value
//Returns 0 if succeeded else -1
int HT_DeleteEntry(HT_info, void *);

//Prints all records in hash file if value = NULL
//Else searches record with id = value and if it is found, it is printed
//Returns number of blocks it needed to find value if succeeded else -1
int HT_GetAllEntries(HT_info, void *);

//Gets statistics of hash table in hash file with name = filename
int HashStatistics(char*);

//Creates new record and returns it
Record CreateRecord(int, char *, char *, char *);
