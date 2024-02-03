#include "Record.h"

//Data structure with information about heap file
typedef struct {
	int fileDesc;
	char attrType;
	char *attrName;
	int attrLength;
}HP_info;

//Initialize block level for heap file
void HP_Init(void);

//Creates new heap file with name = filename
//Returns 0 if succeeded else -1
int HP_CreateFile(char *, char, char *, int);

//Opens heap file with name = filename
//Returns the new data structure about the heap file if succeeded else NULL
HP_info* HP_OpenFile(char *);

//Closes heap file with name = filename
//Returns 0 if succeeded else -1
int HP_CloseFile(HP_info *);

//Inserts new record in heap file
//Returns block's number that new record was inserted to if succeeded else -1
int HP_InsertEntry(HP_info, Record);

//Deletes record with id = value
//Returns 0 if succeeded else -1
int HP_DeleteEntry(HP_info, void *);

//Prints all records in heap file if value = NULL
//Else searches record with id = value and if it is found, it is printed
//Returns number of blocks it needed to find value if succeeded else -1
int HP_GetAllEntries(HP_info, void *);

//Creates new record and returns it
Record CreateRecord(int, char *, char *, char *);
