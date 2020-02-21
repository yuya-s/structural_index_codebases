//
// Created by sasaki on 19/04/18.
//

#ifndef QUERY_BISIMULATION_QUERY_H
#define QUERY_BISIMULATION_QUERY_H

#include "index.h"
#include "utility.h"


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

    void NewLeft(){
        leftNode = new(QueryNode);
        leftNode->clear();
    };
    void NewRight(){
        rightNode = new(QueryNode);
        rightNode->clear();
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
    void Evaluation(vector<pair<int,int>>&, BisimulationIndex&);
    void Evaluation(vector<pair<int,int>>&, vector<int>&, BisimulationIndex&, QueryNode*);
    void Join(BisimulationIndex&, vector<pair<int,int>>&, vector<int>&, vector<pair<int,int>>&,vector<pair<int,int>>&, vector<int>&, vector<int>&);
    void JoinCompare(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);
    void JoinID(BisimulationIndex&, vector<pair<int,int>>&, vector<int>&, vector<pair<int,int>>&,vector<pair<int,int>>&, vector<int>&, vector<int>&);
    void JoinCompareID(vector<pair<int,int>>&, vector<pair<int,int>>&,vector<pair<int,int>>&);


    void Conjunction(BisimulationIndex&, vector<pair<int,int>>&, vector<int>&,vector<pair<int,int>>&,vector<pair<int,int>>&, vector<int>&, vector<int>&);

     void ConjunctionID(BisimulationIndex&, vector<pair<int,int>>&, vector<int>&,vector<pair<int,int>>&,vector<pair<int,int>>&, vector<int>&, vector<int>&);

    void Label2Path(vector<pair<int,int>>&, BisimulationIndex&, vector<int>&);
    void Label2PathID(vector<pair<int,int>>&, BisimulationIndex&, vector<int>&);
    void Label2History(vector<int>&, BisimulationIndex&, vector<int>&);
    void Label2HistoryID(vector<int>&, BisimulationIndex&, vector<int>&);

    void ShowQuery(QueryNode*);
    void SortPathHistory(BisimulationIndex&, vector<pair<int,int>>&, vector<int>&, vector<int>&);

    void Output(string output_filename);
};


class QuerySet{
public:
    vector<Query> rootqueries;
    void InputFile(string, BisimulationIndex);
};

inline bool cmpdst(const pair<int,int> &a, const pair<int,int> &b)
{
	if(a.second==b.second){
		return a.first< b.first;
	}
    else return a.second < b.second;
}


#endif //QUERY_BISIMULATION_QUERY_H
