#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HP.h"
#include "BF.h"

//Initialize block level for heap file
void HP_Init(void){
	BF_Init();
}

int HP_PrintAll(HP_info header_info){
	//variables for information regarding blocks
	void *block;
	int blocks_num;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record *rec;
	int no_of_recs;

	//other variables
	int i, j;

	fd = header_info.fileDesc;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){			//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	i = 1;	//start from second block

	//Check all blocks
	while(i != blocks_num){

		//Get data of block i and put it in block
		if(BF_ReadBlock(fd, i, &block) < 0){			//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		//Get how many records block i has
		memcpy(&no_of_recs, block, sizeof(int));

		j = 0;

		//Check all records
		while(j != no_of_recs){

			//Get j record
			rec = block + sizeof(int) + j * sizeof(Record);

			//print it
			printf("id: %d | name: %s | surname: %s | address: %s\n", rec->id, rec->name, rec->surname, rec->address);

			//go to next record
			j++;

		}

		//go to next block
		i++;
	}

	//Return number of blocks we needed in order to print all records
	return blocks_num;
}

int HP_Search(HP_info header_info, void *value, int mode){
	//variables for information regarding blocks
	void *block;
	int blocks_num;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record *rec;
	int no_of_recs;

	//other variables
	int i, j;
	int *val;

	fd = header_info.fileDesc;
	val = value;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){			//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	i = 1;	//start from second block

	//Check all blocks
	while(i != blocks_num){

		//Get data of block i and put it in block
		if(BF_ReadBlock(fd, i, &block) < 0){			//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		//Get how many records block i has
		memcpy(&no_of_recs, block, sizeof(int));

		j = 0;

		//Check all records
		while(j != no_of_recs){

			rec = block + sizeof(int) + j * sizeof(Record);		//Get j record

			//If we find record with id = value
			if(rec->id != -1 && rec->id == *val){
				//if HP_Search was called by HP_GetAllEntries
				//print it
				if(mode == 1){
					printf("id: %d | name: %s | surname: %s | address: %s\n", rec->id, rec->name, rec->surname, rec->address);
				}

				//Return number of blocks needed to find value
				return i;
			}

			//go to next record
			j++;
		}

		//go to next block
		i++;
	}

	//if HT_Search was called by HT_GetAllEntries
	if(mode == 1){
		printf("Record With ID %d not found\n", *val);

		//Return total number of blocks read
		return i;
	}

	return 0;
}

int HP_CreateFile(char *fileName, char attrType, char* attrName, int attrLength){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//other variables
	int size;
	int attrName_size;

	attrName_size = sizeof(attrName) + 1;

	//Create new file
	if(BF_CreateFile(fileName) < 0){					//check if there was an error
		BF_PrintError("Error creating file");
		return -1;
	}

	//Open it to write needed information
	if((fd = BF_OpenFile(fileName)) < 0){				//check if there was an error
		BF_PrintError("Error opening file");
		return -1;
	}

	//Create a new block
	if(BF_AllocateBlock(fd) < 0) {						//check if there was an error
		BF_PrintError("Error allocating block");
		return -1;
	}

	//Get data of first block
	if(BF_ReadBlock(fd, 0, &block) < 0){				//check if there was an error
		BF_PrintError("Error getting block");
		return -1;
	}

	//Write needed information to block
	strcpy(block, "Heap");
	size = sizeof(int);

	memcpy(block, &attrName_size, size);
	memcpy(block + size, "Heap", strlen("Heap") + 1);
	size += sizeof("Heap") + 1;

	memcpy(block + size, &attrType, sizeof(attrType));
	size += sizeof(attrType);

	memcpy(block + size, attrName, attrName_size);
	size += strlen(attrName) + 1;

	memcpy(block + size, &attrLength, sizeof(attrLength));

	//Write block to disc
	if(BF_WriteBlock(fd, 0) < 0){					//check if there was an error
		BF_PrintError("Error writing block back");
		return -1;
	}

	//Close file
	if(BF_CloseFile(fd) < 0) {						//check if there was an error
		BF_PrintError("Error closing file");
		return -1;
	}

	return 0;
}

HP_info* HP_OpenFile( char *fileName){
	//variables for information regarding blocks
	void *block;

	//variables for file descriptor
	int fd;

	//variables for HP_info
	HP_info *info;
	int *type;
	char *name;
	int *length;

	//other variables
	int attrName_size;
	int size;

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

	//Check if it is a heap file
	if(memcmp(block + sizeof(int), "Heap", strlen("Heap") + 1) != 0){
		printf("Not Heap File\n");
		return NULL;
	}

	memcpy(&attrName_size, block, sizeof(int));
	size = sizeof(int) + strlen("Heap") + 1;

	//Create HP_info with all information we need
	info = malloc(sizeof(HP_info));

	info->fileDesc = fd;

	type = block + size;
	info->attrType = *type;
	size += sizeof(int);

	name = block + size;
	info->attrName = name;
	size += attrName_size;

	length = block + size;
	info->attrLength = *length;

	//Return the data structure
	return info;
}

int HP_CloseFile(HP_info* header_info ){

	//Close file
	if(BF_CloseFile(header_info->fileDesc) == 0){	//if there was not an error
		//free data structure
		free(header_info);

		return 0;
	}

	return -1;
}

int HP_InsertEntry(HP_info header_info, Record record){
	//variables for information regarding blocks
	void *block;
	int blocks_num;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record *rec;
	int num_of_records;
	void *val;

	fd = header_info.fileDesc;

	//Get the number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//If we do not have any blocks for records
	if(blocks_num - 1 == 0){

		//Create new block
		if(BF_AllocateBlock(fd) < 0){										//check if there was an error
			BF_PrintError("Error allocating block");
			return -1;
		}

		//Get the number of blocks
		if((blocks_num = BF_GetBlockCounter(fd)) < 0){						//check if there was an error
			BF_PrintError("Error Getting Number Of Blocks");
			return -1;
		}

		//Get data of new block
		if(BF_ReadBlock(fd, blocks_num - 1, &block) < 0){					//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		num_of_records = 1;	//New block only has one record

		//Write information to new block
		memcpy(block, &num_of_records, sizeof(int));
		memcpy(block + sizeof(int), &record, sizeof(record));

		//Write block to disc
		if(BF_WriteBlock(fd, blocks_num - 1) < 0){							//check if there was an error
			BF_PrintError("Error writing block back");
			return -1;
		}
	}
	//If we have blocks with records
	else{

		memcpy(val, &(record.id), sizeof(int));

		//Check if record with the same id already exists
		if(HP_Search(header_info, val, 0) != 0){
			printf("Record with id %d already exists\n", record.id);
			return 0;
		}

		//Get data of last block
		if(BF_ReadBlock(fd, blocks_num - 1, &block) < 0){					//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		//Find how many records last block has
		memcpy(&num_of_records, block, sizeof(int));

		//Find how much available space the current block still has
		int availableSpace = BLOCK_SIZE - sizeof(int) - num_of_records * sizeof(record);

		//If there is still available space
		if(sizeof(record) < availableSpace) {

			//Write new information to block
			memcpy(block + sizeof(int) + num_of_records * sizeof(record), &record, sizeof(record));

			num_of_records++;
			memcpy(block, &num_of_records, sizeof(int));

			//Write block to disc
			if(BF_WriteBlock(fd, blocks_num - 1) < 0){						//check if there was an error
				BF_PrintError("Error writing block back");
				return -1;
			}

		}
		//If there is not available space to last block
		else{

			//Create new block
			if(BF_AllocateBlock(fd) < 0) {									//check if there was an error
				BF_PrintError("Error allocating block");
				return -1;
			}

			//Get the number of blocks
			if((blocks_num = BF_GetBlockCounter(fd)) < 0){					//check if there was an error
				BF_PrintError("Error Getting Number Of Blocks");
				return -1;
			}

			//Get data of new block
			if(BF_ReadBlock(fd, blocks_num - 1, &block) < 0){				//check if there was an error
				BF_PrintError("Error getting block");
				return -1;
			}

			num_of_records = 1;	//new block has one record

			//Write data to new block
			memcpy(block, &num_of_records, sizeof(int));
			memcpy(block + sizeof(int), &record, sizeof(record));

			//Write data to disc
			if(BF_WriteBlock(fd, blocks_num-1) < 0){						//check if there was an error
				BF_PrintError("Error writing block back");
				return -1;
			}
		}
	}

	//Return the block's number new record was inserted to
	return blocks_num;
}

int HP_DeleteEntry(HP_info header_info, void *value){
	//variables for information regarding blocks
	void *block;
	void *lastblock;
	int blocks_num;

	//variables for file descriptor
	int fd;

	//variables regarding records
	Record *cur_rec;
	Record *last_rec;
	int no_of_recs;
	int last_rec_num;

	//other variables
	int i, j;
	int *val;

	fd = header_info.fileDesc;
	val = value;
	
	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){					//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//Check if we have any records
	if(blocks_num == 1){
		printf("No records\n");
		return 0;
	}

	i = 1;	//start from second block

	//Check all blocks
	while(i != blocks_num){

		//Get data of block i and put it in block
		if(BF_ReadBlock(fd, i, &block) < 0){						//check if there was an error
			BF_PrintError("Error getting block");
			return -1;
		}

		//Get how many records block i has
		memcpy(&no_of_recs, block, sizeof(int));

		j = 0;

		//Check all records
		while(j != no_of_recs){

			cur_rec = block + sizeof(int) + j * sizeof(Record); 	//Get j record

			//If we find record with id = value
			if(cur_rec->id == *val){

				//Get the number of blocks
				if((blocks_num = BF_GetBlockCounter(fd)) < 0){							//check if there was an error
					BF_PrintError("Error Getting Number Of Blocks");
					return -1;
				}

				//Get data of the last block and put it in lastblock
				if(BF_ReadBlock(fd, blocks_num - 1, &lastblock) < 0){					//check if there was an error
					BF_PrintError("Error getting block");
					return -1;
				}

				//Get how many records the last block has
				memcpy(&last_rec_num, lastblock, sizeof(int));

				//Get last record of file
				last_rec = lastblock + sizeof(int) + (last_rec_num - 1) * sizeof(Record);

				//Replace current record with the last record
				cur_rec->id = last_rec->id;
				strcpy(cur_rec->name, last_rec->name);
				strcpy(cur_rec->surname, last_rec->surname);
				strcpy(cur_rec->address, last_rec->address);

				//Last block has one less record
				last_rec_num--;

				memcpy(lastblock, &last_rec_num, sizeof(int));

				//Write new information of the last block to disc
				if(BF_WriteBlock(fd, blocks_num - 1) < 0){								//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				memcpy(block + sizeof(int) + j * sizeof(Record), cur_rec, sizeof(Record));

				//Write new information of current block to disc
				if(BF_WriteBlock(fd, i) < 0){											//check if there was an error
					BF_PrintError("Error writing block back");
					return -1;
				}

				printf("Deleted\n");

				//We do not need to check the other records
				return 0;
			}

			//go to next record
			j++;
		}

		//go to next block
		i++;
	}

	//There was not a record with id = value
	printf("ID %d not found\n", *val);

	return 0;
}

int HP_GetAllEntries(HP_info header_info, void *value){
	//variables for information regarding blocks
	int blocks_num;

	//variables for file descriptor
	int fd;

	fd = header_info.fileDesc;

	//Get number of blocks
	if((blocks_num = BF_GetBlockCounter(fd)) < 0){					//check if there was an error
		BF_PrintError("Error Getting Number Of Blocks");
		return -1;
	}

	//Check if we have any records
	if(blocks_num == 1){
		printf("No records\n");
		return 0;
	}

	//If value = NULL
	//Print all records
	if(value == NULL){
		return HP_PrintAll(header_info);
	}
	//else find record with id = value
	else{
		return HP_Search(header_info, value, 1);
	}

}
