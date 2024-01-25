#include "catalog.h"
#include "query.h"


// forward declaration
const Status ScanSelect(const string & result, 
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen);

/*
 * Selects records from the specified relation.
 *
 * Returns:
 * 	OK on success
 * 	an error code otherwise
 */

const Status QU_Select(const string & result, 
		       const int projCnt, 
		       const attrInfo projNames[],
		       const attrInfo *attr, 
		       const Operator op, 
		       const char *attrValue)
{
	cout << "Doing QU_Select " << endl;
	Status status;
	int reclen;
	AttrDesc *projNamesDesc;
	AttrDesc attrDesc;
	const char *filter;
	Operator opTwo;

	reclen = 0;
	projNamesDesc = new AttrDesc[projCnt];
	for (int i = 0; i < projCnt; i++){
		status = attrCat->getInfo(projNames[i].relName, projNames[i].attrName, projNamesDesc[i]);
		reclen += projNamesDesc[i].attrLen;
	}
	

	if (attr != NULL){
		status = attrCat->getInfo(attr->relName, attr->attrName, attrDesc);
		if (status != OK) return status;

		int tempInt;
		float tempFloat;
		if (attr->attrType == INTEGER){
			tempInt = atoi(attrValue);
			filter = (char*)&tempInt;
		}
		else if (attr->attrType == FLOAT){
			tempFloat = atof(attrValue);
			filter = (char*)&tempFloat;
		}
		else if (attr->attrType == STRING){
			filter = attrValue;
		}
		opTwo = op;
	}
	else{
		strcpy(attrDesc.relName, projNames[0].relName);
		strcpy(attrDesc.attrName, projNames[0].attrName);
		attrDesc.attrLen = 0;
		attrDesc.attrType = STRING;
		attrDesc.attrOffset = 0;
		filter = NULL;
		opTwo = EQ;
	}
	status = ScanSelect(result, projCnt, projNamesDesc, &attrDesc, opTwo, filter, reclen);
	return status;
}


const Status ScanSelect(const string & result, 
#include "stdio.h"
#include "stdlib.h"
			const int projCnt, 
			const AttrDesc projNames[],
			const AttrDesc *attrDesc, 
			const Operator op, 
			const char *filter,
			const int reclen)
{
    cout << "Doing HeapFileScan Selection using ScanSelect()" << endl;
  	Status status;
	InsertFileScan finalRec(result, status);
    if (status != OK)  return status; 
    char outputData[reclen];

    Record outputRec;
    outputRec.data = (void *) outputData;
    outputRec.length = reclen;

	HeapFileScan hfs(attrDesc->relName,status);
    if (status != OK)  return status;

    status = hfs.startScan(attrDesc->attrOffset, attrDesc->attrLen, (Datatype) attrDesc->attrType, filter, op);
    if (status != OK)  return status; 

    RID relRID;
    Record relRec;
	while (hfs.scanNext(relRID) == OK) {
        status = hfs.getRecord(relRec);
        ASSERT(status == OK);
        int outputOffset = 0;
        for (int i = 0; i < projCnt; i++) {
            memcpy(outputData + outputOffset, (char *)relRec.data + projNames[i].attrOffset, projNames[i].attrLen);
            outputOffset += projNames[i].attrLen;
        } 
        RID outRID;
        status = finalRec.insertRecord(outputRec, outRID);
        ASSERT(status == OK);
    }
    return OK;
}
