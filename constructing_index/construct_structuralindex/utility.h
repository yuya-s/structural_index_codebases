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
//#include <unordered_map>
#include <map>
#include <set>
#include <list>
#include <sys/resource.h>
#include <google/dense_hash_map>

using namespace std;

class Result {
public:

    string outputFile;
    string inputFile;

    int k;
    double indexTime;
    double bisimulationTime;
    double maxRSS;

    void Output() {

        std::ofstream fout(outputFile, ios::app);

        fout << inputFile <<"\t" << k  << "\t" << indexTime<< "\t"<<bisimulationTime <<"\t"<<indexTime+bisimulationTime<<"\t"<<maxRSS<< std::endl;

    }

};

template<class T> inline int hash_set(set<T>& sett){

    int hashvalue=17;
    for (auto ittA = sett.begin(); ittA != sett.end(); ittA++) {
        hashvalue +=*ittA;
        hashvalue *=31;
    }

    return hashvalue;
}

template<class T> inline int hash_set(vector<T>& sett){

    int hashvalue=17;
    for (auto ittA = sett.begin(); ittA != sett.end(); ittA++) {
        hashvalue +=*ittA;
        hashvalue *=31;
    }

    return hashvalue;
}

template<class T> inline int hash_set(set<pair<T,T>>& setp){

    int hashvalue=17;
    for (auto ittA = setp.begin(); ittA != setp.end(); ittA++) {
        hashvalue += (*ittA).first;
        hashvalue *= 31;
        hashvalue += (*ittA).second;
        hashvalue *= 31;
    }

    return hashvalue;
}



#endif //K_BISIMULATION_UTILITY_H
