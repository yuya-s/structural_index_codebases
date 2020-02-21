//
// Created by sasaki on 18/12/06.
//


#ifndef K_BISIMULATION_KBISIMULATION_H
#define K_BISIMULATION_KBISIMULATION_H

//#include <bits/unordered_map.h>
//#include <unordered_map>
#include "utility.h"
#include "graph.h"

struct Path {// for google hats map
	int src;
	int dst;
	bool operator==(const Path& p){return (src == p.src&&dst == p.dst);}
	bool operator!=(const Path& p){return (src != p.src||dst != p.dst);}
    bool operator<(const Path& p){return (src < p.src||(src == p.src&&dst < p.dst));}
    bool operator>(const Path& p){return (src > p.src||(src == p.src&&dst > p.dst));}
};


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



struct Segment {
public:
    Path path;
    //int segid;
    int order;
    set<int> labelsequence;
    //set<pair<int,int>> segset;
    //int segvalue;
    void clear(){
    	labelsequence.clear();
    }
    bool operator==(const Segment& s){return (path.src == s.path.src&&path.dst==s.path.dst&&labelsequence == s.labelsequence);}
};



class PathSet {

public:
	int k; //given parameters. keep 2^k length paths.
    int number_of_labels; //original label num * 2;
	int number_of_vertices; // 頂点の数
    int number_of_edges;
    //int number_of_labels;

    vector<vector<Segment>> seg; //[length]->segment
    //vector<vector<vector<pair<int,int>>>> path2segment;
    //google::dense_hash_map<int, vector<int> > hash2pathlabel;//hash
	//vector<set<pair<int,int>>> pathlist;// [length]->set of paths.
    vector<google::dense_hash_map< Path,int,pathhash,patheqr>> path2segment;
    vector<vector<Segment*>> sortref; // for sort
    //vector<int> numberofsegid;
	//vector<Path> pathlist;

	PathSet(int k_, OriginalGraph graph_);
    int find(vector<pair<int,int>>&, int);
    void update(vector<pair<int,int>>&, int, int);
	void Output(string output_filename, OriginalGraph graph_);
	//static bool cmpsegvalue(const Segment *, const Segment *);
    //static bool cmpsrc(const Segment*, const Segment*);
    //static bool cmpdst(const Segment&, const Segment&);
};

inline bool pathcmp(const Path &p1, const Path &p2){
	if(p1.src>p2.src)return true;
	if(p1.src==p2.src&&p1.dst>=p2.dst)return true;
	return false;
}

inline bool paircmp(const pair<int,int> &p1, const pair<int,int> &p2){
	if(p1.first<p2.first)return true;
	if(p1.first==p2.first&&p1.second<=p2.second)return true;
	return false;
}

inline bool segmentcmp(const Segment &s1, const Segment &s2){
  	if(s1.path.src<s2.path.src)return true;
    else if(s1.path.src==s2.path.src&&s1.path.dst<s2.path.dst)return true;
  	else if(s1.path.src==s2.path.src&&s1.path.dst==s2.path.dst&&s1.labelsequence<s2.labelsequence)return true;
//  	if(s1.path.src==s2.path.src&&s1.path.dst==s2.path.dst&&s1.labelsequence==s2.labelsequence&&s1.order<s2.order)return true;
	return false;
}


inline bool cmpsrc(const Segment *a, const Segment *b)
{
	if(a->path.src==b->path.src){
		return a->path.dst< b->path.dst;
	}
    else return a->path.src < b->path.src;
}

inline bool cmpdst(const Segment &a, const Segment &b)
{
	if(a.path.dst==b.path.dst){
		return a.path.src< b.path.src;
	}
    else return a.path.dst < b.path.dst;
}


inline Path MinPath(vector<Segment*> &segvector){

    Path p = segvector[0]->path;

    for(int i=1;i < segvector.size();i++){
        if(p.src > segvector[i]->path.src){
            p = segvector[i]->path;
        }
        else if(p.src==segvector[i]->path.src&&p.dst > segvector[i]->path.dst){
            p=segvector[i]->path;
        }
    }
    return p;
}

inline vector<int> decodeLabel(int labelsequence, int labelnum, int k){

    vector<int> labellist;
    int k_value=k;

    while(1){
        if(labelsequence/(int)pow(labelnum,k_value)>0) {
            labellist.push_back(labelsequence % (int) pow(labelnum, k_value)-1);
            labelsequence = labelsequence/(int)pow(labelnum,k_value);
        }
        k_value--;
        if(k_value==0){
            labellist.push_back(labelsequence);
            break;
        }
    }
    return labellist;
}

inline int encodeLabel(vector<int>& labellist, int labelnum, int k){

    int labelsequence=0;
    for(int i=0;i<labellist.size();i++){

        if(i==0)labelsequence += labellist[i];
        else{
            labelsequence += (labellist[i]+1)*labelnum;
        }
    }
    return labelsequence;
}

#endif //K_BISIMULATION_KBISIMULATION_H
