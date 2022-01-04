
//

#include "utility.h"
#include "query.h"

void QuerySet::InputFile(string workload_file, BisimulationIndex index_){
    const char *workloadhfile=workload_file.c_str();
    std::ifstream inputwork(workloadhfile);

    if(!inputwork){
        std::cout<<"error: cannot open workload files"<<std::endl;
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

    vector<vector<string>> perser;

    vector<string> queryprocess;
    vector<vector<string>> queryprocessset;
    vector<int> labelset;
    while(std::getline(inputwork,line)){
//        cout<<line<<endl;
        bool queryDoneFlag=false;
        bool labelFlag=false;
        bool idFlag=false;
        std::stringstream ss;
        ss<<line;
        queryprocess.clear();
        labelset.clear();
        while(ss >> label){
            if(label=="!"){queryDoneFlag=true;break;}
            if(label != "!"&&label != "c"&&label != "j"&&label != "c_id"&&label != "j_id"){
                labelFlag=true;
                if(label=="id")idFlag=true;
                else labelset.push_back(stoi(label));
            }
            queryprocess.push_back(label);
        }

        if(index_.workloadindex&&labelFlag){

            while(1) {
                if(index_.workload[encodeLabel(labelset, index_.labelnum, index_.k)]){
                   queryprocessset.push_back(queryprocess);
                   break;

                }
                else{
                    vector<string> tempprocess;
                    tempprocess.clear();
                    if(idFlag){
                        tempprocess.push_back("j_id");
                        queryprocessset.push_back(tempprocess);
                        queryprocess.pop_back();
                    }
                    else{
                        tempprocess.push_back("j");
                        queryprocessset.push_back(tempprocess);
                    }

                    labelset.erase(labelset.begin());
                    tempprocess.clear();
                    tempprocess.push_back(queryprocess[0]);
                    queryprocessset.push_back(tempprocess);
                    queryprocess.erase(queryprocess.begin());
                }
            }
        }
        else if(!queryDoneFlag)queryprocessset.push_back(queryprocess);

        if(!queryDoneFlag)continue;

        for(auto query_ps : queryprocessset) {
            labelFlag=false;
            for (string label  : query_ps) {
	    	//cout <<label<<endl;
                if (label == "!") {
                    queryDoneFlag = true;
                    break;
                } else if (label == "c" || label == "j" || label == "c_id" || label == "j_id") {
                    if (label == "c")tempNode->SetOperation(CONJ);
                    else if (label == "j")tempNode->SetOperation(JOIN);
                    else if (label == "c_id")tempNode->SetOperation(CONJ_ID);
                    else if (label == "j_id")tempNode->SetOperation(JOIN_ID);
                    //tempNode->hight++;
                    tempNode->NewLeft();
                    tempNode->NewRight();
                    tempNode->leftNode->topNode = tempNode;
                    tempNode->rightNode->topNode = tempNode;
                    tempNode = tempNode->leftNode;
                } else {
                    labelFlag = true;
                    if (label == "id")idFlag = true;
                    else labelsequence.push_back(stoi(label));
                }
            }
            if (labelFlag) {
                tempNode->SetLabelSequence(labelsequence);
                if (idFlag)tempNode->SetOperation(ID);
                labelsequence.clear();
                if (tempNode->topNode != NULL) {
                    if (!tempNode->topNode->leftdone) {
                        tempNode->topNode->leftdone = true;
                        tempNode = tempNode->topNode->rightNode;
                    } else {

                        while (1) {
                            tempNode = tempNode->topNode;
                            if (tempNode == NULL)break;
                            tempNode->hight = max(tempNode->leftNode->hight, tempNode->rightNode->hight) + 1;
                            if (!tempNode->leftdone) {
			        tempNode->leftdone =true;
                                tempNode = tempNode->rightNode;
                                break;
                            }
                        }
                    }
                }

            }
        }
        tempQuery.rootquery=rootNode;
        rootqueries.push_back(tempQuery);
        //tempQuery.ShowQuery(&tempQuery.rootquery);
	rootNode.clear();
        tempNode = &rootNode;
        queryprocessset.clear();
    }
}

void Query::ShowQuery(QueryNode* query_)
{
	cout<<"operation:" <<query_->operation<<endl;
	for(auto l : query_->labelsequence)cout<<l<<",";
	cout<<endl;
	if(query_->leftNode !=NULL)ShowQuery(query_->leftNode);
	if(query_->rightNode!=NULL)ShowQuery(query_->rightNode);
}

void Query::Evaluation(vector<pair<int,int>>& answers, BisimulationIndex& bisimulationindex){

	if(rootquery.hight==0){
        if(rootquery.operation==ID)Label2PathID(answers, bisimulationindex,rootquery.labelsequence);
        else{

            Label2Path(answers, bisimulationindex,rootquery.labelsequence);
	    }

    }
    else{
        if(rootquery.operation==JOIN){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();
            Evaluation(lPath, lHistory,bisimulationindex, rootquery.leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, rootquery.rightNode);

            vector<int> histories;
            histories.clear();
            Join(bisimulationindex, answers, histories, lPath, rPath, lHistory, rHistory);
        }
        else if(rootquery.operation == CONJ) {//CONJUNCTION
             vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();

            Evaluation(lPath, lHistory,bisimulationindex, rootquery.leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, rootquery.rightNode);


            vector<int> histories;
            histories.clear();
            Conjunction(bisimulationindex, answers, histories, lPath, rPath,lHistory, rHistory);
            if(!histories.empty()) {
                for (int history : histories) {
                    for (pair<int, int> path: bisimulationindex.history2path[history]) {
                        answers.push_back(path);
                    }
                }
            }
        }
        else if(rootquery.operation == JOIN_ID){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();


            Evaluation(lPath, lHistory,bisimulationindex, rootquery.leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, rootquery.rightNode);

            vector<int> histories;
            histories.clear();
            JoinID(bisimulationindex, answers, histories, lPath, rPath, lHistory, rHistory);
        }
        else if(rootquery.operation == CONJ_ID){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();
            Evaluation(lPath, lHistory,bisimulationindex, rootquery.leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, rootquery.rightNode);

           vector<int> histories;
            histories.clear();
            ConjunctionID(bisimulationindex, answers, histories, lPath, rPath,lHistory, rHistory);
            if(!histories.empty()) {
                for (int history : histories) {
                    for (pair<int, int> path: bisimulationindex.history2path[history]) {
                        answers.push_back(path);
                    }
                }
            }
        }
    }
};


void Query::Evaluation(vector<pair<int,int>>& answers, vector<int>& histories, BisimulationIndex& bisimulationindex, QueryNode* query_node){

    if(query_node->hight==0){
        if(query_node->operation==ID) Label2HistoryID(histories, bisimulationindex,query_node->labelsequence);
        else Label2History(histories, bisimulationindex,query_node->labelsequence);

     }
    else{
        if(query_node->operation==JOIN){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();


            Evaluation(lPath, lHistory,bisimulationindex, query_node->leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, query_node->rightNode);

            Join(bisimulationindex, answers, histories, lPath, rPath, lHistory, rHistory);
            sort(answers.begin(),answers.end());
            answers.erase(std::unique(answers.begin(), answers.end()), answers.end());
            return;
        }
        else if(query_node->operation == CONJ) {
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();

	    Evaluation(lPath, lHistory, bisimulationindex, query_node->leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, query_node->rightNode);
            Conjunction(bisimulationindex, answers, histories, lPath,rPath,lHistory, rHistory);
         }
        else if(query_node->operation == JOIN_ID){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();


            Evaluation(lPath, lHistory,bisimulationindex, query_node->leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, query_node->rightNode);


            JoinID(bisimulationindex, answers, histories, lPath, rPath, lHistory, rHistory);
            sort(answers.begin(),answers.end());
            answers.erase(std::unique(answers.begin(), answers.end()), answers.end());
        }
        else if(query_node->operation == CONJ_ID){
            vector<pair<int,int>> lPath, rPath;
            lPath.clear();
            rPath.clear();
            vector<int> lHistory,rHistory;
            lHistory.clear();
            rHistory.clear();

            Evaluation(lPath, lHistory, bisimulationindex, query_node->leftNode);
            Evaluation(rPath, rHistory, bisimulationindex, query_node->rightNode);

           ConjunctionID(bisimulationindex, answers, histories, lPath,rPath,lHistory, rHistory);
        }
    }
};


void Query::Join(BisimulationIndex& bisimulationindex, vector<pair<int,int>>& answers, vector<int>& histories, vector<pair<int,int>>& lpath, vector<pair<int,int>>& rpath, vector<int>& lhistory, vector<int>& rhistory){


    vector<pair<int,int>> l_Paths;
    vector<pair<int,int>> r_Paths;

    if(lhistory.empty()&&rhistory.empty()) {
        sort(lpath.begin(),lpath.end(), cmpdst);
        JoinCompare(answers,lpath,rpath);
    }
    else if(rhistory.empty()){

        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end(),cmpdst);
        JoinCompare(answers,l_Paths,rpath);
    }
    else if(lhistory.empty()){
        sort(lpath.begin(),lpath.end(),cmpdst);

        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(r_Paths.begin(),r_Paths.end());
        JoinCompare(answers,lpath,r_Paths);
     }
    else{

        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }

        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end(),cmpdst);
        sort(r_Paths.begin(),r_Paths.end());


        JoinCompare(answers,l_Paths,r_Paths);

    }

};


