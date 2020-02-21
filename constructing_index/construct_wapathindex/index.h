//
// Created by sasaki on 19/03/11.
//

#ifndef K_BISIMULATION_INDEX_INDEX_H_H
#define K_BISIMULATION_INDEX_INDEX_H_H

#include "utility.h"
#include "pathset.h"



struct vectoreqr {
	inline bool operator()(const vector<int>& s1, const vector<int>& s2) const {
		for(int i=0;i<s1.size();i++) {
            if (s1[i] != s2[i])return false;
        }
		return true;
	}
};


struct vectorhash {
	inline size_t operator()(const vector<int>& s1) const {
        int hashvalue=17;
        for(int i=0;i<s1.size();i++) {
            hashvalue += s1[i];
            hashvalue *= 31;
        }
		return hashvalue;
	}
};




class PathIndex{

public:
    int k;
    int labelnum; //original label num * 2;
    //vector<int> numberofsegid;
//    google::dense_hash_map< std::vector<int>, vector< Path >,vectorhash,vectoreqr > history2path;
    //vector< vector<Path>> history2path;
    vector< vector< pair<int,int> >> label2path;
    PathIndex(PathSet&);
    void Output(string output_filename);
};



#endif //K_BISIMULATION_INDEX_INDEX_H_H
