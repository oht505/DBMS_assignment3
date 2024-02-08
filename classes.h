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
    
    int hashTable(int id){
        
    }
};


class LinearHashIndex {

private:
    static const int BLOCK_SIZE = 4096;

    vector<int> blockDirectory; // Map the least-significant-bits of h(id) to a bucket location in EmployeeIndex (e.g., the jth bucket)
                                // can scan to correct bucket using j*BLOCK_SIZE as offset (using seek function)
								// can initialize to a size of 256 (assume that we will never have more than 256 regular (i.e., non-overflow) buckets)
    int n;  // The number of indexes in blockDirectory currently being used
    int i;	// The number of least-significant-bits of h(id) to check. Will need to increase i once n > 2^i
    int numRecords;    // Records currently in index. Used to test whether to increase n
    int nextFreeBlock; // Next place to write a bucket. Should increment it by BLOCK_SIZE whenever a bucket is written to EmployeeIndex
    string fName;      // Name of index file
    int pageNumber;    // To handle the number of page in main memory(there are three pages in mm)
    char page[3][BLOCK_SIZE]={};    // Define 3 * page(4KB) for re-hashing

    // Insert new record into index
    void insertRecord(Record record) {

        // No records written to index yet
        if (numRecords == 0) {
            // Initialize index with first blocks (start with 4)
            blockDirectory.resize(n);
        }

        // Add record to the index in the correct block, creating a overflow block if necessary
        //      * Decide that which bucket the records should be inserted
        
        //      * Convert record data into binary
        //      * Insert record into a page 

        // Take neccessary steps if capacity is reached:
		// increase n; increase i (if necessary); place records in the new bucket that may have been originally misplaced due to a bit flip
        
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
        blockDirectory.resize(n);
 
        // make sure to account for the created buckets by incrementing nextFreeBlock appropriately
        
        
        dataFile.close();
    }

    // Read csv file and add records to the index
    void createFromFile(string csvFName) {
        // Read the csvFName file
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
            
            insertRecord(record);
        }
        // If a page containing records is not full,
        // write the page in the dataFile.
        //dataFile.write(page[pageNumber], sizeof(page));
        
        inputFile.close();
        dataFile.close();
    }

    // Given an ID, find the relevant record and print it
    Record findRecordById(int id) {

        Record result;

        // Open input file
        ifstream inputFile(fName, ios::in|ios::binary);
        if(!inputFile.is_open()){
            throw "Error inputFile" + fName;
        }

        // Use blockDirectory to find a page from the file
        //   * 


        // Find a record from the page
        //   * Read directory(offset, length)
        //   * Use the directory pointer to locate record fields pointers
        //   * Use record fields pointer to locate attributes of the record

        // if: employee_ID is equal to input ID
        //   * Use vector<string> to 

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
};
