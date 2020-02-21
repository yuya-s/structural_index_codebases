//
// Created by sasaki on 18/12/06.
//


#ifndef K_BISIMULATION_KBISIMULATION_H
#define K_BISIMULATION_KBISIMULATION_H


#include "utility.h"
#include "graph.h"
#include "workload.h"
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
    int order;
    set<int> labelsequence;
    void clear(){
    	labelsequence.clear();
    }
    bool operator==(const Segment& s){return (path.src == s.path.src&&path.dst==s.path.dst&&labelsequence == s.labelsequence);}
};



class WorkloadawareTransequivalent {

public:
	int k; //given parameters. keep 2^k length paths.
    int number_of_labels; //original label num * 2;
	int number_of_vertices;
    int number_of_edges;

    vector<vector<Segment>> seg; //[length]->segment
    vector<google::dense_hash_map< Path,int,pathhash,patheqr>> path2segment;


	WorkloadawareTransequivalent(int k_, OriginalGraph graph_, WorkLoad workload_);
	void Output(string output_filename, OriginalGraph graph_);

};


inline bool segmentcmppointer(const Segment *s1, const Segment *s2){
	if(s1->labelsequence.size() == s2->labelsequence.size()){
		if(s1->labelsequence.size()==0)cout<<"ohno"<<endl;
		auto itrs1 = s1->labelsequence.begin();
		auto itrs2 = s2->labelsequence.begin();
		for(int i=0;i<s1->labelsequence.size();i++){
			if(*itrs1 != *itrs2)return *itrs1 < *itrs2;//return true;
			itrs1++;itrs2++;
		}
		if(s1->path.src == s1->path.dst&&s2->path.src != s2->path.dst){
			return true;
		}
		else if(s1->path.src != s1->path.dst&&s2->path.src == s2->path.dst){
			return false;
		}
		else if(s1->path.src == s1->path.dst&&s2->path.src == s2->path.dst){
			return s1->path.src < s2->path.src;
		}
		else{
			if(s1->path.src == s2->path.src){
				return s1->path.dst < s2->path.dst;
			}
			else{ 
				return s1->path.src < s2->path.src;
			}
		}
	}
	else return s1->labelsequence.size()<s2->labelsequence.size();			
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
