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

        //string output =  "average.txt";
        std::ofstream fout(outputFile, ios::app);

        fout << inputFile <<"\t" << k  << "\t" << indexTime<< "\t"<<pathenumerationTime <<"\t"<<indexTime+pathenumerationTime<<"\t"<<maxRSS<< std::endl;

    }

};



template<class T> set<T> Product(set<T>& rsettA, set<T>&rsettB) {
    set<T> settProduct;
    for (auto itt = rsettB.begin(); itt != rsettB.end(); itt++) {
        if (rsettA.find(*itt) != rsettA.end()) {
            settProduct.insert(*itt);
        }
    }
    return settProduct;
}
template<class T> void inline Union(set<T>& rsettA, set<T>&rsettB) {

    for (auto itt = rsettB.begin(); itt != rsettB.end(); itt++) {
        rsettA.insert(*itt);
    }

}


template<class T> inline void SetEnumerate(set<T>& rsettOriginal, set<T>& rsetPathl, set<T>&rsetPath0, int l) {

    for (auto ittA = rsetPathl.begin(); ittA != rsetPathl.end(); ittA++) {
        for (auto ittB = rsetPath0.begin(); ittB != rsetPath0.end(); ittB++) {
            int hashvalue;
            hashvalue = (1 +*ittA)*l + *ittB;

            //vector<int> templist=hash2pathlabel[*ittA];
            //templist.push_back(*ittB);
            //hash2pathlabel[hashvalue]=templist;
            rsettOriginal.insert(hashvalue);
        }
    }

}

template<class T> inline void SetEnumerate(vector<T>& rsettOriginal, vector<T>& rsetPathl, vector<T>&rsetPath0, int l, google::dense_hash_map< int, vector<int> >& hash2pathlabel) {

    for (auto ittA = rsetPathl.begin(); ittA != rsetPathl.end(); ittA++) {
        for (auto ittB = rsetPath0.begin(); ittB != rsetPath0.end(); ittB++) {
            int hashvalue;
            if(l == 1) {hashvalue = ((17 +*ittA)*31 + *ittB)*31;}
            else hashvalue = (*ittA + *ittB)*31;

            vector<int> templist=hash2pathlabel[*ittA];
            templist.push_back(*ittB);
            hash2pathlabel[hashvalue]=templist;
            rsettOriginal.push_back(hashvalue);
        }
    }

}

template<class T> inline void SetEnumerate(set<T>& rsettOriginal, set<T>& rsetPathl, set<T>&rsetPath0, int l, map< int, vector<int> >& hash2pathlabel) {
    set<T> settEnumerate(rsettOriginal);
    for (auto ittA = rsetPathl.begin(); ittA != rsetPathl.end(); ittA++) {
        for (auto ittB = rsetPath0.begin(); ittB != rsetPath0.end(); ittB++) {
            int hashvalue;
            if(l == 1) {hashvalue = ((17 +*ittA)*31 + *ittB)*31;}
            else hashvalue = (*ittA + *ittB)*31;

            vector<int> templist=hash2pathlabel[*ittA];
            templist.push_back(*ittB);
            hash2pathlabel[hashvalue]=templist;
            rsettOriginal.insert(hashvalue);
        }
    }

}

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