void Query::JoinCompare(vector<pair<int,int>>& answers, vector<pair<int,int>>& lpaths,vector<pair<int,int>>& rpaths){

    int for_r=0;
    int restart_r=0;
    bool restartFlag=true;
    int rsize = rpaths.size();

    if(lpaths.size()==0||rpaths.size()==0)return;
    for(int i=0;i<lpaths.size();i++){
        //cout<<i<<":"<<lpaths.size()<<endl;
        for_r=restart_r;
        restartFlag=true;
        while(1) {
             if (lpaths[i].second == rpaths[for_r].first) {
                //cout<<lpaths[i].first<<"->"<<lpaths[i].second<<","<<rpaths[for_r].first<<"->"<<rpaths[for_r].second<<endl;
                answers.push_back(make_pair(lpaths[i].first, rpaths[for_r].second));
                if(restartFlag){
                    restart_r=for_r;
                    restartFlag=false;
                }
            }
            if (lpaths[i].second < rpaths[for_r].first) {
                if(restartFlag)restart_r=for_r;
                break;
            }

            for_r++;
            if(for_r >= rsize)break;

        }
        if(for_r >= rsize&&restartFlag)break;
    }
}





void Query::JoinID(BisimulationIndex& bisimulationindex, vector<pair<int,int>>& answers, vector<int>& histories, vector<pair<int,int>>& lpath, vector<pair<int,int>>& rpath, vector<int>& lhistory, vector<int>& rhistory){


    vector<pair<int,int>> l_Paths;
    vector<pair<int,int>> r_Paths;

    if(lhistory.empty()&&rhistory.empty()) {
        sort(lpath.begin(),lpath.end(), cmpdst);


        JoinCompareID(answers,lpath,rpath);
    }
    else if(rhistory.empty()){


        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end(),cmpdst);
        JoinCompareID(answers,l_Paths,rpath);

    }
    else if(lhistory.empty()){
        sort(lpath.begin(),lpath.end(),cmpdst);

        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(r_Paths.begin(),r_Paths.end());
        JoinCompareID(answers,lpath,r_Paths);

    }
    else{


        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }

        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end(),cmpdst);
        sort(r_Paths.begin(),r_Paths.end());


        JoinCompareID(answers,l_Paths,r_Paths);
    }

};


