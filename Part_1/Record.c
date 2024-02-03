#include <string.h>
#include "Record.h"

//Creates new record with the given values
Record CreateRecord(int id, char *name, char *surname, char *address){
	Record record;

	record.id = id;
	strcpy(record.name, name);
	strcpy(record.surname, surname);
	strcpy(record.address, address);

	return record;
}

