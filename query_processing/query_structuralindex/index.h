//
// Created by sasaki on 19/04/18.
//

#ifndef QUERY_BISIMULATION_INDEX_H
#define QUERY_BISIMULATION_INDEX_H

#include "utility.h"
#include "graph.h"

struct Path {// for google hatsh map
	int src;
	int dst;
	bool operator==(const Path& p){return (src == p.src&&dst == p.dst);}
	bool operator!=(const Path& p){return (src != p.src||dst != p.dst);}

};
struct PathwithIDs {
public:
    Path path;

    set<int> labelsequence;set<int> segvalue;
    bool bisimilar;
    bool operator==(const PathwithIDs& s){return (path.src == s.path.src&&path.dst==s.path.dst&&segvalue == s.segvalue&&labelsequence == s.labelsequence);}
    bool operator==(const PathwithIDs* s){return (path.src == s->path.src&&path.dst==s->path.dst&&segvalue == s->segvalue&&labelsequence == s->labelsequence);}
    void clear(){
    	labelsequence.clear();
        segvalue.clear();
    }
};

struct PathwithHistory {
public:
    Path path;
    int newhistory;
    int currenthistory;
    void clear(){
    	newhistory=-1;
        currenthistory=-1;
    }
};


struct Segment {
public:
    Path path;
    set<int> labelsequence;

    void clear(){
    	labelsequence.clear();
    }
};

struct PathHistory {
public:
    Path path;
    int history;

};


class BisimulationIndex{

public:
    int k;
    int labelnum; //original label num * 2;
    int labelidsize;
    bool workloadindex;
    vector< vector<pair<int,int>>> history2path;
    vector< vector< int >> label2history;
    vector< bool > workload;
    google::dense_hash_map<string, int> path2history;

    BisimulationIndex(std::string);
    void Output(string output_filename);
    void DeleteEdge(Edge, OriginalGraph&);
    void AddEdge(Edge, OriginalGraph&);
    void DeleteWorkload(vector<int>, OriginalGraph&);
    void FindHistory(int, set<int>);
    void AddWorkload(vector<int>, OriginalGraph&);
    void SetWorkload(std::string);
};

inline int encodeLabel(vector<int>& labellist, int labelnum, int k){

    int labelsequence=0;
    for(int i=0;i<labellist.size();i++){

        if(i==0)labelsequence += labellist[i];
        else{
            labelsequence = (labelsequence+1)*labelnum+labellist[i];
        }
    }
    return labelsequence;
}

inline vector<int> decodeLabel(int labelsequence, int labelnum, int k){

    vector<int> labellist;
    int k_value=1;

    while(1){
        labellist.push_back(labelsequence % (int) pow(labelnum, k_value));
        if(labelsequence < (int) pow(labelnum, k_value))return labellist;
        labelsequence = labelsequence/(int)pow(labelnum,k_value) -1;

        if(k_value==k){
            break;
        }
         k_value++;
    }
    return labellist;
}

struct patheqr {
	inline bool operator()(const Path& s1, const Path& s2) const {
		if (s1.src == s2.src && s1.dst == s2.dst)return true;
		else return false;
	}
};


struct pathhash {
	inline size_t operator()(const Path& s1) const {
		return ((17+s1.src)*31+s1.dst)*31;
	}
};




inline bool cmppathhistorypath(const PathHistory &p1, const PathHistory &p2){
	if(p1.path.src<p2.path.src)return true;
	if(p1.path.src==p2.path.src&&p1.path.dst<=p2.path.dst)return true;
	return false;
}

inline bool cmpsegmentpath(const Segment &p1, const Segment &p2){
	if(p1.path.src<p2.path.src)return true;
	if(p1.path.src==p2.path.src&&p1.path.dst<=p2.path.dst)return true;
	return false;
}



#endif //QUERY_BISIMULATION_INDEX_H
