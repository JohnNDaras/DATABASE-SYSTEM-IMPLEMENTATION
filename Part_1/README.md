**Block-Level Database Management 1**

Compile:

    make all

Run:
    
    make run


Overview

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

We leverage the BF library functions to manage the block-level operations. These functions encompass initialization, file creation, opening, closing, block allocation, and read/write operations. \\



**Heap File (HP) Functions**

Our implementation of heap file management includes the following functions:

    HP_CreateFile: Creates and appropriately initializes an empty heap file.
    HP_OpenFile: Opens a heap file, reads information from the first block, and updates a structure with file details.
    HP_CloseFile: Responsible for closing a heap file and releasing associated memory.
    HP_InsertEntry: Inserts a record into the heap file.
    HP_DeleteEntry: Deletes a record from the heap file.
    HP_GetAllEntries: Prints all records in the heap file with a specified key field value.

**Hash Table (HT) Functions**

\\

We implement hash table functions to manage static hashing files:

    HT_CreateIndex: Creates and initializes an empty hash file.
    HT_OpenIndex: Opens a hash file, reads information from the first block, and updates a structure with file details.
    HT_CloseIndex: Closes a hash file and releases associated memory.
    HT_InsertEntry: Inserts a record into the hash file.
    HT_DeleteEntry: Deletes a record from the hash file.
    HT_GetAllEntries: Prints all records in the hash file with a specified key field value.\\

**Hashing Statistics**

To evaluate our implemented hashing functions, we introduce the HashStatistics function. This function reads a hash file and prints detailed statistics, including the number of blocks, minimum/average/maximum records per bucket, average blocks per bucket, and overflow block information.

We include a comprehensive main function for functionality testing. This function:

    Creates a heap file or a static hashing file.
    Inserts indicative records (from provided files).
    Searches for a subset of records based on criteria regarding the key field.


**Conclusion**

The implemented functionalities showcase our collective understanding and successful application of key concepts in Database Systems. Thorough testing is conducted using the provided main test function and additional tests as needed. The project includes proper error handling and detailed comments for each function's functionality and parameters.


