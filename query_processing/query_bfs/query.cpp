//
// Created by sasaki on 19/04/17.
//
#include "utility.h"
#include "query.h"
#include "graph.h"



void QuerySet::InputFile(string workload_file){

    const char *workloadhfile=workload_file.c_str();
    std::ifstream inputwork(workloadhfile);

    if(!inputwork){
        std::cout<<"error: cannot open graph and/or workload files"<<std::endl;
        exit(1);
    }

    Query tempQuery;
    string line, label;

    //std::stringstream ss(line);
    //ss.ignore(line.size(), ' ');
    //std::cout << name << " = ";
    QueryNode rootNode;
    QueryNode* tempNode;
    rootNode.clear();
    //tempNode->clear();
    tempNode = &rootNode;
    vector<int> labelsequence;
    bool function;
    bool leftdone=false;
    vector<vector<string>> perser;

    while(std::getline(inputwork,line)){
//        cout<<line<<endl;
        bool queryDoneFlag=false;
        bool labelFlag=false;
        bool idFlag=false;
        std::stringstream ss;
        ss<<line;
        //cout<<ss.str()<<endl;
        //ss.ignore(line.size(), ' ');
        while(ss >> label)
        {
            if(label == "!"){
                queryDoneFlag=true;
                break;
            }
            else if(label == "c"||label == "j"||label == "c_id"||label == "j_id"){
                if(label == "c")tempNode->SetOperation(CONJ);
                else if(label =="j")tempNode->SetOperation(JOIN);
                else if (label =="c_id")tempNode->SetOperation(CONJ_ID);
                else if (label =="j_id")tempNode->SetOperation(JOIN_ID);
                //tempNode->hight++;
                tempNode->NewLeft();
                tempNode->NewRight();
                tempNode->leftNode->topNode = tempNode;
                tempNode->rightNode->topNode = tempNode;
                tempNode = tempNode->leftNode;
            }
            else{
                labelFlag=true;
                if(label=="id")idFlag=true;
                else labelsequence.push_back(stoi(label));
            }
        }
        if(labelFlag){
            tempNode->SetLabelSequence(labelsequence);
            if(idFlag)tempNode->SetOperation(ID);
            labelsequence.clear();
            if(tempNode->topNode!=NULL) {
                if (!tempNode->topNode->leftdone) {
                    tempNode->topNode->leftdone = true;
                    tempNode = tempNode->topNode->rightNode;
                } else {
                    //tempNode->topNode->rightdone = true;

                    while (1) {
                        tempNode = tempNode->topNode;
                        if (tempNode == NULL)break;
                        tempNode->hight = max(tempNode->leftNode->hight, tempNode->rightNode->hight) + 1;
                        if (!tempNode->leftdone) {
			    tempNode->leftdone=true;
                            tempNode = tempNode->rightNode;
                            break;
                        }
                    }
                }
            }
        }
        if(queryDoneFlag){
            tempQuery.rootquery=rootNode;
            rootqueries.push_back(tempQuery);
        //tempQuery.ShowQuery(&tempQuery.rootquery);
            rootNode.clear();
            tempNode = &rootNode;
        }
        //std::cout << "\n";
    }
}


void Query::ShowQuery(QueryNode* query_)
{
	cout<<"hight:"<<query_->hight<<", operation" <<query_->operation<<endl;
	for(auto l : query_->labelsequence)cout<<l<<",";
	cout<<endl;
	if(query_->leftNode !=NULL)ShowQuery(query_->leftNode);
	if(query_->rightNode!=NULL)ShowQuery(query_->rightNode);
}


void Query::Evaluation(vector<pair<int,int>>& answers, OriginalGraph& graph){

    if(rootquery.hight==0){
        if(rootquery.operation==ID)Label2PathID(answers, graph,rootquery.labelsequence);
        else {
	    Label2Path(answers, graph,rootquery.labelsequence);
       //Label2Path(answers, pathindex,rootquery.labelsequence);
	}
	//cout<<answers.size()<<endl;
    }
    else{

        vector<pair<int,int>> lPath;lPath.clear(); Evaluation(lPath, graph, rootquery.leftNode);
        vector<pair<int,int>> rPath;rPath.clear(); Evaluation(rPath,  graph,  rootquery.rightNode);
       //cout<<"leftsize: "<<lPath.size()<<", rightsize: "<<rPath.size()<<endl;

        if(rootquery.operation == JOIN){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end(),cmpdst);
            //cout<<lPath.size()<<","<<rPath.size()<<endl;
		    //cout<< "sort Done"<<endl;
            Join(answers, lPath,rPath);
            //cout<<answers.size()<<endl;
        }
        else if(rootquery.operation == CONJ){
            sort(lPath.begin(),lPath.end());
            sort(rPath.begin(),rPath.end());

            Conjunction(answers, lPath,rPath);
        }
        else if(rootquery.operation == JOIN_ID){
		
            //cout<<lPath.size()<<","<<rPath.size()<<endl;
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end(),cmpdst);

	        JoinID(answers, lPath,rPath);
	    }
        else if(rootquery.operation == CONJ_ID){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end());

            ConjunctionID(answers, lPath,rPath);
        }
    }
};

