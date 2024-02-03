#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HT.h"
#include "BF.h"

//variables to know where every information of first block is
int size1, size2, size3, size4;

//Hash Function
int hash (int id, int buckets){
	return id % buckets;
}

//Initialize block level of hash file
void HT_Init(void){
	BF_Init();
}

//Prints all records in hash file
int HT_PrintAll(HT_info header_info){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	int blocks_num;
	int next;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record record;
	int num_of_records;

	//variables for hash table
	Index *hash_table;
	int buckets;

	//other variables
	int i, j;
	int blocks_read = 0;

	fd = header_info.fileDesc;
	buckets = header_info.numBuckets;

	//Get data from block with the information about the hash table
	if(BF_ReadBlock(fd, 1, &indexBlock) < 0){				//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Get array of hash table
	hash_table = malloc(buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, buckets * sizeof(Index));

	//For every index of array
	for(i = 0; i < buckets; i++){
		//Get first block of index i
		next = hash_table[i].first;

		//While index i has blocks
		while(next){
			//Get data of block with number = next
			if(BF_ReadBlock(fd, next, &block) < 0){			//check if there was an error
				BF_PrintError("Error getting block");
				return -1;
			}

			blocks_read++;

			//Get how many records block next has
			memcpy(&num_of_records, block, sizeof(int));

			//Check all records
			for(j = 0; j < num_of_records; j++){
				//Get j record
				memcpy(&record, block + sizeof(int) + j * sizeof(Record), sizeof(Record));

				//Print it
				printf("id: %d | name: %s | surname: %s | address: %s\n", record.id, record.name, record.surname, record.address);
			}

			//Go to next block of current index
			memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
		}
	}

	free(hash_table);

	//Return number of blocks needed to be read in order to print all records
	return blocks_read;
}

//Searches record with id = value in hash file
int HT_Search(HT_info header_info, void *value, int mode){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	int blocks_num;
	int next;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record record;
	int num_of_records;

	//variables for hash table
	Index *hash_table;
	int buckets;
	int index;

	//other variables
	int i;
	int val;
	int blocks_read = 0;

	fd = header_info.fileDesc;
	buckets = header_info.numBuckets;

	//Get data from block with the information about the hash table
	if(BF_ReadBlock(fd, 1, &indexBlock) < 0){			//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Get array of hash table
	hash_table = malloc(buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, buckets * sizeof(Index));

	//Get id asked to be found
	memcpy(&val, value, sizeof(int));

	index = hash(val, buckets);

	//Get first block of index the hash function returned
	next = hash_table[index].first;

	//While index has blocks
	while(next){
		//Find data of block with number = next
		if(BF_ReadBlock(fd, next, &block) < 0){			//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		blocks_read++;

		//Get how many records current block has
		memcpy(&num_of_records, block, sizeof(int));

		//Check all records
		for(i = 0; i < num_of_records; i++){
			//Get i record
			memcpy(&record, block + sizeof(int) + i * sizeof(Record), sizeof(Record));

			//If we find record with id = value
			if(record.id == val){
				//if HT_Search was called by HT_GetAllEntries
				//print record
				if(mode == 1){
					printf("id: %d | name: %s | surname: %s | address: %s\n", record.id, record.name, record.surname, record.address);
				}

				//Return number of blocks needed to find value
				return blocks_read;
			}
		}

		//Go to next block of current index
		memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
	}

	free(hash_table);

	//if HT_Search was called by HT_GetAllEntries
	if(mode == 1){
		printf("Record with id %d not found\n", val);

		//Return total number of blocks read
		return blocks_read;
	}

	return 0;
}

int HT_CreateIndex(char *fileName, char attrType, char *attrName, int attrLength, int buckets){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//other variables
	int size;

	//Create new file
	if(BF_CreateFile(fileName) < 0){						//check if there was an error
		BF_PrintError("Error creating file");
		return -1;
	}

	//Open it
	if((fd = BF_OpenFile(fileName)) < 0){					//check if there was an error
		BF_PrintError("Error opening file");
		return -1;
	}

	//Create a new block
	if(BF_AllocateBlock(fd) < 0) {							//check if there was an error
		BF_PrintError("Error allocating block");
		return -1;
	}

	//Get data of first block
	if(BF_ReadBlock(fd, 0, &block) < 0){					//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Write needed information to block
	strcpy(block, "Hash");
	memcpy(block, "Hash", strlen("Hash") + 1);
	size = sizeof("Hash") + 1;
	size1 = size;

	memcpy(block + size, &attrType, sizeof(int));
	size += sizeof(int);
	size2 = size;

	memcpy(block + size, attrName, strlen(attrName) + 1);
	size += strlen(attrName) + 1;
	size3 = size;

	memcpy(block + size, &attrLength, sizeof(int));
	size += sizeof(int);
	size4 = size;

	memcpy(block + size, &buckets, sizeof(buckets));

	//Write block to disc
	if(BF_WriteBlock(fd, 0) < 0){							//check if there was an error
		BF_PrintError("Error writing block back");
		return -1;
	}

	//Close file
	if(BF_CloseFile(fd) < 0) {								//check if there was an error
		BF_PrintError("Error closing file");
		return -1;
	}

	return 0;
}

HT_info* HT_OpenIndex(char *fileName){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//variables for HP_info
	HT_info *info;
	int *type;
	char *name;
	int *length;
	int *buckets;

	//Open file
	if((fd = BF_OpenFile(fileName)) < 0){					//check if there was an error
		BF_PrintError("Error opening file");
		return NULL;
	}

	//Get data of first block
	if(BF_ReadBlock(fd, 0, &block) < 0){					//check if there was an error
		BF_PrintError("Error getting block");
		return NULL;
	}

	//Check if it is a hash file
	if(memcmp(block, "Hash", strlen("Hash") + 1) != 0){
		printf("Not Hash File\n");
		return NULL;
	}

	//Create HT_info with all information we need
	info = malloc(sizeof(HT_info));

	info->fileDesc = fd;

	type = block + size1;
	info->attrType = *type;

	name = block + size2;
	info->attrName = name;

	length = block + size3;
	info->attrLength = *length;

	buckets = block + size4;
	info->numBuckets = *buckets;

	//Return the data structure
	return info;
}

int HT_CloseIndex(HT_info *header_info){

	//Close file
	if(BF_CloseFile(header_info->fileDesc) == 0){		//check if there was an error
		//free data structure
		free(header_info);
		return 0;
	}

	return -1;
}

int HT_InsertEntry(HT_info header_info, Record record){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	int blocks_num;
	int next;
	int overflow = 0;

	//variables for file descriptor
	int fd;

	//variables regarding records
	int num_of_records;

	//variables for hash table
	Index *hash_table;
	int buckets;
	int index;

	//other variables
	int i;
	int availableSpace;
	void *val;

	fd = header_info.fileDesc;
	buckets = header_info.numBuckets;
	hash_table = malloc(buckets * sizeof(Index));

	//Find in which index of the hash table the new record should be added
	index = hash(record.id, buckets);
	//Get the number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){								//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//If we do not have the hash table
	if(blocks_num == 1){

		//Create new block for hash table
		if(BF_AllocateBlock(fd) < 0){											//check if there was an error
			BF_PrintError("Error allocating block");
			return -1;
		}

		//Create new block for records
		if(BF_AllocateBlock(fd) < 0){											//check if there was an error
			BF_PrintError("Error allocating block");
			return -1;
		}

		//There are not records in the hash table yet
		for(i = 0 ; i < buckets; i++){
			hash_table[i].first = 0;
			hash_table[i].last = 0;
		}

		//First record of index in hash table will be in the second block
		hash_table[index].first = 2;

		//Find hash table in file
		if(BF_ReadBlock(fd, 1, &indexBlock) < 0){								//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		memcpy(indexBlock, hash_table, buckets * sizeof(Index));

		//Write new data to disc
		if(BF_WriteBlock(fd, 1) < 0){											//check if there was an error
			BF_PrintError("Error writing block back");
			return -1;
		}

		//Find new block
		if(BF_ReadBlock(fd, 2, &block) < 0){									//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		//New block only has one record
		num_of_records = 1;

		//Write information to new block
		memcpy(block, &num_of_records, sizeof(int));
		memcpy(block + sizeof(int), &record, sizeof(Record));
		memcpy(block + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));

		//Write block to disc
		if(BF_WriteBlock(fd, 2) < 0){											//check if there was an error
			BF_PrintError("Error writing block back");
			return -1;
		}

		//return block new record was inserted to
		return hash_table[index].first;
	}
	//If we have blocks with records
	else{
		memcpy(val, &(record.id), sizeof(int));

		//Check if record with the same id already exists
		if(HT_Search(header_info, val, 0) != 0){								//check if there was an error
			printf("Record with id %d already exists\n", record.id);
			return 0;
		}

		//Find hash table in file
		if(BF_ReadBlock(fd, 1, &indexBlock) < 0){								//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		memcpy(hash_table, indexBlock, buckets * sizeof(Index));

		//Get number of blocks
		if((blocks_num = BF_GetBlockCounter(fd)) < 0){							//check if there was an error
			BF_PrintError("Error Getting Number Of Blocks");
			return -1;
		}

		//Find first block in index
		int next = hash_table[index].first;

		//If it does not have any records
		if(next == 0){

			//Create new block
			if(BF_AllocateBlock(fd) < 0) {										//check if there was an error
				BF_PrintError("Error allocating block");
				return -1;
			}

			//Get number of blocks
			if((blocks_num = BF_GetBlockCounter(fd)) < 0){						//check if there was an error
				BF_PrintError("Error Getting Number Of Blocks");
				return -1;
			}

			//Write number of first block index with value hRes is pointing to
			hash_table[index].first = blocks_num - 1;

			memcpy(indexBlock, hash_table, buckets * sizeof(Index));

			//Write new data to disc
			if(BF_WriteBlock(fd, 1) < 0){										//check if there was an error
				BF_PrintError("Error writing block back");
				return -1;
			}

			//Get data of new block
			if(BF_ReadBlock(fd, blocks_num - 1, &block) < 0){					//check if there was an error
				BF_PrintError("Error getting block");
				return -1;
			}

			//New block only has one record
			num_of_records = 1;

			//Write information to new block
			memcpy(block, &num_of_records, sizeof(int));
			memcpy(block + sizeof(int), &record, sizeof(Record));
			memcpy(block + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));

			//Write block to disc
			if(BF_WriteBlock(fd, blocks_num - 1) < 0){							//check if there was an error
				BF_PrintError("Error writing block back");
				return -1;
			}

			//Return the block's number new record was inserted to
			return blocks_num - 1;
		}
		//If it has records
		else{
			//Get data of first block of index in hash table
			if(BF_ReadBlock(fd, next, &block) < 0){								//check if there was an error
				BF_PrintError("Error getting block");
				return -1;
			}

			//Find how many records it has
			memcpy(&num_of_records, block, sizeof(int));

			//Find how much space the block still has available
			availableSpace = BLOCK_SIZE - 2 * sizeof(int) - num_of_records * sizeof(record);

			//If there is still available space
			if(sizeof(Record) < availableSpace){
				//Write new information to block
				memcpy(block + sizeof(int) + num_of_records * sizeof(Record), &record, sizeof(Record));
				num_of_records++;
				memcpy(block, &num_of_records, sizeof(int));

				//Write block to disc
				if(BF_WriteBlock(fd, next) < 0){								//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				//Return the block's number new record was inserted to
				return next;
			}
			//If there is not available space
			else{
				//If there is not an overflow block in current index
				if(hash_table[index].last == 0){
					void *overflowBlock;

					//Create new block
					if(BF_AllocateBlock(fd) < 0) {								//check if there was an error
						BF_PrintError("Error allocating block");
						return -1;
					}

					//Get number of blocks
					if((blocks_num = BF_GetBlockCounter(fd)) < 0){				//check if there was an error
						BF_PrintError("Error Getting Number Of Blocks");
						return -1;
					}
					blocks_num--;

					//Block of current index points to new block
					memcpy(block + BLOCK_SIZE - sizeof(int), &blocks_num, sizeof(int));

					//Write data to disc
					if(BF_WriteBlock(fd, next) < 0){							//check if there was an error
						BF_PrintError("Error writing block back");
						return -1;
					}

					//Find new block
					if(BF_ReadBlock(fd, blocks_num, &overflowBlock) < 0){		//check if there was an error
						BF_PrintError("Error getting block");
						return -1;
					}

					//new block has one record
					num_of_records = 1;

					//Write data to new block
					memcpy(overflowBlock, &num_of_records, sizeof(int));
					memcpy(overflowBlock + sizeof(int), &record, sizeof(Record));
					overflow = 0;
					memcpy(overflowBlock + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));

					//Write data to disc
					if(BF_WriteBlock(fd, blocks_num) < 0){						//check if there was an error
						BF_PrintError("Error writing block back");
						return -1;
					}

					//Write new information in hash table
					hash_table[index].last = blocks_num;
					memcpy(indexBlock, hash_table, buckets * sizeof(Index));

					//Write data to disc
					if(BF_WriteBlock(fd, 1) < 0){								//check if there was an error
						BF_PrintError("Error writing block back");
						return -1;
					}

					//Return the block's number new record was inserted to
					return blocks_num;
				}
				//If there is an overflow block in current index
				else{
					//Find last block of current index
					if(BF_ReadBlock(fd, hash_table[index].last, &block) < 0){	//check if there was an error
						BF_PrintError("Error getting block");
						return -1;
					}

					//Find how many records it has
					memcpy(&num_of_records, block, sizeof(int));

					//Find how much space the block still has available
					availableSpace = BLOCK_SIZE - 2 * sizeof(int) - num_of_records * sizeof(record);

					//If there is still available space
					if(sizeof(Record) < availableSpace){

						//Write new information to block
						memcpy(block + sizeof(int) + num_of_records * sizeof(Record), &record, sizeof(Record));
						num_of_records++;
						memcpy(block, &num_of_records, sizeof(int));

						//Write block to disc
						if(BF_WriteBlock(fd, hash_table[index].last) < 0){		//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//Return the block's number new record was inserted to
						return hash_table[index].last;
					}
					//If there is not available space
					else{
						void *overflowBlock;

						//Create new block
						if(BF_AllocateBlock(fd) < 0) {							//check if there was an error
							BF_PrintError("Error allocating block");
							return -1;
						}

						//Get number of blocks
						if((blocks_num = BF_GetBlockCounter(fd)) < 0){			//check if there was an error
							BF_PrintError("Error Getting Number Of Blocks");
							return -1;
						}
						blocks_num--;

						//current block points to new block
						memcpy(block + BLOCK_SIZE - sizeof(int), &blocks_num, sizeof(int));

						//new block has one record
						num_of_records = 1;

						//Write data to disc
						if(BF_WriteBlock(fd, hash_table[index].last) < 0){			//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//last block of current index points to new block
						hash_table[index].last = blocks_num;
						memcpy(indexBlock, hash_table, buckets * sizeof(Index));

						//Write data to disc
						if(BF_WriteBlock(fd, 1) < 0){							//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//Find new block
						if(BF_ReadBlock(fd, blocks_num, &overflowBlock) < 0){	//check if there was an error
							BF_PrintError("Error getting block");
							return -1;
						}

						//Write data to new block
						memcpy(overflowBlock, &num_of_records, sizeof(int));
						overflow = 0;
						memcpy(overflowBlock + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));
						memcpy(overflowBlock + sizeof(int), &record, sizeof(Record));

						//Write data to disc
						if(BF_WriteBlock(fd, blocks_num) < 0){					//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//Return the block's number new record was inserted to
						return hash_table[index].last;
					}
				}
			}
		}
	}
}

int HT_DeleteEntry(HT_info header_info, void *value){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	void *lastblock;
	int blocks_num;
	int next;
	int last;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record cur_rec;
	Record last_rec;
	int num_of_records;
	int last_num_rec;

	//variables for hash table
	Index *hash_table;
	int buckets;
	int index;

	//other variables
	int i;
	int val;
	int blocks_read = 0;

	fd = header_info.fileDesc;
	buckets = header_info.numBuckets;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){				//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//Check if we have any records
	if(blocks_num < 2){
		printf("No records\n");
		return 0;
	}

	//Find array in file
	if(BF_ReadBlock(fd, 1, &indexBlock) < 0){					//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Get array of hash table
	hash_table = malloc(buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, buckets * sizeof(Index));

	//Get id asked to be deleted
	memcpy(&val, value, sizeof(int));

	index = hash(val, buckets);

	//Get first block of index
	next = hash_table[index].first;

	//While index has blocks
	while(next){
		//Find data of block with number = next
		if(BF_ReadBlock(fd, next, &block) < 0){
			BF_PrintError("Error getting block");
			return -1;
		}

		//Get how many records current block has
		memcpy(&num_of_records, block, sizeof(int));

		//Check all records
		for(i = 0; i < num_of_records; i++){
			//Get i record
			memcpy(&cur_rec, block + sizeof(int) + i * sizeof(Record), sizeof(Record));

			//If we find record with id = value
			if(cur_rec.id == val){
				last = hash_table[index].last;

				//Get data of last block of current index
				if(BF_ReadBlock(fd, last, &lastblock) < 0){					//check if there was an error
					BF_PrintError("Error getting block");
					return -1;
				}

				//Get number of records of last block
				memcpy(&last_num_rec, lastblock, sizeof(int));

				//Get last record of index
				memcpy(&last_rec, lastblock + sizeof(int) + (last_num_rec - 1) * sizeof(Record), sizeof(Record));

				//Replace current record with the last record
				cur_rec.id = last_rec.id;
				strcpy(cur_rec.name, last_rec.name);
				strcpy(cur_rec.surname, last_rec.surname);
				strcpy(cur_rec.address, last_rec.address);

				//Last block has one less record
				last_num_rec--;

				memcpy(lastblock, &last_num_rec, sizeof(int));

				//Write new information of the last block of index to disc
				if(BF_WriteBlock(fd, last) < 0){							//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				memcpy(block + sizeof(int) + i * sizeof(Record), &cur_rec, sizeof(Record));

				//Write new information of current block to disc
				if(BF_WriteBlock(fd, next) < 0){							//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				printf("Deleted\n");

				//We do not need to check the other records
				return 0;
			}
		}

		//Go to next block of current index
		memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
	}

	free(hash_table);

	//There was not a record with id = value
	printf("Record with id %d not found\n", val);

	return 0;

}

int HT_GetAllEntries(HT_info header_info, void *value){
	//variables for information regarding blocks
	int blocks_num;

	//variables for file descriptor
	int fd;

	fd = header_info.fileDesc;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){						//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//Check if we have any records
	if(blocks_num < 2){
		printf("No records\n");
		return 0;
	}

	//If value = NULL
	//Print all records
	if(value == NULL){
		return HT_PrintAll(header_info);
	}
	//else find record with id = value
	else{
		return HT_Search(header_info, value, 1);
	}

}

int HashStatistics(char *filename){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	int blocks_num;
	int next;
	int *bucks;

	//variables for file descriptor
	int fd;

	//variables for HT_info
	HT_info *info;

	//variables regarding records
	Record record;
	int num_of_records;
	int *rec_num;

	//variables for hash table
	Index *hash_table;
	int buckets;

	//variables for statistics
	int max, min, gpa;

	//other variables
	int sum;

	printf("\nHash Statistics\n");

	//Open hash file
	info = HT_OpenIndex(filename);

	fd = info->fileDesc;
	if(fd == -1){
		printf("Failed to open file");
		return 1;
	}

	buckets = info->numBuckets;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){				//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	printf("\na) Hash File has %d blocks\n", blocks_num);

	if(blocks_num < 2){
		printf("No records\n");
		return 0;
	}

	if(BF_ReadBlock(fd, 1, &indexBlock) < 0){
		BF_PrintError("Error getting block");
		return -1;
	}

	//Get array of hash table
	hash_table = malloc(buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, buckets * sizeof(Index));

	rec_num = malloc(buckets * sizeof(int));
	bucks = malloc(buckets * sizeof(int));

	for(int i = 0; i < buckets; i++){
		rec_num[i] = 0;
		bucks[i] = 0;
	}

	for(int i = 0; i < buckets; i++){
		//Get first block of index i
		next = hash_table[i].first;

		//While index has blocks
		while(next){
			//Find data of block with number = next
			if(BF_ReadBlock(fd, next, &block) < 0){
				BF_PrintError("Error getting block");
				return -1;
			}

			memcpy(&num_of_records, block, sizeof(int));

			rec_num[i] += num_of_records;
			bucks[i]++;

			//Go to next block of current index
			memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
		}
	}

	//Find max number of records
	max = 0;

	for(int i = 0; i < buckets; i++){
		if(rec_num[i] > max){
			max = rec_num[i];
		}
	}

	printf("\nb) Max number of records in a bucket: %d\n", max);

	//Find min number of records
	min = max;

	for(int i = 0; i < buckets; i++){
		if(rec_num[i] < min){
			min = rec_num[i];
		}
	}

	printf("   Min number of records in a bucket: %d\n", min);

	//Find average number of records
	sum = 0;

	for(int i = 0; i < buckets; i++){
		sum += rec_num[i];
	}

	printf("   Average number of records in file: %0.2lf\n", (double) sum / buckets);

	//Average number of blocks
	printf("\nc) Average number of block in bucket %d in file: %0.2lf\n", 0, (double) bucks[0] / (blocks_num - 2));
	for(int i = 1; i < buckets; i++){
		printf("   Average number of block in bucket %d in file: %0.2lf\n", i, (double) bucks[i] / (blocks_num - 2));
	}

	//Find overflow blocks for every bucket
	sum = 0;

	printf("\nd) Bucket %d has %d blocks\n", 0, bucks[0]);
	for(int i = 1; i < buckets; i++){
		if(bucks[i] > 1){
			printf("   Bucket %d has %d blocks\n", i, bucks[i]);
			sum++;
		}
	}

	printf("   %d buckets have overflow blocks\n", sum);

	free(hash_table);

	//close hash file
	if(HT_CloseIndex(info) == -1){
		printf("Failed to close file");
		return 1;
	}

	return 0;
}
