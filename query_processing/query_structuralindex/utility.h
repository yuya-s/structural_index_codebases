//
// Created by sasaki on 19/04/18.
//

#ifndef QUERY_BISIMULATION_UTILITY_H
#define QUERY_BISIMULATION_UTILITY_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <stack>
#include <list>
#include <random>
#include <algorithm>
#include <map>
#include <set>
#include <list>
#include <google/dense_hash_map>

using namespace std;

class Result {
public:

    string outputFile;
    string inputFile;

    int k;
    int querynum;
    double querySumTime;
    double maintenanceAddEdgeTime;
    double maintenanceDeleteEdgeTime;
    double maintenanceAddWorkloadTime;
    double maintenanceDeleteWorkloadTime;
    int maintenanceEdgeNum;
    int maintenanceWorkloadNum;

    void Crear(){
        k=0;
        querynum=0;
        querySumTime=0;
    }
    void Output() {

        std::ofstream fout(outputFile, ios::app);

        fout << inputFile << "\t" << k  << "\t" << querySumTime/(double)querynum<< std::endl;

    }

    void OutputMaintenance(){
        std::ofstream fout(outputFile+"_maintenance", ios::app);
        fout << inputFile << "\t" << k  << "\t"<<maintenanceEdgeNum<<"\t"<<maintenanceWorkloadNum<<"\t" << maintenanceAddEdgeTime/(double)maintenanceEdgeNum << "\t" << maintenanceDeleteEdgeTime/(double)maintenanceEdgeNum<< "\t" << maintenanceAddWorkloadTime/(double)maintenanceWorkloadNum << "\t" << maintenanceDeleteWorkloadTime/(double)maintenanceWorkloadNum<<std::endl;
    }

};
#endif //QUERY_BISIMULATION_UTILITY_H
