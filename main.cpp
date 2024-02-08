/*
Skeleton code for linear hash indexing
*/

#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    Record employeeInfo;
    int employeeID = 0;
    int result=0;

    // Create the index
    LinearHashIndex emp_index("EmployeeIndex");
    emp_index.createFromFile("Employee.csv");

    // Loop to lookup IDs until user is ready to quit
    // while(true){
        
    //     cout<<"Insert employee ID: ";
    //     cin>>employeeID;
    //     cout<<endl;
    
    //     employeeInfo = emp_index.findRecordById(employeeID);

    //     if(employeeInfo.id==-1){
    //         cout<<"No record info."<<endl;
    //     } else{
    //         cout<<"ID: "<<employeeInfo.id
    //         <<", Name: "<<employeeInfo.name
    //         <<", Bio: "<< employeeInfo.bio
    //         << ", manager_id: "<<employeeInfo.manager_id<<endl;
    //     }
    //     cout<<"--------------------------------------";
    //     cout<<"--------------------------------------";
    //     cout<<endl;

    // }

    return 0;
}