void Query::Evaluation(vector<pair<int,int>>& answers, OriginalGraph& graph, QueryNode* query_node){

    if(query_node->hight==0){
        if(query_node->operation==ID)Label2PathID(answers, graph, query_node->labelsequence);
        else Label2Path(answers, graph, query_node->labelsequence);
    	//cout<<answers.size()<<endl;
	}
    else{
        vector<pair<int,int>> lPath;lPath.clear(); Evaluation(lPath, graph, query_node->leftNode);
        vector<pair<int,int>> rPath;rPath.clear(); Evaluation(rPath, graph,  query_node->rightNode);

            //cout<<"leftsize: "<<lPath.size()<<", rightsize: "<<rPath.size()<<", join:"<<answers.size()<<endl;
        if(query_node->operation == JOIN){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end(),cmpdst);

            Join(answers, lPath,rPath);
            sort(answers.begin(),answers.end());
            answers.erase(std::unique(answers.begin(), answers.end()), answers.end());
            //cout<<"leftsize: "<<lPath.size()<<", rightsize: "<<rPath.size()<<", join:"<<answers.size()<<endl;
        }
        else if(query_node->operation == CONJ){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end());

            Conjunction(answers, lPath,rPath);
        }
        else if(query_node->operation == JOIN_ID){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end(),cmpdst);
            JoinID(answers, lPath,rPath);
        }
        else if(query_node->operation == CONJ_ID){
            sort(rPath.begin(),rPath.end());
            sort(lPath.begin(),lPath.end());

            ConjunctionID(answers, lPath,rPath);
        }

    }
};

void Query::Join(vector<pair<int,int>>& answers, vector<pair<int,int>>& l, vector<pair<int,int>>& r){

    int for_r=0;
    int restart_r=0;
    bool restartFlag=true;
    int rsize = r.size();
    if(r.size()==0||l.size()==0)return;
    for(int i=0;i<l.size();i++){
        //cout<<i<<":"<<l.size()<<endl;
        for_r=restart_r;
        restartFlag=true;
        while(1) {
            if (l[i].second == r[for_r].first) {
                answers.push_back(make_pair(l[i].first, r[for_r].second));
                if(restartFlag){
                    restart_r=for_r;
                    restartFlag=false;
                }
            }
            if (l[i].second < r[for_r].first) {
                if(restartFlag)restart_r=for_r;
                break;
            }

            for_r++;
            if(for_r >= rsize)break;
        }
        if(for_r >= rsize&&restartFlag)break;
    }
};

void Query::JoinID(vector<pair<int,int>>& answers, vector<pair<int,int>>& l, vector<pair<int,int>>& r){

    int for_r=0;
    int restart_r=0;
    bool restartFlag=true;
    int rsize = r.size();
    if(r.size()==0||l.size()==0)return;
    for(int i=0;i<l.size();i++){
        //cout<<i<<":"<<l.size()<<endl;
        for_r=restart_r;
        restartFlag=true;
        while(1) {
            if (l[i].second == r[for_r].first) {
                if(l[i].first == r[for_r].second)answers.push_back(make_pair(l[i].first, r[for_r].second));
                if(restartFlag){
                    restart_r=for_r;
                    restartFlag=false;
                }
            }
            if (l[i].second < r[for_r].first) {
                if(restartFlag)restart_r=for_r;
                break;
            }

            for_r++;
            if(for_r >= rsize)break;

        }
        if(for_r >= rsize&&restartFlag)break;
    }
};

