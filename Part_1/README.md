**<p align = center>Block-Level Database Management 1**

Compile:

    make all

Run:
    
    make run


**Overview**

In this project, we implement key functionalities for a Database Management System (DBMS) to efficiently manage files at both the block and record levels. Our primary focus is on understanding the intricacies of heap file organization and static hashing (Hash Table) to enhance the overall performance of a DBMS, particularly through the utilization of indexes on records.
Implemented Record Structure

To support our database operations, we define a record structure as follows:

    typedef struct {
        int id;
        char name[15];
        char surname[25];
        char address[50];
    } Record;

**Block File (BF) Functions**

We leverage the BF library functions to manage the block-level operations. These functions encompass initialization, file creation, opening, closing, block allocation, and read/write operations. 

    BF_Init(): Initializes the BF level.
    BF_CreateFile(char* filename): Creates a file with the given filename, deleting the old file if it exists. Returns 0 on success, a negative number on failure.
    BF_OpenFile(char* filename): Opens an existing block file with the given filename. Returns file identifier on success, a negative number on failure.
    BF_CloseFile(int fileDesc): Closes the open file with the file identifier fileDesc. Returns 0 on success, a negative number on failure.
    BF_GetBlockCounter(int fileDesc): Returns the number of available blocks for the open file with file identifier fileDesc. Returns a negative number on failure.
    BF_AllocateBlock(int fileDesc): Allocates a new block for the file with file identifier fileDesc at the end. Returns 0 on success, a negative number on failure.
    BF_ReadBlock(int fileDesc, int blockNumber, void** block): Reads the block with blockNumber of the open file with file identifier fileDesc. Returns 0 on success, a negative number on failure.
    BF_WriteBlock(int fileDesc, int blockNumber): Writes the allocated block with blockNumber to the disk of the open file with file identifier fileDesc. Returns 0 on success, a negative number on failure.
    BF_PrintError(char* message): Prints error messages to stderr followed by a description of the most recent error.

**Heap File (HP) Functions**

Our implementation of heap file management includes the following functions:

    HP_CreateFile: Creates and appropriately initializes an empty heap file.
    HP_OpenFile: Opens a heap file, reads information from the first block, and updates a structure with file details.
    HP_CloseFile: Responsible for closing a heap file and releasing associated memory.
    HP_InsertEntry: Inserts a record into the heap file.
    HP_DeleteEntry: Deletes a record from the heap file.
    HP_GetAllEntries: Prints all records in the heap file with a specified key field value.

**Hash Table (HT) Functions**



We implement hash table functions to manage static hashing files:

    HT_CreateIndex: Creates and initializes an empty hash file.
    HT_OpenIndex: Opens a hash file, reads information from the first block, and updates a structure with file details.
    HT_CloseIndex: Closes a hash file and releases associated memory.
    HT_InsertEntry: Inserts a record into the hash file.
    HT_DeleteEntry: Deletes a record from the hash file.
    HT_GetAllEntries: Prints all records in the hash file with a specified key field value.\\

**Hashing Statistics**

To evaluate our implemented hashing functions, we introduce the HashStatistics function. This function reads a hash file and prints detailed statistics, including the number of blocks, minimum/average/maximum records per bucket, average blocks per bucket, and overflow block information.

    HashStatistics(char* filename): Reads the file with the given filename, prints statistics, and returns 0 on success, -1 on failure.

**Main Function for Functionality Testing**

We include a comprehensive main function for functionality testing. This function:
* Creates a heap file or a static hashing file.
* Inserts indicative records (from provided files).
* Searches for a subset of records based on criteria regarding the key field.


**Conclusion**

The implemented functionalities showcase our collective understanding and successful application of key concepts in Database Systems. Thorough testing is conducted using the provided main test function and additional tests as needed. The project includes proper error handling and detailed comments for each function's functionality and parameters.