void Query::JoinCompareID(vector<pair<int,int>>& answers, vector<pair<int,int>>& lpaths,vector<pair<int,int>>& rpaths){

    int for_r=0;
    int restart_r=0;
    bool restartFlag=true;
    int rsize = rpaths.size();

    if(lpaths.size()==0||rpaths.size()==0)return;
    for(int i=0;i<lpaths.size();i++){

        for_r=restart_r;
        restartFlag=true;
        while(1) {

            if (lpaths[i].second == rpaths[for_r].first) {
                if(lpaths[i].first == rpaths[for_r].second)answers.push_back(make_pair(lpaths[i].first, rpaths[for_r].second));

                if(restartFlag){
                    restart_r=for_r;
                    restartFlag=false;
                }
            }
            if (lpaths[i].second < rpaths[for_r].first) {
                if(restartFlag)restart_r=for_r;
                break;
            }

            for_r++;
            if(for_r >= rsize)break;

        }
        if(for_r >= rsize&&restartFlag)break;
    }
}



void Query::Conjunction(BisimulationIndex& bisimulationindex, vector<pair<int,int>>& answers, vector<int>& histories, vector<pair<int,int>>& lpath, vector<pair<int,int>>& rpath, vector<int>& lhistory, vector<int>& rhistory){

    int lcount=0,rcount=0;
    vector<pair<int,int>> l_Paths;
    vector<pair<int,int>> r_Paths;
    if(lpath.empty()&&rpath.empty()) {
        lcount=0,rcount=0;
        while(1) {
            if (lcount >= lhistory.size() || rcount >= rhistory.size())break;
            if (lhistory[lcount] == rhistory[rcount]) {
                histories.push_back(lhistory[lcount]);
                rcount++;
                lcount++;
            } else if (lhistory[lcount] < rhistory[rcount]) {
                lcount++;
            } else rcount++;


        }
    }
    else if(rpath.empty()){
        lcount=0,rcount=0;
        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(r_Paths.begin(),r_Paths.end());

        while (1) {
            if(lcount>=lpath.size()||rcount>=r_Paths.size())break;
            if(lpath[lcount] == r_Paths[rcount]){
                answers.push_back(lpath[lcount]);
                rcount++;
                lcount++;
            }
            else if(lpath[lcount].first < r_Paths[rcount].first||(lpath[lcount].first == r_Paths[rcount].first &&lpath[lcount].second < r_Paths[rcount].second)){
                lcount++;
            }
            else rcount++;

        }


    }
    else if(lpath.empty()){
        lcount=0,rcount=0;

        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end());

        while (1) {
            if(lcount>=l_Paths.size()||rcount>=rpath.size())break;
            if(l_Paths[lcount] == rpath[rcount]){
                answers.push_back(lpath[lcount]);
                rcount++;
                lcount++;
            }
            else if(l_Paths[lcount].first < rpath[rcount].first||(l_Paths[lcount].first == rpath[rcount].first &&l_Paths[lcount].second < rpath[rcount].second)){
                lcount++;
            }
            else rcount++;


        }


    }
    else{
        lcount=0,rcount=0;

        while (1) {
            if(lcount>=lpath.size()||rcount>=rpath.size())break;
            if(lpath[lcount] == rpath[rcount]){
                answers.push_back(lpath[lcount]);
                rcount++;
                lcount++;
            }
            else if(lpath[lcount].first < rpath[rcount].first||(lpath[lcount].first == rpath[rcount].first &&lpath[lcount].second < rpath[rcount].second)){
                lcount++;
            }
            else rcount++;
        }
    }


};



