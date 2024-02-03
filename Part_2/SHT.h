#pragma once

#include "Types.h"

//hash function for secondary hash table
int shash(char *, int);

//Creates new hash file with name = sfilename
//Returns 0 if succeeded else -1
int SHT_CreateSecondaryIndex(char *, char *, int, int, char *);

//Opens hash file with name = sfilename
//Returns the new data structure if succeeded else NULL
SHT_info* SHT_OpenSecondaryIndex(char *);

//Closes hash file with name = sfilename
//Returns 0 if succeeded else -1
int SHT_CloseSecondaryIndex(SHT_info *);

//Inserts new record in hash file
//Returns 0 if succeeded else -1
int SHT_SecondaryInsertEntry(SHT_info, SecondaryRecord);

//Prints all records with surname = value
//Returns number of blocks it needed to find all values if succeeded else -1
int SHT_SecondaryGetAllEntries(SHT_info, HT_info, void *);

//Finds record in primary hash table with surname = record.surname
//and creates new SecondaryRecord
SecondaryRecord *FindBlock(HT_info , Record);