//
// Created by sasaki on 18/12/05.
//

#ifndef K_BISIMULATION_UTILITY_H
#define K_BISIMULATION_UTILITY_H


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
#include <unordered_set>
#include <sys/resource.h>
#include <google/dense_hash_map>

using namespace std;

class Result {
public:

    string outputFile;
    string inputFile;

    int k;
    double indexTime;
    double pathenumerationTime;
    double maxRSS;

    void Output() {

        std::ofstream fout(outputFile, ios::app);

        fout << inputFile << "\t" << k  << "\t" << indexTime<< "\t"<<pathenumerationTime <<"\t"<<indexTime+pathenumerationTime<<"\t"<<maxRSS<< std::endl;

    }

};




#endif //K_BISIMULATION_UTILITY_H
