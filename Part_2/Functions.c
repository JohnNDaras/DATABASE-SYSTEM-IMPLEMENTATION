#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "BF.h"
#include "Types.h"
#include "HT.h"

SecondaryRecord *FindBlock(HT_info header_info, Record record){
	//variables for information regarding blocks
	void *block;
	void *indexBlock;
	int blocks_num;
	int next;

	//variables for file descriptor
	int fd;

	//variables for secondary record
	SecondaryRecord *srec;

	//variables regarding records
	Record rec;
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
		return NULL;
	}

	//Get array of hash table
	hash_table = malloc(buckets * sizeof(Index));
	memcpy(hash_table, indexBlock, buckets * sizeof(Index));

	index = hash(record.id, buckets);

	//Get first block of index the hash function returned
	next = hash_table[index].first;

	//While index has blocks
	while(next){
		//Find data of block with number = next
		if(BF_ReadBlock(fd, next, &block) < 0){			//check if there was an error
			BF_PrintError("Error getting block");
			return NULL;
		}

		blocks_read++;

		//Get how many records current block has
		memcpy(&num_of_records, block, sizeof(int));

		//Check all records
		for(i = 0; i < num_of_records; i++){
			//Get i record
			memcpy(&rec, block + sizeof(int) + i * sizeof(Record), sizeof(Record));

			//If we find record with id = value
			if(rec.id == record.id){

				srec = malloc(sizeof(SecondaryRecord));

				srec->record.id = record.id;
				strcpy(srec->record.name, record.name);
				strcpy(srec->record.surname, record.surname);
				strcpy(srec->record.address, record.address);

				srec->blockId = next;

				//Return number of blocks needed to find value
				return srec;
			}
		}

		//Go to next block of current index
		memcpy(&next, block + BLOCK_SIZE - sizeof(int), sizeof(int));
	}

	free(hash_table);

	return NULL;
}




