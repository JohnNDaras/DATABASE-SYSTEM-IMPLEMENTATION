#pragma once
//struct for record
typedef struct{
	int id;
	char name[15];
	char surname[25];
	char address[50];
}Record;

typedef struct IndElem{
	int first;			//first block of every index in the hash table
	int last;			//last block of every index in the hash table
}Index;


//Data structure with information about heap file
typedef struct {
	int fileDesc;
	char attrType;
	char *attrName;
	int attrLength;
	long int numBuckets;
}HT_info;

typedef struct {
	int fileDesc;
	char* attrName;
	int attrLength;
	long int numBuckets;
	char *fileName;
}SHT_info;

typedef struct{
	Record record;
	int blockId;
}SecondaryRecord;
