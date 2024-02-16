#include <string>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <bitset>
using namespace std;

class Record {
public:
    int id, manager_id;
    std::string bio, name;

    Record() :id(-1) {}

    Record(vector<std::string> fields) {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
    
};


class LinearHashIndex {

private:
    static const int BLOCK_SIZE = 4096;
    static const int MAX_BLOCK_IN_MEMORY = 3;
    const int POINTER_ADDRESS_SIZE = sizeof(int);
    const int RECORD_LENGTH_SIZE = sizeof(int);
    const int SLOT_RECORD_NUMBER_SIZE = sizeof(int); 
    const int SLOT_OVERFLOW_SIZE = sizeof(int);
    // Size of 2 pointers for (offset, length) of a record in the slot directory
    const int SLOT_DIRECTORY_SIZE = POINTER_ADDRESS_SIZE * 2;
    // Size of offset address for 4 fields + 1 offset end address of a record
    const int FIELD_OFFSET_POINTER_SIZE = POINTER_ADDRESS_SIZE * 5; 
    // Size of integer fields
    const int ID_FIELD_SIZE = 8;
    const int MAX_RECORD_SIZE = ID_FIELD_SIZE * 2 + 500 + 200;
    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    string fName;      // Name of index file
    int totalNumberOfPage;    // To count the total number of pages 
    int totalNumRecords;  // To count the total number of records
    char mmpages[MAX_BLOCK_IN_MEMORY][BLOCK_SIZE]={};    // Define 3 * page(4KB) for re-hashing in main memory 
    int pageRemaining = BLOCK_SIZE-2*POINTER_ADDRESS_SIZE-SLOT_RECORD_NUMBER_SIZE;
    int addressOfOverflowPage = -1;   // -1: no overflow page, >0: address of overflow page  
    int freeSpacePointer = 0;
    int addressOfCurrentField = FIELD_OFFSET_POINTER_SIZE;      // The address of the current field relative to the variable length record
    int addressOfCurrentOffsetField = 0;    // The address of the current offset field relative to the variable length record
    int latestRecordOffset = BLOCK_SIZE-2*POINTER_ADDRESS_SIZE-SLOT_RECORD_NUMBER_SIZE;
    char page[BLOCK_SIZE]={};

    // Insert new record into index
    void insertRecord(Record record, ofstream *dataFile) {

        // Read the Employee Index file 
        ifstream readFile("EmployeeIndex", ios::in|ios::binary);
        if(!readFile.is_open()){
            throw "Error Opening";
        }
        readFile.seekg(blockDirectory[bitHash(record.id)]);
        readFile.read(page,BLOCK_SIZE);

        // Store numRecords 
        memcpy(&numRecords, &page[0]+BLOCK_SIZE-2*POINTER_ADDRESS_SIZE, POINTER_ADDRESS_SIZE);
        //cout<<"offset:"<<blockDirectory[bitHash(record.id)]<<", numRecords:"<<numRecords<<endl;
        memcpy(&freeSpacePointer, &page[0]+BLOCK_SIZE-POINTER_ADDRESS_SIZE, POINTER_ADDRESS_SIZE);
        //cout<<"total_records: "<<totalNumRecords<<endl;

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 4)  
            
        }

        // Size of string (Name, Bio)
        int nameLength = record.name.size() + 1;
        int bioLength = record.bio.size() + 1;

        // Size of a variable record(its length and its pointer)
        int recordLength = FIELD_OFFSET_POINTER_SIZE + (ID_FIELD_SIZE*2) + nameLength + bioLength;
        int recordRealSize = SLOT_DIRECTORY_SIZE + recordLength;

        // Add record to the index in the correct block, creating a overflow block if necessary

        // create a variable record set to store info
        char variableRecord[recordLength];
        
        // The address of the current field relative to the variable length record
        int addressOfCurrentField = FIELD_OFFSET_POINTER_SIZE;
        // The address of the current offset field relative to the variable length record
        int addressOfCurrentOffsetField = 0;

