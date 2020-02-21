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

	Edge(int src_, int dst_,int label_)
	{
		src = src_;
		dst = dst_;
        label = label_;
	}
};

class OriginalGraph {

public:
	int number_of_vertices; // 頂点の数
    int number_of_edges;
    int number_of_labels;
	vector<Edge> edge_list; // 辺リスト
    vector< vector <int> > vertex_edge_list;
    vector<int> degrees;
    double average_degrees;
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
	if(p1.src>p2.src)return true;
	if(p1.src==p2.src&&p1.dst>=p2.dst)return true;
	return false;
}

#endif //K_BISIMULATION_GRAPH_H
