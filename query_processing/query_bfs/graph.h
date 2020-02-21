//
// Created by sasaki on 18/12/05.
//

#ifndef K_BISIMULATION_GRAPH_H
#define K_BISIMULATION_GRAPH_H

#include "utility.h"

struct Edge {
public:
	int src;  // 辺の始点
	int dst; // 辺の終点
    int label;

    bool operator==(const Edge& s){return (src == s.src&&dst==s.dst&&label == s.label);}
    bool operator<(const Edge& s) const {return (src < s.src||(src==s.src&&dst<s.dst)||(src==s.src&&dst==s.dst&&label <= s.label));}
    
	Edge(int src_, int dst_,int label_)
	{
		src = src_;
		dst = dst_;
        label = label_;
	}
};

struct EdgeWithLabelset {
public:
	int src;  // 辺の始点
	int dst; // 辺の終点
    vector<int> labelset;

	EdgeWithLabelset(int src_, int dst_)
	{
		src = src_;
		dst = dst_;
        labelset.clear();
	}
};



class OriginalGraph {

public:
	int number_of_vertices; // 頂点の数
    int number_of_edges;
    int number_of_labels;
    int max_vertexid;
	vector<Edge> edge_list; // 辺リスト
    vector< vector <int> > vertex_edge_list;
    vector< vector <int> > vertex_inverseedge_list;
    vector<int> degrees;
	int max_degree;
    google::dense_hash_map<std::string,int> vertexname2id;
    google::dense_hash_map<string,int> labelname2id;
	google::dense_hash_map<int, string> id2vertexname;
    google::dense_hash_map<int, string> id2labelname;

	OriginalGraph(){
		edge_list.clear();
        degrees.clear();
		max_degree=0;
    }

	int GetNumberOfVertices() // 頂点の数を返す
	{
		return number_of_vertices;
	}

	vector<Edge>* GetEdgeList() // 辺リストを返す
	{
		return &edge_list;
	}

    void InputFile(std::string);
};

inline bool cmpedge(const Edge &p1, const Edge &p2){
	if(p1.src<p2.src)return true;
	if(p1.src==p2.src&&p1.dst<=p2.dst)return true;
	return false;
}

inline bool cmpedgewithlabelse(const EdgeWithLabelset &p1, const EdgeWithLabelset &p2){
	if(p1.src<p2.src)return true;
	if(p1.src==p2.src&&p1.dst<=p2.dst)return true;
	return false;
}

inline bool cmpedgelabel(const Edge &p1, const Edge &p2){
	if(p1.src < p2.src)return true;
	else if(p1.src==p2.src&&p1.dst < p2.dst)return true;
    else if(p1.src==p2.src&&p1.dst==p2.dst&&p1.label < p2.label)return true;
	return false;
}


#endif //K_BISIMULATION_GRAPH_H