        // Write field offset pointer and field
        // record.id
        memcpy(variableRecord + addressOfCurrentField, &record.id, sizeof(int));
        addressOfCurrentField += freeSpacePointer;
        memcpy(variableRecord + addressOfCurrentOffsetField, &addressOfCurrentField, POINTER_ADDRESS_SIZE);
        addressOfCurrentField -= freeSpacePointer;
        addressOfCurrentField += ID_FIELD_SIZE;
        addressOfCurrentOffsetField += POINTER_ADDRESS_SIZE;

        // record.name
        memcpy(variableRecord + addressOfCurrentField, &record.name[0], nameLength);
        addressOfCurrentField += freeSpacePointer;
        memcpy(variableRecord + addressOfCurrentOffsetField, &addressOfCurrentField, POINTER_ADDRESS_SIZE);
        addressOfCurrentField -= freeSpacePointer;
        addressOfCurrentField += nameLength;
        addressOfCurrentOffsetField += POINTER_ADDRESS_SIZE;

        // record.bio
        memcpy(variableRecord + addressOfCurrentField, &record.bio[0], bioLength);
        addressOfCurrentField += freeSpacePointer;
        memcpy(variableRecord + addressOfCurrentOffsetField, &addressOfCurrentField, POINTER_ADDRESS_SIZE);
        addressOfCurrentField -= freeSpacePointer;
        addressOfCurrentField += bioLength;
        addressOfCurrentOffsetField += POINTER_ADDRESS_SIZE;

        // record.manager_id
        memcpy(variableRecord + addressOfCurrentField, &record.manager_id, sizeof(int));
        addressOfCurrentField += freeSpacePointer;
        memcpy(variableRecord + addressOfCurrentOffsetField, &addressOfCurrentField, POINTER_ADDRESS_SIZE);
        addressOfCurrentField -= freeSpacePointer;
        addressOfCurrentField += ID_FIELD_SIZE;
        addressOfCurrentOffsetField += POINTER_ADDRESS_SIZE;

        // Offset of end of record
        addressOfCurrentField += freeSpacePointer;
        memcpy(variableRecord + addressOfCurrentOffsetField, &addressOfCurrentField, POINTER_ADDRESS_SIZE);
        addressOfCurrentField -= freeSpacePointer;

        // Copy variable length record to the memory page
        memcpy(page + freeSpacePointer, &variableRecord, recordLength);
        int recordAddress = freeSpacePointer;
        freeSpacePointer += recordLength;
        //cout<<"offset:"<<blockDirectory[bitHash(record.id)]<<", recordAddress:"<<recordAddress<<endl;

        // Add (offset, recordLength) to the slot directory in the page 
        memcpy(page + BLOCK_SIZE - POINTER_ADDRESS_SIZE - SLOT_RECORD_NUMBER_SIZE - SLOT_OVERFLOW_SIZE - ((numRecords+1)*8), &recordAddress, POINTER_ADDRESS_SIZE);
        memcpy(page + BLOCK_SIZE - POINTER_ADDRESS_SIZE - SLOT_RECORD_NUMBER_SIZE - SLOT_OVERFLOW_SIZE - ((numRecords+1)*8 - 4), &recordLength, RECORD_LENGTH_SIZE);
        
        // Update free space pointer and the number of variable length records in the memory page
        numRecords += 1;
        totalNumRecords += 1;
        memcpy(page + BLOCK_SIZE - POINTER_ADDRESS_SIZE, &freeSpacePointer, POINTER_ADDRESS_SIZE);
        memcpy(page + BLOCK_SIZE - POINTER_ADDRESS_SIZE - SLOT_RECORD_NUMBER_SIZE, &numRecords, SLOT_RECORD_NUMBER_SIZE);
        memcpy(page + BLOCK_SIZE - 2 * POINTER_ADDRESS_SIZE - SLOT_RECORD_NUMBER_SIZE, &addressOfOverflowPage, POINTER_ADDRESS_SIZE);

