//
// Created by sasaki on 19/04/16.
//

#ifndef QUERY_PATHINDEX_QUERY_H
#define QUERY_PATHINDEX_QUERY_H

#include "index.h"
#include "utility.h"
#include "graph.h"

#define JOIN 0
#define CONJ 1
#define JOIN_ID 2
#define CONJ_ID 3
#define ID 4

struct QueryNode{

    int hight;
    vector<int> labelsequence;
    int operation; //0=join, 1=conjunction
    QueryNode *topNode;
    QueryNode *leftNode;
    QueryNode *rightNode;
    bool leftdone;
    bool rightdone;


    void clear(){
    	labelsequence.clear();
    	hight = 0;
        operation = 0;
        rightNode =NULL;
        leftNode =NULL;
        topNode =NULL;
        leftdone=false;
        rightdone=false;
    }
    void SetFunction(int function, QueryNode* left, QueryNode* right){
        hight++;
        operation = function;
        leftNode = left;
        rightNode = right;
    };
    void SetLeft(QueryNode* left){
        leftNode = left;
        leftNode->clear();
    };
    void NewLeft(){
        leftNode = new(QueryNode);
        leftNode->clear();
    };
    void NewRight(){
        rightNode = new(QueryNode);
        rightNode->clear();
    };
    void SetRight(QueryNode* right){
        rightNode = right;
    };
    void SetOperation(int function){
        operation = function;
    };

    void SetLabelSequence(vector<int> labelsequence_){
        for(int i=0;i<labelsequence_.size();i++)labelsequence.push_back(labelsequence_[i]);
    };

};


class Query{

public:
    QueryNode rootquery;

    Query(){rootquery.clear();};
    void Evaluation(vector<pair<int,int>>&, OriginalGraph& graph);
    void Evaluation(vector<pair<int,int>>& , OriginalGraph& graph, QueryNode*);
    void Join(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);
    void Conjunction(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);
    void JoinID(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);
    void ConjunctionID(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);
    void Label2Path(vector<pair<int,int>>&,  OriginalGraph& graph, vector<int>&);
    void Label2PathID(vector<pair<int,int>>&,  OriginalGraph& graph, vector<int>&);
    void Output(string output_filename);
    void ShowQuery(QueryNode*);
};

class QuerySet{
public:
    vector<Query> rootqueries;
    void InputFile(string);
    void EnumerateQuery(int);
};



inline bool cmpdst(const pair<int,int> &a, const pair<int,int> &b)
{
	if(a.second==b.second){
		return a.first< b.first;
	}
    else return a.second < b.second;
}

#endif //QUERY_PATHINDEX_QUERY_H