void Query::ConjunctionID(BisimulationIndex& bisimulationindex, vector<pair<int,int>>& answers, vector<int>& histories, vector<pair<int,int>>& lpath, vector<pair<int,int>>& rpath, vector<int>& lhistory, vector<int>& rhistory){

    int lcount=0,rcount=0;
    vector<pair<int,int>> l_Paths;
    vector<pair<int,int>> r_Paths;

    if(lpath.empty()&&rpath.empty()) {
        lcount=0,rcount=0;
        while(1) {
           if (lcount >= lhistory.size() || rcount >= rhistory.size())break;

	    if (lhistory[lcount] == rhistory[rcount]) {
              if(bisimulationindex.history2path[lhistory[lcount]].empty()){
		      }
              else if(bisimulationindex.history2path[lhistory[lcount]][0].first ==bisimulationindex.history2path[lhistory[lcount]][0].second){
                  histories.push_back(lhistory[lcount]);
              }
            rcount++;
            lcount++;
            } else if (lhistory[lcount] < rhistory[rcount]) {
                lcount++;
            } else rcount++;
        }
    }
    else if(rpath.empty()){
        lcount=0,rcount=0;
        for(int history : rhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                r_Paths.push_back(path);
            }
        }
        sort(r_Paths.begin(),r_Paths.end());

        while (1) {
            if(lcount>=lpath.size()||rcount>=r_Paths.size())break;
            if(lpath[lcount] == r_Paths[rcount]){
                if(lpath[lcount].first==lpath[lcount].second)answers.push_back(lpath[lcount]);
                rcount++;
                lcount++;
            }
            else if(lpath[lcount].first < r_Paths[rcount].first||(lpath[lcount].first == r_Paths[rcount].first &&lpath[lcount].second < r_Paths[rcount].second)){
                lcount++;
            }
            else rcount++;

        }

    }
    else if(lpath.empty()){
        lcount=0,rcount=0;

        for(int history : lhistory){
            for(pair<int,int> path: bisimulationindex.history2path[history]){
                l_Paths.push_back(path);
            }
        }
        sort(l_Paths.begin(),l_Paths.end());

        while (1) {
            if(lcount>=l_Paths.size()||rcount>=rpath.size())break;
            if(l_Paths[lcount] == rpath[rcount]){
                if(l_Paths[lcount].first==l_Paths[lcount].second)answers.push_back(l_Paths[lcount]);
                rcount++;
                lcount++;
            }
            else if(l_Paths[lcount].first < rpath[rcount].first||(l_Paths[lcount].first == rpath[rcount].first &&l_Paths[lcount].second < rpath[rcount].second)){
                lcount++;
            }
            else rcount++;


        }


    }
    else{
        lcount=0,rcount=0;

        while (1) {
            if(lcount>=lpath.size()||rcount>=rpath.size())break;
            if(lpath[lcount] == rpath[rcount]){
                if(lpath[lcount].first==lpath[lcount].second)answers.push_back(lpath[lcount]);
                rcount++;
                lcount++;
            }
            else if(lpath[lcount].first < rpath[rcount].first||(lpath[lcount].first == rpath[rcount].first &&lpath[lcount].second < rpath[rcount].second)){
                lcount++;
            }
            else rcount++;


        }

    }


};


