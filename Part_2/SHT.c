#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SHT.h"
#include "BF.h"

//variables to know where every information of first block is
int size5, size6, size7, size8;

int shash(char *surname, int bucket){
	int h=0, a=90;

	for (; *surname!='\0'; surname++){
		h=(a*h + *surname) % bucket;
	}

//	h = surname[8] % bucket;

	return h;
}

int SHT_CreateSecondaryIndex(char *sfileName, char *attrName, int attrLength, int buckets, char *fileName){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//other variables
	int size;

	//Create new file
	if(BF_CreateFile(sfileName) < 0){						//check if there was an error
		BF_PrintError("Error creating file");
		return -1;
	}

	//Open it
	if((fd = BF_OpenFile(sfileName)) < 0){					//check if there was an error
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
	strcpy(block, "Secondary Hash");
	memcpy(block, "Secondary Hash", strlen("Secondary Hash") + 1);
	size = sizeof("Secondary Hash") + 1;
	size5 = size;

	memcpy(block + size, attrName, strlen(attrName) + 1);
	size += strlen(attrName) + 1;
	size6 = size;

	memcpy(block + size, &attrLength, sizeof(int));
	size += sizeof(int);
	size7 = size;

	memcpy(block + size, &buckets, sizeof(buckets));
	size += sizeof(int);
	size8 = size;

	memcpy(block + size, fileName, strlen(fileName) + 1);

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

SHT_info* SHT_OpenSecondaryIndex(char *sfileName){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//variables for HP_info
	SHT_info *info;
	char *name;
	int *length;
	int *buckets;
	char *filename;

	//Open file
	if((fd = BF_OpenFile(sfileName)) < 0){					//check if there was an error
		BF_PrintError("Error opening file");
		return NULL;
	}

	//Get data of first block
	if(BF_ReadBlock(fd, 0, &block) < 0){					//check if there was an error
		BF_PrintError("Error getting block");
		return NULL;
	}

	//Check if it is a hash file
	if(memcmp(block, "Secondary Hash", strlen("Secondary Hash") + 1) != 0){
		printf("Not Secondary Hash File\n");
		return NULL;
	}

	//Create HT_info with all information we need
	info = malloc(sizeof(SHT_info));

	info->fileDesc = fd;

	name = block + size5;
	info->attrName = name;

	length = block + size6;
	info->attrLength = *length;

	buckets = block + size7;
	info->numBuckets = *buckets;

	filename = block + size8;
	info->fileName = filename;

	//Return the data structure
	return info;
}

int SHT_CloseSecondaryIndex(SHT_info *header_info){
	//Close file
	if(BF_CloseFile(header_info->fileDesc) == 0){		//check if there was an error
		//free data structure
		free(header_info);
		return 0;
	}

	return -1;
}

int SHT_SecondaryInsertEntry(SHT_info header_info, SecondaryRecord record){
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
	index = shash(record.record.surname, buckets);

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
		memcpy(block + sizeof(int), &(record.record.surname), sizeof(record.record.surname));
		memcpy(block + sizeof(int) + sizeof(record.record.surname), &(record.blockId), sizeof(int));
		memcpy(block + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));

		//Write block to disc
		if(BF_WriteBlock(fd, 2) < 0){											//check if there was an error
			BF_PrintError("Error writing block back");
			return -1;
		}

		//return block new record was inserted to
		return 0;
	}
	//If we have blocks with records
	else{

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
			memcpy(block + sizeof(int), &(record.record.surname), sizeof(record.record.surname));
			memcpy(block + sizeof(int) + sizeof(record.record.surname), &(record.blockId), sizeof(int));
			memcpy(block + BLOCK_SIZE - sizeof(int), &overflow, sizeof(int));

			//Write block to disc
			if(BF_WriteBlock(fd, blocks_num - 1) < 0){							//check if there was an error
				BF_PrintError("Error writing block back");
				return -1;
			}

			//Return the block's number new record was inserted to
			return 0;
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
			availableSpace = BLOCK_SIZE - 2 * sizeof(int) - num_of_records * (sizeof(record.record.surname) + sizeof(int));

			//If there is still available space
			if((sizeof(int) + strlen(record.record.surname) + 1) < availableSpace){
				//Write new information to block
				memcpy(block + sizeof(int) + num_of_records * (sizeof(record.record.surname) + sizeof(int)), &(record.record.surname), sizeof(record.record.surname));
				memcpy(block + sizeof(int) + num_of_records * (sizeof(record.record.surname) + sizeof(int)) + sizeof(record.record.surname), &(record.blockId), sizeof(int));
				num_of_records++;
				memcpy(block, &num_of_records, sizeof(int));

				//Write block to disc
				if(BF_WriteBlock(fd, next) < 0){								//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				//Return the block's number new record was inserted to
				return 0;
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
					memcpy(overflowBlock + sizeof(int), &(record.record.surname), sizeof(record.record.surname));
					memcpy(overflowBlock + sizeof(int) + sizeof(record.record.surname), &(record.blockId), sizeof(int));
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
					return 0;
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
					availableSpace = BLOCK_SIZE - 2 * sizeof(int) - num_of_records * (sizeof(record.record.surname) + sizeof(int));

					//If there is still available space
					if(sizeof(Record) < availableSpace){

						//Write new information to block
						memcpy(block + sizeof(int) + num_of_records * (sizeof(record.record.surname) + sizeof(int)), &(record.record.surname), sizeof(record.record.surname));
						memcpy(block + sizeof(int) + num_of_records * (sizeof(record.record.surname) + sizeof(int)) + sizeof(record.record.surname), &(record.blockId), sizeof(int));
						num_of_records++;
						memcpy(block, &num_of_records, sizeof(int));

						//Write block to disc
						if(BF_WriteBlock(fd, hash_table[index].last) < 0){		//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//Return the block's number new record was inserted to
						return 0;
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
						memcpy(overflowBlock + sizeof(int), &(record.record.surname), sizeof(record.record.surname));
						memcpy(overflowBlock + sizeof(int) + sizeof(record.record.surname), &(record.blockId), sizeof(int));

						//Write data to disc
						if(BF_WriteBlock(fd, blocks_num) < 0){					//check if there was an error
							BF_PrintError("Error writing block back");
							return -1;
						}

						//Return the block's number new record was inserted to
						return 0;
					}
				}
			}
		}
	}
}