void Query::Conjunction(vector<pair<int,int>>& answers, vector<pair<int,int>>& l, vector<pair<int,int>>& r){
    //vector<pair<int,int>>* pathlist;
    int lcount=0,rcount=0;
    //cout<<"start conjunction"<<endl;
    while(1){
        if(lcount>=l.size()||rcount>=r.size())break;
        //cout<<lcount<<","<<rcount<<endl;
        if(l[lcount] == r[rcount]){
            answers.push_back(l[lcount]);
            rcount++;
            lcount++;
        }
        else if(l[lcount].first < r[rcount].first||(l[lcount].first == r[rcount].first &&l[lcount].second < r[rcount].second)){
            lcount++;
        }
        else rcount++;
    }
    //cout<<"DONE conjunction"<<endl;

};

void Query::ConjunctionID(vector<pair<int,int>>& answers, vector<pair<int,int>>& l, vector<pair<int,int>>& r){
    //vector<pair<int,int>>* pathlist;
    int lcount=0,rcount=0;
    while(1){

        if(lcount>=l.size()||rcount>=r.size())break;
        if(l[lcount] == r[rcount]){
            if(l[lcount].first==l[lcount].second)answers.push_back(l[lcount]);
            rcount++;
            lcount++;
        }
        else if(l[lcount].first < r[rcount].first||(l[lcount].first == r[rcount].first &&l[lcount].second < r[rcount].second)){
            lcount++;
        }
        else rcount++;

        //if(lcount>l.size()||rcount>r.size())break;
    }

};

void Query::Label2Path(vector<pair<int,int>>& answers, OriginalGraph& graph, vector<int>& labellist){


    for(int for_v=0; for_v < graph.number_of_vertices; for_v++){

        vector<int> candidateSet;
        candidateSet.clear();
        vector<int> nextCandidateSet;
        nextCandidateSet.clear();

        candidateSet.push_back(for_v);

        for(int for_label = 0; for_label < labellist.size(); for_label++) {
            nextCandidateSet.clear();

            for (int candidate : candidateSet) {
                if(labellist[for_label]<graph.number_of_labels) {
                    for (int edgeid : graph.vertex_edge_list[candidate]) {
                        Edge tempedge = graph.edge_list[edgeid];
                        if (labellist[for_label] == tempedge.label)nextCandidateSet.push_back(tempedge.dst);
                    }
                }
                else{
                    for (int edgeid : graph.vertex_inverseedge_list[candidate]) {
                        Edge tempedge = graph.edge_list[edgeid];
                        if (labellist[for_label] == tempedge.label+graph.number_of_labels)nextCandidateSet.push_back(tempedge.src);
                    }
                }
            }
            candidateSet = nextCandidateSet;
        }

        sort(candidateSet.begin(), candidateSet.end());
        candidateSet.erase(std::unique(candidateSet.begin(), candidateSet.end()), candidateSet.end());


        for(int candidate : candidateSet){
            answers.push_back(make_pair(for_v, candidate));
        }
    }

//    for(auto path : pathindex.label2path[labelid]){
//        answers.push_back(path);
//    }
};

void Query::Label2PathID(vector<pair<int,int>>& answers,  OriginalGraph& graph, vector<int>& labellist){


        for(int for_v=0; for_v < graph.number_of_vertices; for_v++){

        vector<int> candidateSet;
        candidateSet.clear();
        vector<int> nextCandidateSet;
        nextCandidateSet.clear();

        candidateSet.push_back(for_v);

        for(int for_label = 0; for_label < labellist.size(); for_label++) {
            nextCandidateSet.clear();

            for (int candidate : candidateSet) {
                if(labellist[for_label]<graph.number_of_labels) {
                    for (int edgeid : graph.vertex_edge_list[candidate]) {
                        Edge tempedge = graph.edge_list[edgeid];
                        if (labellist[for_label] == tempedge.label)nextCandidateSet.push_back(tempedge.dst);
                    }
                }
                else{
                    for (int edgeid : graph.vertex_inverseedge_list[candidate]) {
                        Edge tempedge = graph.edge_list[edgeid];
                        if (labellist[for_label] == tempedge.label+graph.number_of_labels)nextCandidateSet.push_back(tempedge.src);
                    }
                }
            }
            candidateSet = nextCandidateSet;
        }

        sort(candidateSet.begin(), candidateSet.end());
        candidateSet.erase(std::unique(candidateSet.begin(), candidateSet.end()), candidateSet.end());

        for(int candidate : candidateSet){
            if(for_v==candidate)answers.push_back(make_pair(for_v, candidate));
        }
    }

//    int labelid = encodeLabel(labellist, pathindex.labelnum,pathindex.k);
//    for(auto path : pathindex.label2path[labelid]){
//        if(path.first==path.second)answers.push_back(path);
//    }

};