        (*dataFile).seekp(blockDirectory[bitHash(record.id)]);
        (*dataFile).write(page, sizeof(page));
         
        // Take neccessary steps if capacity is reached:
		// increase n; increase i (if necessary); place records in the new bucket that may have been originally misplaced due to a bit flip
    
        readFile.close();
    }

public:
    LinearHashIndex(string indexFileName) {
        n = 4; // Start with 4 buckets in index
        i = 2; // Need 2 bits to address 4 buckets
        numRecords = 0;
        nextFreeBlock = 0;
        fName = indexFileName;

        // Create your EmployeeIndex file 
        ofstream dataFile(fName, ios::out | ios::binary);
        if(!dataFile.is_open()){
            throw "Error creating" + fName;
        }        
        
        // write out the initial 4 buckets
        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately
        char slotDirectory[3*POINTER_ADDRESS_SIZE];
        memcpy(slotDirectory,&addressOfOverflowPage,POINTER_ADDRESS_SIZE);
        memcpy(slotDirectory+POINTER_ADDRESS_SIZE, &numRecords, SLOT_RECORD_NUMBER_SIZE);
        memcpy(slotDirectory+POINTER_ADDRESS_SIZE+SLOT_RECORD_NUMBER_SIZE, &freeSpacePointer, POINTER_ADDRESS_SIZE);
        memcpy(&page[0]+BLOCK_SIZE-3*POINTER_ADDRESS_SIZE, &slotDirectory, sizeof(slotDirectory));

        for (int nextFreeBlock=0; nextFreeBlock<n; nextFreeBlock++){
            blockDirectory.push_back(nextFreeBlock*BLOCK_SIZE);    
            dataFile.write(page, sizeof(page));       
        }

        // To check data safety
        // blockDirectory.resize(8);
        // printVector(blockDirectory);

        dataFile.close();
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        // Read the csvFName file
        // ifstream inputFile(csvFName);
        ifstream inputFile(csvFName);

        if(!inputFile.is_open()){
            throw "Error inputFile" + csvFName;
        }

        ofstream dataFile(fName, ios::out | ios::binary);
        if(!dataFile.is_open()){
            throw "Error dataFile" + fName;
        }
        
        string line;
        while(getline(inputFile, line)){
            vector<string> fields;
            stringstream ss(line);
            string field;
            while(getline(ss, field, ',')){
                fields.push_back(field);
            }
            Record record(fields);

            insertRecord(record, &dataFile);
        }

        // Write blockDirectory to the file
        
        

        inputFile.close();
        dataFile.close();
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {

        Record result;
        int idBits = 0;

        // Open input file
        ifstream inputFile(fName, ios::in|ios::binary);
        if(!inputFile.is_open()){
            throw "Error inputFile" + fName;
        }

        // Use id with bit hashTable to find which blockDirectory
        idBits = bitHash(id);


        // Use blockDirectory to find a page from the file
        //   * idBits can be an index of blockDirectory
        //   * => blockDirectory[idBits]=Pointer to page
        //   * => write directly to the file. 
        //   
        
        

        // Find a record from the page
        //   * Read directory(offset, length)
        //   * Use the directory pointer to locate record fields pointers
        //   * Use record fields pointer to locate attributes of the record

        // if: employee_ID is equal to input ID
        //   *  

        // else: Find other records

        // result.id=11442121;
        // result.name="TesterASDF";
        // result.bio="Idon'tknow~";
        // result.manager_id=11433333;

        return result;
    }

    int bitHash(int id){
        bitset<8> decimalBitset(id % 216);
        unsigned int lsbBits = (decimalBitset.to_ulong()) & ((1<<i)-1);
        return lsbBits;
    }

    void printVector(vector<int> const &a){
        for (int i=0; i<a.size();i++){
            cout<<a.at(i)<<' ';
        }
    }
};
