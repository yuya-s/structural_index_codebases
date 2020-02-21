//
// Created by sasaki on 19/04/16.
//

#ifndef QUERY_PATHINDEX_UTILITY_H
#define QUERY_PATHINDEX_UTILITY_H

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
//#include <unordered_map>
#include <map>
#include <set>
#include <list>
//#include <unordered_set>
#include <google/dense_hash_map>

using namespace std;

class Result {
public:

    string outputFile;
    string inputFile;

    int k;
    int querynum;
    double querySumTime;

    void Crear(){
        k=0;
        querynum=0;
        querySumTime=0;
    }
    void Output() {

        std::ofstream fout(outputFile, ios::app);

        fout << inputFile << "\t" << k  << "\t" << querySumTime/(double)querynum<< std::endl;

    }

};
#endif //QUERY_PATHINDEX_UTILITY_H
