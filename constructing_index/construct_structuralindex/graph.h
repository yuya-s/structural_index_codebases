//
// Created by sasaki on 18/12/05.
//

#ifndef K_BISIMULATION_GRAPH_H
#define K_BISIMULATION_GRAPH_H

#include "utility.h"

struct Edge {
public:
	int src;
	int dst;
    int label;

	Edge(int src_, int dst_,int label_)
	{
		src = src_;
		dst = dst_;
        label = label_;
	}
};

class OriginalGraph {

public:
	int number_of_vertices;
    int number_of_edges;
    int number_of_labels;
	vector<Edge> edge_list;
    vector< vector <int> > vertex_edge_list;
    vector<int> degrees;
	int max_degree;

	OriginalGraph(){
		edge_list.clear();
        degrees.clear();
		max_degree=0;
    }

    void InputFile(std::string);
};

inline bool cmpedge(const Edge &p1, const Edge &p2){
	if(p1.src>p2.src)return true;
	if(p1.src==p2.src&&p1.dst>=p2.dst)return true;
	return false;
}

#endif //K_BISIMULATION_GRAPH_H
