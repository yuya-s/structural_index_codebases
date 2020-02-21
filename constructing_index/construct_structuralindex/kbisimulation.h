//
// Created by sasaki on 18/12/06.
//


#ifndef K_BISIMULATION_KBISIMULATION_H
#define K_BISIMULATION_KBISIMULATION_H

#include "utility.h"
#include "graph.h"

struct Path {// for google hatsh map
	int src;
	int dst;
	bool operator==(const Path& p){return (src == p.src&&dst == p.dst);}
	bool operator!=(const Path& p){return (src != p.src||dst != p.dst);}

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

struct PathwithIDs {
public:
    Path path;
    set<int> labelsequence;
    set<int> segvalue;
    bool bisimilar;
    bool operator==(const PathwithIDs& s){return (path.src == s.path.src&&path.dst==s.path.dst&&segvalue == s.segvalue&&labelsequence == s.labelsequence);}
    bool operator==(const PathwithIDs* s){return (path.src == s->path.src&&path.dst==s->path.dst&&segvalue == s->segvalue&&labelsequence == s->labelsequence);}
    void clear(){
    	labelsequence.clear();
        segvalue.clear();
    }
};



struct Segment {
public:
    Path path;
    int segid;
    int order;
    vector<int> labelsequence;
    vector<int> segset;
    int segvalue;
    bool bisimilar;
    bool loop;
    bool possiblitilyofbisimilar;

    void clear(){
    	labelsequence.clear();
    	segset.clear();
    }
};



class KPathBisimulation {

public:
	int k; //given parameters. keep 2^k length paths.
    int number_of_labels; //original label num * 2;
	int number_of_vertices; // 頂点の数
    int number_of_edges;

    vector<vector<Segment>> seg; //[length]->segment
    vector<google::dense_hash_map< Path,int,pathhash,patheqr>> path2segment;
    vector<vector<Segment*>> sortref; // for sort
    vector<int> numberofsegid;

	KPathBisimulation(int k_, OriginalGraph graph_);
	void Output(string output_filename, OriginalGraph graph_);
};

inline bool pathcmp(const Path &p1, const Path &p2){
	if(p1.src<p2.src)return true;
	if(p1.src==p2.src&&p1.dst<=p2.dst)return true;
	return false;
}

//inline bool cmpsegvalue(const Segment &a, const Segment &b)
//{
//    if(a.possiblitilyofbisimilar&&!b.possiblitilyofbisimilar)return true;
//    else if(!a.possiblitilyofbisimilar&&b.possiblitilyofbisimilar)return false;
//	else if(a.segvalue==b.segvalue){
//		if(a.path.src==a.path.dst){
//			return true;
//		}
//		else return false;
//	}
//    else return a.segvalue < b.segvalue;
//}

inline bool cmpsegvaluepointer_0(const Segment *a, const Segment *b)
{
    if(a->segvalue==b->segvalue){
		if(a->path.src==a->path.dst)return true;
		else if(b->path.src==b->path.dst)return false;
        else if(a->path.src<b->path.src)return true;
        else if(a->path.src==b->path.src&&a->path.dst<b->path.dst)return true;
        else return false;
	}
    else return a->segvalue < b->segvalue;
}

inline bool cmpsegvaluepointer(const Segment *a, const Segment *b)
{
    if(a->possiblitilyofbisimilar&&!b->possiblitilyofbisimilar)return true;
    else if(!a->possiblitilyofbisimilar&&b->possiblitilyofbisimilar)return false;
	else if(a->segvalue==b->segvalue){
        if(a->segset.size() == b->segset.size()) {
            for(int i=0;i<a->segset.size();i++){
                if((a->segset)[i] != b->segset[i])return a->segset[i]<b->segset[i];
            }
            if (a->path.src == a->path.dst)return true;
            else if (b->path.src == b->path.dst)return false;
            else if (a->path.src < b->path.src)return true;
            else if (a->path.src == b->path.src && a->path.dst < b->path.dst)return true;
            else return false;
        }
        else a->segset.size() < b->segset.size();
	}
    else return a->segvalue < b->segvalue;
}

inline bool cmpsrc(const Segment *a, const Segment *b)
{
	if(a->path.src==b->path.src){
		return a->path.dst< b->path.dst;
	}
    else return a->path.src < b->path.src;
}

//inline bool cmpdst(const Segment &a, const Segment &b)
//{
//	if(a.path.dst==b.path.dst){
//		return a.path.src< b.path.src;
//	}
//    else return a.path.dst < b.path.dst;
//}
//
//inline bool cmppathwithid(const PathwithIDs &a, const PathwithIDs &b)
//{
//
//    if(a.path.src<b.path.src)return true;
//    if(a.path.src==b.path.src&&a.path.dst<b.path.dst)return true;
//    if(a.path.src==b.path.src&&a.path.dst==b.path.dst&&a.segvalue<b.segvalue)return true;
//
//    return false;
//}
//
//inline bool cmppathwithidpointer(const PathwithIDs *a, const PathwithIDs *b)
//{
//
//    if(a->path.src<b->path.src)return true;
//    if(a->path.src==b->path.src&&a->path.dst<b->path.dst)return true;
//    if(a->path.src==b->path.src&&a->path.dst==b->path.dst&&a->segvalue<b->segvalue)return true;
//
//    return false;
//}


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

//inline vector<int> decodeLabel(int labelsequence, int labelnum, int k){
//
//    vector<int> labellist;
//    int k_value=k;
//
//    while(1){
//        if(labelsequence/(int)pow(labelnum,k_value)>0) {
//            labellist.push_back(labelsequence % (int) pow(labelnum, k_value)-1);
//            labelsequence = labelsequence/(int)pow(labelnum,k_value);
//        }
//        k_value--;
//        if(k_value==0){
//            labellist.push_back(labelsequence);
//            break;
//        }
//    }
//    return labellist;
//}
//
//inline int encodeLabel(vector<int>& labellist, int labelnum, int k){
//
//    int labelsequence=0;
//    for(int i=0;i<labellist.size();i++){
//
//        if(i==0)labelsequence += labellist[i];
//        else{
//            labelsequence += (labellist[i]+1)*labelnum;
//        }
//    }
//    return labelsequence;
//}

#endif //K_BISIMULATION_KBISIMULATION_H