int SHT_SecondaryGetAllEntries(SHT_info header_info_sht, HT_info header_info_ht, void *value){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	void *prim_block;
	int blocks_num;
	int next;

	//variables for file descriptor
	int sht_fd;
	int ht_fd;

	//variables regarding records
	Record record;
	int num_of_records;
	int prim_num_of_records;
	char surname[25];
	int blockID;

	//variables for hash table
	Index *hash_table;
	int sht_buckets;
	int ht_buckets;
	int index;

	//other variables
	int i;
	char val[25];
	int blocks_read = 0;

	sht_fd = header_info_sht.fileDesc;
	sht_buckets = header_info_sht.numBuckets;

	ht_fd = header_info_ht.fileDesc;
	ht_buckets = header_info_ht.numBuckets;

	//Get data from block with the information about the hash table
	if(BF_ReadBlock(sht_fd, 1, &indexBlock) < 0){			//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Get array of hash table
	hash_table = malloc(sht_buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, sht_buckets * sizeof(Index));

	//Get id asked to be found
	memcpy(&val, value, sizeof(val));

	index = shash(val, sht_buckets);

	//Get first block of index the hash function returned
	next = hash_table[index].first;

	//While index has blocks
	while(next){
		//Find data of block with number = next
		if(BF_ReadBlock(sht_fd, next, &block) < 0){			//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		blocks_read++;

		//Get how many records current block has
		memcpy(&num_of_records, block, sizeof(int));

		//Check all records
		for(i = 0; i < num_of_records; i++){
			//Get i record
			memcpy(surname, block + sizeof(int) + i * (sizeof(char) * 25 + sizeof(int)), sizeof(record.surname));

			//If we find record with id = value
			if(strcmp(surname, val) == 0){

				memcpy(&blockID, block + sizeof(int) + i * (sizeof(record.surname) + sizeof(int)) + sizeof(char) * 25, sizeof(int));

				if(BF_ReadBlock(ht_fd, blockID, &prim_block) < 0){			//check if there was an error
					BF_PrintError("Error getting block");
					return -1;
				}

				memcpy(&prim_num_of_records, prim_block, sizeof(int));

				for(int j = 0; j < prim_num_of_records; j++){

					memcpy(&record, prim_block + sizeof(int) + j * sizeof(Record), sizeof(Record));

					if(strcmp(surname, record.surname) == 0){
						printf("id: %d | name: %s | surname: %s | address: %s\n", record.id, record.name, record.surname, record.address);
					}
				}
			}
		}

		//Go to next block of current index
		memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
	}

	free(hash_table);

	return 0;
}
