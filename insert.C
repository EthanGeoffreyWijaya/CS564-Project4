#include "catalog.h"
#include "query.h"

/*
 * Inserts a record into the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Insert(const string & relation, 
	const int attrCnt, 
	const attrInfo attrList[])
{

int attrCount;
int recLen = 0;
AttrDesc *attrs;
attrCat->getRelInfo(relation, attrCount, attrs);

if (attrCnt != attrCount) return ATTRNOTFOUND;

for (int i = 0; i < attrCount; i++) {
	recLen += attrs[i].attrLen;
}

Record rec;
char data[recLen];
rec.data = (void*)data;
rec.length = recLen;

for (int i = 0; i < attrCount; i++) {
	int j;
	for (j = 0; j < attrCount; j++) {
		if (strcmp(attrList[i].attrName, attrs[j].attrName) == 0 
		&& strcmp(attrList[i].relName, attrs[j].relName) == 0 
		&& attrList[i].attrType == attrs[j].attrType) {
			if (attrList[i].attrValue == NULL) return ATTRNOTFOUND;

			int temp;
			switch (attrs[j].attrType) {
			case INTEGER:
				temp = atoi((char*)attrList[i].attrValue);
				memcpy(data + attrs[j].attrOffset, &temp, attrs[j].attrLen);
				break;
			case FLOAT:
				temp = atof((char*)attrList[i].attrValue);
				memcpy(data + attrs[j].attrOffset, &temp, attrs[j].attrLen);
				break;
			case STRING:
				memcpy(data + attrs[j].attrOffset, attrList[i].attrValue, attrs[j].attrLen);
				break;
			}
			break;
		}
	}
	if (j >= attrCount) return ATTRNOTFOUND;
}

InsertFileScan *ifs;
RID rid;
Status status;

ifs = new InsertFileScan(relation, status);
if (status != OK) return status;

status = ifs->insertRecord(rec, rid);
delete ifs;
return status;
}

