#include "catalog.h"
#include "query.h"


/*
 * Deletes records from a specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Delete(const string & relation, 
		       const string & attrName, 
		       const Operator op,
		       const Datatype type, 
		       const char *attrValue)
{

HeapFileScan *hfs;
Status status;
Status deleteStatus = OK;
AttrDesc attr;
RID rid;

attrCat->getInfo(relation, attrName, attr);
hfs = new HeapFileScan(relation, status);
if (status != OK) return status;

if (attrName.empty()) {
	if ((status = hfs->startScan(0, 0, STRING, NULL, EQ)) != OK) {
		delete hfs;
		return status;
	}
} else {
	switch (type) {
		case INTEGER:
			{
			int temp = atoi(attrValue);
			char buf[sizeof(int)];
			memcpy(buf, &temp, sizeof(int));
			status = hfs->startScan(attr.attrOffset, attr.attrLen, type, buf, op);
			break;
			}
		case FLOAT:
			{
			float temp = atof(attrValue);
			char buf[sizeof(float)];
			memcpy(buf, &temp, sizeof(float));
			status = hfs->startScan(attr.attrOffset, attr.attrLen, type, buf, op);
			break;
			}
		case STRING:
			status = hfs->startScan(attr.attrOffset, attr.attrLen, type, attrValue, op);
			break;
	}
	if ((status) != OK) {
		delete hfs;
		return status;
	}
}

while (status == OK) {
	status = hfs->scanNext(rid);
	//error.print(status);
	if (status == OK) deleteStatus = hfs->deleteRecord();
	if (deleteStatus != OK) return deleteStatus;
}

status = hfs->endScan();
delete hfs;

return status;
}


