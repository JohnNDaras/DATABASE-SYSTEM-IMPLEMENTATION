**<p align = center>Block-Level Database Management 2**


Compile:
    
    make all

Run:
    
    make run


**Project Overview**

This project delves into the intricacies of Database Systems, specifically focusing on block-level and record-level management. The implementation includes functions for handling files based on heap file organization and static hashing.
Record Structure

The record structure used for both heap files and static hashing files is defined as follows:

   typedef struct{

**Block File Functions (BF)**

The block file functions, encapsulated in BF.h, provide essential block-level management functionalities:

    BF_Init(): Initializes the BF level.
    BF_CreateFile(char* filename): Creates a file with the given name.
    BF_OpenFile(char* filename): Opens an existing block file.
    BF_GetBlockCounter(int fileDesc): Retrieves the number of available blocks in a file.
    BF_AllocateBlock(int fileDesc): Allocates a new block for the file.
    BF_ReadBlock(int fileDesc, int blockNumber, void** block): Reads a block from the file.
    BF_WriteBlock(int fileDesc, int blockNumber): Writes a block to the file.
    BF_PrintError(char* message): Prints errors related to BF functions.

**HP Functions (Heap File)**

For heap files, we implement the following functions:

    HP_CreateFile(char *fileName, char attrType, char* attrName, int attrLength): Creates and initializes an empty heap file.
    HP_OpenFile(char *fileName): Opens a heap file and reads file information.
    HP_CloseFile(HP_info* header_info): Closes a heap file and frees memory.
    HP_InsertEntry(HP_info header_info, Record record): Inserts a record into the heap file.
    HP_DeleteEntry(HP_info header_info, void *value): Deletes a record from the heap file.
    HP_GetAllEntries(HP_info header_info, void *value): Prints records with a specified key value.

**HT Functions (Hash Table)**

The hash table (HT) functions include:

    HT_CreateIndex(char *fileName, char attrType, char* attrName, int attrLength, int buckets): Creates and initializes an empty hash file.
    HT_OpenIndex(char *fileName): Opens a hash file and reads file information.
    HT_CloseIndex(HT_info* header_info): Closes a hash file and frees memory.
    HT_InsertEntry(HT_info header_info, Record record): Inserts a record into the hash file.
    HT_DeleteEntry(HT_info header_info, void *value): Deletes a record from the hash file.
    HT_GetAllEntries(HT_info header_info, void *value): Prints records with a specified key value.

**SHT Functions (Secondary Hash Table)**

For creating and managing a secondary static hash index on the surname attribute, we implemented the following functions:

    SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, int buckets, char *fileName): Creates and initializes a secondary hash file.
    SHT_OpenSecondaryIndex(char *sfileName): Opens a secondary hash file and reads file information.
    SHT_CloseSecondaryIndex(SHT_info* header_info): Closes a secondary hash file and frees memory.
    SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record): Inserts a record into the secondary hash file.
    SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value): Prints records with a specified key value in the secondary index.

**Hashing Statistics**

To evaluate the implemented hash functions, we include the HashStatistics(char* filename) function. This function reads a file and prints statistics such as the number of blocks, minimum/average/maximum records per bucket, average blocks per bucket, and information on buckets with overflow blocks.
Main Testing Function

For functionality testing, a main testing function is provided. It covers the following scenarios:

    Creation of a static hashing file (HT).
    Creation of a secondary index (SHT) on the last name (surname) field.
    Insertion of indicative records from provided files.
    Searching for records based on the key field (id).
    Searching for records using the last name (surname) field in the secondary index.

The main testing function ensures comprehensive validation of the correctness of the implemented functions.