void Query::Label2Path(vector<pair<int,int>>& answers,BisimulationIndex& bisimulationindex, vector<int>& labellist){


    int labelid = encodeLabel(labellist, bisimulationindex.labelnum,bisimulationindex.k);
    vector<int> historyset = bisimulationindex.label2history[labelid];

    for(int history : historyset){
        for(pair<int,int> path: bisimulationindex.history2path[history]){
            answers.push_back(path);
        }
    }

};
void Query::Label2PathID(vector<pair<int,int>>& answers,BisimulationIndex& bisimulationindex, vector<int>& labellist){

    int labelid = encodeLabel(labellist, bisimulationindex.labelnum,bisimulationindex.k);
    vector<int> historyset = bisimulationindex.label2history[labelid];
    for(int history : historyset){
	if(bisimulationindex.history2path[history].empty())continue;
        if(bisimulationindex.history2path[history][0].first !=bisimulationindex.history2path[history][0].second)continue;
        for(pair<int,int> path: bisimulationindex.history2path[history]){
            answers.push_back(path);
        }
    }

};


void Query::Label2History(vector<int>& histories, BisimulationIndex& bisimulationindex, vector<int>& labellist){

    int labelid = encodeLabel(labellist, bisimulationindex.labelnum,bisimulationindex.k);

    histories = bisimulationindex.label2history[labelid];

};

void Query::Label2HistoryID(vector<int>& histories, BisimulationIndex& bisimulationindex, vector<int>& labellist){

    int labelid = encodeLabel(labellist, bisimulationindex.labelnum,bisimulationindex.k);
     vector<int> historyset = bisimulationindex.label2history[labelid];
    for(int history : historyset){
        if(bisimulationindex.history2path[history][0].first !=bisimulationindex.history2path[history][0].second)continue;
        histories.push_back(history);
    }

};


