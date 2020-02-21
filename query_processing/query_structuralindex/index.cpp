//
// Created by sasaki on 19/04/18.
//

#include "index.h"

BisimulationIndex::BisimulationIndex(std::string input_file_name)
{
    string inputfile_h2p = input_file_name + "_h2p";
    string inputfile_l2h = input_file_name + "_l2h";

    std::ifstream inputh2p(inputfile_h2p);
    std::ifstream inputl2h(inputfile_l2h);

    if(!inputh2p||!inputl2h){
        std::cout<<"error: cannot open index file"<<std::endl;
        exit(1);
    }

    k=0;
    labelnum=0;
    workloadindex=false;


    inputl2h>>k>>labelnum;

    labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);


    label2history.resize(labelidsize);

    workload.resize(labelidsize);
    for(int i=0;i<labelidsize;i++)workload[i]=true;

    string v1,v2,label;
    int v1Id,v2Id,labelId,history;


    string line;
    while(std::getline(inputl2h,line)){

        std::stringstream ss(line);
        ss >> labelId;
        ss.ignore(line.size(), ' ');

       while(ss >> history)
        {
            label2history[labelId].push_back(history);
        }
    }

    int historysize;
    inputh2p>>historysize;
    history2path.resize(historysize);
    path2history.set_empty_key("-1");

    while(std::getline(inputh2p,line)){

        std::stringstream ss(line);
        ss >> history;
        ss.ignore(line.size(), ' ');
        while(ss >> v1Id >>v2Id)
        {
            history2path[history].push_back(make_pair(v1Id,v2Id));
            string s = to_string(v1Id)+"-"+to_string(v2Id);
            path2history.insert(make_pair(s,history+1));
        }
    }
}

void BisimulationIndex::SetWorkload(std::string workload_file_name){

    std::ifstream inputwork(workload_file_name);

    if(!inputwork){
        std::cout<<"error: cannot open index workload file"<<std::endl;
        exit(1);
    }

    string line, label;
    int labelsequence;

    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    for(int i=0;i<labelnum;i++)workload[i]=true;
    for(int i=labelnum;i<labelidsize;i++)workload[i]=false;

    while(std::getline(inputwork,line)){
       std::stringstream ss;
        ss<<line;
        cout<<ss.str()<<endl;
        int labelcount=0;
        while(ss >> label)
        {
             if(std::all_of(label.cbegin(),label.cend(), [](char ch) { return isdigit(ch);})) {
                 if (labelcount == 0) {
                     labelsequence = stoi(label);
                 } else {
                     labelsequence = (labelsequence + 1) * labelnum + stoi(label);
                 }
                 labelcount++;
             }
        }
        workload[labelsequence]=true;
    }
}


void BisimulationIndex::DeleteEdge(Edge dedge, OriginalGraph& graph)
{

    struct timespec maintenance_startTime, maintenance_endTime, h2p_update_startTime,h2p_update_endTime,l2h_update_startTime,l2h_update_endTime;
    double maintainTime, h2p_updateTime, l2h_updateTime;
    h2p_updateTime=0;l2h_updateTime=0;

    int v1 = dedge.src;
    int v2 = dedge.dst;
    int label = dedge.label;



    int dedgeID=-1;
    for(int i =0;i<graph.edge_list.size();i++){
        if(graph.edge_list[i] == dedge){
            dedgeID = i;
            break;
        }
    }
	if(dedgeID==-1)return;
    for(int ii =0;ii<graph.edge_list.size();ii++){
        if(graph.edge_list[ii] == dedge && ii != dedgeID){
		return;
	    }
    }
    set<Edge> pathcheck;

    vector<bool> srccheck;
    vector<bool> dstcheck;
    srccheck.resize(graph.max_vertexid);
    dstcheck.resize(graph.max_vertexid);
    for(int i=0;i<srccheck.size();i++){
        srccheck[i]=false;
        dstcheck[i]=false;
    }

    vector<vector<Edge>> deletedPathList;
    deletedPathList.resize(k);

    vector<vector<bool>> labelset_k;
    labelset_k.resize(k);
    for(int for_k=0;for_k<k;for_k++) {
        labelset_k[for_k].resize(labelnum);
        for (int i = 0; i < labelnum; i++) {
            labelset_k[for_k][i]=false;
        }
    }


    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    Edge inverteddedge(v2,v1,label+graph.number_of_labels);
    deletedPathList[0].push_back(dedge);
    deletedPathList[0].push_back(inverteddedge);

    for(int for_k=1;for_k < k;for_k++) {
        for(auto dedge_k : deletedPathList[for_k-1]) {
            for (int i = 0; i < graph.vertex_edge_list[dedge_k.dst].size(); i++) {
                int edgeid = graph.vertex_edge_list[dedge_k.dst][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                if(dedgeID != edgeid) {
                    int labelsequence = (dedge_k.label+1)*labelnum+graph.edge_list[edgeid].label;
                    Edge tempedge(dedge_k.src,graph.edge_list[edgeid].dst,labelsequence);
                    deletedPathList[for_k].push_back(tempedge);
		            dstcheck[tempedge.dst]=true;
		        }
            }
            for (int i = 0; i < graph.vertex_inverseedge_list[dedge_k.dst].size(); i++) {
                int edgeid = graph.vertex_inverseedge_list[dedge_k.dst][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                if(dedgeID != edgeid) {
                    int labelsequence = (dedge_k.label+1)*labelnum+graph.edge_list[edgeid].label+graph.number_of_labels;
                    Edge tempedge(dedge_k.src,graph.edge_list[edgeid].src,labelsequence);
                    deletedPathList[for_k].push_back(tempedge);
		            dstcheck[tempedge.dst]=true;
                }
            }

            for (int i = 0; i < graph.vertex_edge_list[dedge_k.src].size(); i++) {
                int edgeid = graph.vertex_edge_list[dedge_k.src][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                if(dedgeID != edgeid) {
                    int labelsequence = (graph.edge_list[edgeid].label+graph.number_of_labels+1)*pow((double)labelnum, (double)for_k)+dedge_k.label;
                    Edge tempedge(graph.edge_list[edgeid].dst,dedge_k.dst,labelsequence);
                    deletedPathList[for_k].push_back(tempedge);
		            dstcheck[tempedge.dst]=true;
                }
            }
            for (int i = 0; i < graph.vertex_inverseedge_list[dedge_k.src].size(); i++) {
                int edgeid = graph.vertex_inverseedge_list[dedge_k.src][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                if(dedgeID != edgeid) {
                    int labelsequence = (graph.edge_list[edgeid].label+1)*pow((double)labelnum, (double)for_k)+dedge_k.label;
                    Edge tempedge(graph.edge_list[edgeid].src,dedge_k.dst,labelsequence);
                    deletedPathList[for_k].push_back(tempedge);
		            dstcheck[tempedge.dst]=true;
                }
            }
        }
    }

    for (int for_k = 1; for_k < k; for_k++) {
        auto itr_de = deletedPathList[for_k].begin();
        while (itr_de != deletedPathList[for_k].end()) {
            if (!workload[(*itr_de).label]) {
                itr_de = deletedPathList[for_k].erase(itr_de);
            } else {
                vector<int> templabelsequence = decodeLabel((*itr_de).label,labelnum,k);
                //cout<<(*itr_de).label<<":";
                for(int i=0;i<templabelsequence.size();i++){
                    labelset_k[templabelsequence.size()-i-1][templabelsequence[i]]=true;
                    //cout<<templabelsequence[templabelsequence.size()-i]<<" ";
                }
                //cout<<endl;
                itr_de++;
            }
        }
    }


    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
	cout<<"first time: "<<maintainTime<<endl;

	clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

	for(int for_k=0;for_k< k; for_k++) {
        sort(deletedPathList[for_k].begin(), deletedPathList[for_k].end(), cmpedgelabel);
        deletedPathList[for_k].erase(std::unique(deletedPathList[for_k].begin(), deletedPathList[for_k].end()), deletedPathList[for_k].end());
    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
	cout<<"sort time: "<<maintainTime<<endl;

    vector<Edge> candidatePathList;
    vector<Edge> candidatePathList_dst;
    vector<Edge> candidatePathList_next;
    vector<vector<Edge>> candidateAllPathkList;
	candidatePathList.clear();
	candidatePathList_next.clear();
    candidatePathList_dst.clear();
	candidateAllPathkList.resize(k);

    struct timespec klength_time_start, klength_time_end;
    double initialpathklengthTime=0;
    double longpathklengthTime=0;
    double insertpathklengthTime=0;

    for(auto capl : candidateAllPathkList)capl.clear();

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);


    for(int for_k=1;for_k< k; for_k++) {
        sort(deletedPathList[for_k].begin(), deletedPathList[for_k].end(), cmpedgedst);
        deletedPathList[for_k].erase(std::unique(deletedPathList[for_k].begin(), deletedPathList[for_k].end()), deletedPathList[for_k].end());
    }

	int test=0;
    for(int for_k=1;for_k< k; for_k++) {

        cout << "dsize:" << deletedPathList[for_k].size() << endl;
        auto itr_de = deletedPathList[for_k].begin();
        while (itr_de != deletedPathList[for_k].end()) {
            auto deletedPath = (*itr_de);
            test++;
            clock_gettime(CLOCK_REALTIME, &klength_time_start);

            candidatePathList.clear();

            for (auto edgeid : graph.vertex_edge_list[deletedPath.src]) {
                if (!labelset_k[0][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                if (dedgeID != edgeid) {
                    Edge tempedge(deletedPath.src, graph.edge_list[edgeid].dst, graph.edge_list[edgeid].label);
                    candidatePathList.push_back(tempedge);
                }
            }
            for (auto edgeid : graph.vertex_inverseedge_list[deletedPath.src]) {
                if (!labelset_k[0][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                if (dedgeID != edgeid) {
                    Edge tempedge(deletedPath.src, graph.edge_list[edgeid].src, graph.edge_list[edgeid].label + graph.number_of_labels);
                    candidatePathList.push_back(tempedge);
                }
            }
            clock_gettime(CLOCK_REALTIME, &klength_time_end);
            initialpathklengthTime += (klength_time_end.tv_sec - klength_time_start.tv_sec) +
                       (klength_time_end.tv_nsec - klength_time_start.tv_nsec) * pow(10, -9);


            clock_gettime(CLOCK_REALTIME, &klength_time_start);
            int count = 1;
            while (1) { // BFS

                if (count > for_k-1)break;

                for (auto edge : candidatePathList) {
                    for (auto edgeid : graph.vertex_edge_list[edge.dst]) {
                        if (!labelset_k[count][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                        //if(count==for_k && !dstcheck[graph.edge_list[edgeid].dst])continue;
                        if (dedgeID != edgeid) {
                            int labelsequnce = (edge.label + 1) * labelnum + graph.edge_list[edgeid].label;
                            Edge tempedge(edge.src, graph.edge_list[edgeid].dst, labelsequnce);

                            candidatePathList_next.push_back(tempedge);

                        }
                    }

                    for (auto edgeid : graph.vertex_inverseedge_list[edge.dst]) {
                        if (!labelset_k[count][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                        //if(count==for_k && !dstcheck[graph.edge_list[edgeid].dst])continue;
                        if (dedgeID != edgeid) {
                            int labelsequnce = (edge.label + 1) * labelnum + graph.edge_list[edgeid].label +
                                               graph.number_of_labels;
                            Edge tempedge(edge.src, graph.edge_list[edgeid].src, labelsequnce);

                            candidatePathList_next.push_back(tempedge);

                        }
                    }
                }

                candidatePathList = candidatePathList_next;
                candidatePathList_next.clear();
                count++;

            }

            candidatePathList_dst.clear();
            for (auto edgeid : graph.vertex_edge_list[deletedPath.dst]) {
                if (!labelset_k[for_k][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                if (dedgeID != edgeid) {
                    Edge tempedge(graph.edge_list[edgeid].dst,deletedPath.dst,  graph.edge_list[edgeid].label+graph.number_of_labels);
                    candidatePathList_dst.push_back(tempedge);
                }
            }
            for (auto edgeid : graph.vertex_inverseedge_list[deletedPath.dst]) {
                if (!labelset_k[for_k][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                if (dedgeID != edgeid) {
                    Edge tempedge(graph.edge_list[edgeid].src, deletedPath.dst,  graph.edge_list[edgeid].label);
                    candidatePathList_dst.push_back(tempedge);
                }
            }

            bool flag=false;
	        bool flag2=false;
            sort(candidatePathList.begin(), candidatePathList.end(), cmpedgedst);
            sort(candidatePathList_dst.begin(), candidatePathList_dst.end(), cmpedgelabel);

            while(1) {
                flag=false;
		        flag2=false;
                pair<int,int> de = make_pair((*itr_de).src,(*itr_de).dst);
                for (auto cedge: candidatePathList) {
                    for (auto cedge_dst: candidatePathList_dst) {
                        if (cedge.dst == cedge_dst.src) {
                            //cout<<"test"<<endl;
                            if ((cedge.label + 1) * labelnum + cedge_dst.label == (*itr_de).label) {
                                itr_de = deletedPathList[for_k].erase(itr_de);
                                flag = true;
				                flag2= true;
                                break;
                            }
                        }
                        else if (cedge.dst < cedge_dst.src) {//cp is smaller than de
                            break;
                        }
                    }
		    if(flag2)break;
                }
                if (!flag)itr_de++;

                if(itr_de == deletedPathList[for_k].end())break;
                pair<int,int> de_next= make_pair((*itr_de).src,(*itr_de).dst);

                if(de != de_next)break;
            }

        }

    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) +
                       (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout << "klength path  time: " << maintainTime << "("<<initialpathklengthTime<<","<<longpathklengthTime<<","<<insertpathklengthTime<< ")"<<endl;
    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

    for(int for_k=1;for_k< k; for_k++) {
        deletedPathList[for_k].erase(std::unique(deletedPathList[for_k].begin(), deletedPathList[for_k].end()), deletedPathList[for_k].end());
    }


    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"candidate delete time: "<<maintainTime<<endl;
    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

    vector<Edge> deletedPathListAll;
    for(int i =0;i<k;i++){
        for(int j =0;j<deletedPathList[i].size();j++) {
            deletedPathListAll.push_back(deletedPathList[i][j]);
        }
    }
    sort(deletedPathListAll.begin(), deletedPathListAll.end(), cmpedgelabel);
    cout<<"deletedPathlist size = "<<deletedPathListAll.size()<<endl;
    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

    vector<EdgeWithLabelset> deletedPathWLabelset;
  	deletedPathWLabelset.clear();
    int previousSrc=-1;
    int previousDst=-1;
    for(auto dPath : deletedPathListAll){

        if(dPath.src == previousSrc &&dPath.dst == previousDst){
            deletedPathWLabelset[deletedPathWLabelset.size()-1].labelset.push_back(dPath.label);
        }
        else{
            EdgeWithLabelset tempedge_labelset(dPath.src,dPath.dst);
            tempedge_labelset.labelset.push_back(dPath.label);
            deletedPathWLabelset.push_back(tempedge_labelset);
            previousSrc = dPath.src;
            previousDst = dPath.dst;
        }
    }

    int history_of_dpath;
    bool breakflag=false;
    vector<int> setNewHistories;
    setNewHistories.clear();

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"label merge time: "<<maintainTime<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    cout<<"delete path size : "<<deletedPathWLabelset.size()<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    //cout<<"path merge time: "<<maintainTime<<"(pathhisotryset = "<<pathhistoryset.size()<<", addedPathWLabelset ="<<deletedPathWLabelset.size()<<")"<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);


    for(auto dPath : deletedPathWLabelset){

        clock_gettime(CLOCK_REALTIME, &h2p_update_startTime);
        breakflag=false;

        string s = to_string(dPath.src)+"-"+to_string(dPath.dst);

        int for_h = path2history[s]-1;
        if(history2path[for_h].empty())continue;
        if(history2path[for_h][history2path[for_h].size()-1].first < dPath.src || (history2path[for_h][history2path[for_h].size()-1].first == dPath.src && history2path[for_h][history2path[for_h].size()-1].second < dPath.dst))continue;

        int left=0;
        int right = history2path[for_h].size()-1;
        int for_path;
        while(left<=right){
            for_path=(left+right)/2;
            if(history2path[for_h][for_path].first == dPath.src && history2path[for_h][for_path].second == dPath.dst){
                history2path[for_h].erase(history2path[for_h].begin()+for_path);
                history_of_dpath = for_h;
                //cout<<history_of_dpath<<",,,"<< path2history[s]-1<<endl;
                breakflag=true;
                break;
            }
            else if(history2path[for_h][for_path].first < dPath.src || (history2path[for_h][for_path].first == dPath.src && history2path[for_h][for_path].second < dPath.dst)){
                left = for_path+1;
            }
            else {
                right = for_path-1;
            }
        }





        clock_gettime(CLOCK_REALTIME, &h2p_update_endTime);
        h2p_updateTime += (h2p_update_endTime.tv_sec - h2p_update_startTime.tv_sec) + (h2p_update_endTime.tv_nsec - h2p_update_startTime.tv_nsec) * pow(10, -9);

        clock_gettime(CLOCK_REALTIME, &l2h_update_startTime);
        int newhistory = history2path.size();
        bool insertFlag = false;

        for(int for_l =0;for_l<label2history.size(); for_l++){
            if(label2history[for_l].empty())continue;
            if(label2history[for_l][label2history[for_l].size() -1]<history_of_dpath)continue;

            int left=0;
            int right = label2history[for_l].size()-1;

            int for_h;
            while(left<=right){
                for_h=(left+right)/2;
                if(label2history[for_l][for_h]==history_of_dpath){

                    bool sameLabelFlag=false;
                    for(int i=0;i<dPath.labelset.size();i++){
                        if(dPath.labelset[i]==for_l){sameLabelFlag=true;break;}
                    }
                    if(!sameLabelFlag){
                        label2history[for_l].push_back(newhistory);
                        insertFlag=true;
                    }
                    break;
                    }
                    else if(label2history[for_l][for_h]<history_of_dpath){
                    left = for_h+1;
                }
                else {
                    right = for_h-1;
                }
            }
        }

        if(insertFlag){
            vector<pair<int,int>> historyset ={make_pair(dPath.src,dPath.dst)};
            history2path.push_back(historyset);
            path2history[s]=history2path.size();
        }
        clock_gettime(CLOCK_REALTIME, &l2h_update_endTime);
        l2h_updateTime += (l2h_update_endTime.tv_sec - l2h_update_startTime.tv_sec) + (l2h_update_endTime.tv_nsec - l2h_update_startTime.tv_nsec) * pow(10, -9);

    }


    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"index update time: "<<maintainTime<<"(h2p:"<<h2p_updateTime<<", l2h:"<<l2h_updateTime<<")"<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    graph.edge_list[dedgeID].src=-1;
    graph.edge_list[dedgeID].dst=-1;
    graph.edge_list[dedgeID].label=-1;
    for(int i = 0;i < graph.vertex_edge_list.size();i++){
        for(int j = 0; j< graph.vertex_edge_list[i].size();j++){
            if(graph.vertex_edge_list[i][j]==dedgeID){
                graph.vertex_edge_list[i].erase(graph.vertex_edge_list[i].begin()+j);
                break;
            }
        }
    }
    for(int i = 0;i < graph.vertex_inverseedge_list.size();i++){
        for(int j = 0; j< graph.vertex_inverseedge_list[i].size();j++){
            if(graph.vertex_inverseedge_list[i][j]==dedgeID){
                graph.vertex_inverseedge_list[i].erase(graph.vertex_inverseedge_list[i].begin()+j);
                break;
           }
        }
    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"graph update time: "<<maintainTime<<endl;
}




void BisimulationIndex::AddEdge(Edge aedge, OriginalGraph& graph)
{

    struct timespec maintenance_startTime, maintenance_endTime, h2p_update_startTime,h2p_update_endTime,l2h_update_startTime,l2h_update_endTime;
    double maintainTime, h2p_updateTime, l2h_updateTime;
    h2p_updateTime=0;l2h_updateTime=0;
    int v1 = aedge.src;
    int v2 = aedge.dst;
    int label = aedge.label;

    vector<vector<Edge>> addedPathList;
    addedPathList.resize(k);



    int aedgeID=-1;
    for(int i =0;i<graph.edge_list.size();i++){
        if(graph.edge_list[i] == aedge){
            aedgeID = i;
            break;
        }
    }
	if(aedgeID!=-1)return;

    set<Edge> pathcheck;

    vector<bool> srccheck;
    vector<bool> dstcheck;
    srccheck.resize(graph.max_vertexid);
    dstcheck.resize(graph.max_vertexid);
    for(int i=0;i<srccheck.size();i++){
        srccheck[i]=false;
        dstcheck[i]=false;
    }

    vector<vector<bool>> labelset_k;
    labelset_k.resize(k);
    for(int for_k=0;for_k<k;for_k++) {
        labelset_k[for_k].resize(labelidsize);
        for (int i = 0; i < labelidsize; i++) {
            labelset_k[for_k][i]=false;
        }
    }

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    Edge inverteddedge(v2,v1,label+graph.number_of_labels);
    addedPathList[0].push_back(aedge);
    addedPathList[0].push_back(inverteddedge);

    for(int for_k=1;for_k < k;for_k++) {
        for(auto aedge_k : addedPathList[for_k-1]) {
            for (int i = 0; i < graph.vertex_edge_list[aedge_k.dst].size(); i++) {
                int edgeid = graph.vertex_edge_list[aedge_k.dst][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                int labelsequence = (aedge_k.label+1)*labelnum+graph.edge_list[edgeid].label;
                Edge tempedge(aedge_k.src,graph.edge_list[edgeid].dst,labelsequence);
                addedPathList[for_k].push_back(tempedge);
		        dstcheck[tempedge.dst]=true;
            }
            for (int i = 0; i < graph.vertex_inverseedge_list[aedge_k.dst].size(); i++) {
                int edgeid = graph.vertex_inverseedge_list[aedge_k.dst][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                int labelsequence = (aedge_k.label+1)*labelnum+graph.edge_list[edgeid].label+graph.number_of_labels;
                Edge tempedge(aedge_k.src,graph.edge_list[edgeid].src,labelsequence);
                addedPathList[for_k].push_back(tempedge);
                dstcheck[tempedge.dst]=true;

            }
            for (int i = 0; i < graph.vertex_edge_list[aedge_k.src].size(); i++) {
                int edgeid = graph.vertex_edge_list[aedge_k.src][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                int labelsequence = (graph.edge_list[edgeid].label+graph.number_of_labels+1)*pow((double)labelnum, (double)for_k)+aedge_k.label;
                Edge tempedge(graph.edge_list[edgeid].dst,aedge_k.dst,labelsequence);
                addedPathList[for_k].push_back(tempedge);
		        dstcheck[tempedge.dst]=true;
            }
            for (int i = 0; i < graph.vertex_inverseedge_list[aedge_k.src].size(); i++) {
                int edgeid = graph.vertex_inverseedge_list[aedge_k.src][i];
                if(graph.edge_list[edgeid].label==-1)continue;
                int labelsequence = (graph.edge_list[edgeid].label+1)*pow((double)labelnum, (double)for_k)+aedge_k.label;
                Edge tempedge(graph.edge_list[edgeid].src,aedge_k.dst,labelsequence);
                addedPathList[for_k].push_back(tempedge);
		        dstcheck[tempedge.dst]=true;
            }
        }
    }

    for (int for_k = 1; for_k < k; for_k++) {
        auto itr_de = addedPathList[for_k].begin();
        while (itr_de != addedPathList[for_k].end()) {
            if (!workload[(*itr_de).label]) {
                itr_de = addedPathList[for_k].erase(itr_de);
            } else {
                vector<int> templabelsequence = decodeLabel((*itr_de).label,labelnum,k);
                for(int i=0;i<templabelsequence.size();i++){
                    labelset_k[templabelsequence.size()-i-1][templabelsequence[i]]=true;
                }
                itr_de++;
            }
        }
    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
	cout<<"first time: "<<maintainTime<<endl;

	clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

    for(int for_k=0;for_k< k; for_k++) {
        sort(addedPathList[for_k].begin(), addedPathList[for_k].end(), cmpedgelabel);
        addedPathList[for_k].erase(std::unique(addedPathList[for_k].begin(), addedPathList[for_k].end()), addedPathList[for_k].end());
    }
    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
	cout<<"sort time: "<<maintainTime<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    vector<Edge> candidatePathList;
    vector<Edge> candidatePathList_dst;
    vector<Edge> candidatePathList_next;
    vector<vector<Edge>> candidateAllPathkList;
	candidatePathList.clear();
	candidatePathList_next.clear();
    candidatePathList_dst.clear();
	candidateAllPathkList.resize(k);

    struct timespec klength_time_start, klength_time_end;
    double initialpathklengthTime=0;


    for(int for_k=1;for_k< k; for_k++) {
        sort(addedPathList[for_k].begin(), addedPathList[for_k].end(), cmpedgedst);
        addedPathList[for_k].erase(std::unique(addedPathList[for_k].begin(), addedPathList[for_k].end()), addedPathList[for_k].end());
    }


    for(int for_k=1;for_k< k; for_k++) {

        cout << "asize:" << addedPathList[for_k].size() << endl;
        auto itr_de = addedPathList[for_k].begin();
        while (itr_de != addedPathList[for_k].end()) {

            auto deletedPath = (*itr_de);
            clock_gettime(CLOCK_REALTIME, &klength_time_start);

            candidatePathList.clear();

            for (auto edgeid : graph.vertex_edge_list[deletedPath.src]) {
                if (!labelset_k[0][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                if (aedgeID != edgeid) {
                    Edge tempedge(deletedPath.src, graph.edge_list[edgeid].dst, graph.edge_list[edgeid].label);
                    candidatePathList.push_back(tempedge);
                }
            }
            for (auto edgeid : graph.vertex_inverseedge_list[deletedPath.src]) {
                if (!labelset_k[0][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                if (aedgeID != edgeid) {
                    Edge tempedge(deletedPath.src, graph.edge_list[edgeid].src, graph.edge_list[edgeid].label + graph.number_of_labels);
                    candidatePathList.push_back(tempedge);
                }
            }
            clock_gettime(CLOCK_REALTIME, &klength_time_end);
            initialpathklengthTime += (klength_time_end.tv_sec - klength_time_start.tv_sec) +
                       (klength_time_end.tv_nsec - klength_time_start.tv_nsec) * pow(10, -9);


            clock_gettime(CLOCK_REALTIME, &klength_time_start);

            int count = 1;
            while (1) { // BFS

                if (count > for_k-1)break;

                for (auto edge : candidatePathList) {
                    for (auto edgeid : graph.vertex_edge_list[edge.dst]) {
                        if (!labelset_k[count][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                        //if(count==for_k && !dstcheck[graph.edge_list[edgeid].dst])continue;
                        if (aedgeID != edgeid) {
                            int labelsequnce = (edge.label + 1) * labelnum + graph.edge_list[edgeid].label;
                            Edge tempedge(edge.src, graph.edge_list[edgeid].dst, labelsequnce);

                            candidatePathList_next.push_back(tempedge);

                        }
                    }

                    for (auto edgeid : graph.vertex_inverseedge_list[edge.dst]) {
                        if (!labelset_k[count][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                        //if(count==for_k && !dstcheck[graph.edge_list[edgeid].dst])continue;
                        if (aedgeID != edgeid) {
                            int labelsequnce = (edge.label + 1) * labelnum + graph.edge_list[edgeid].label +
                                               graph.number_of_labels;
                            Edge tempedge(edge.src, graph.edge_list[edgeid].src, labelsequnce);

                            candidatePathList_next.push_back(tempedge);

                        }
                    }
                }

                candidatePathList = candidatePathList_next;
                candidatePathList_next.clear();
                count++;

            }

            candidatePathList_dst.clear();
            for (auto edgeid : graph.vertex_edge_list[deletedPath.dst]) {
                if (!labelset_k[for_k][graph.edge_list[edgeid].label+graph.number_of_labels]||graph.edge_list[edgeid].label == -1)continue;
                if (aedgeID != edgeid) {
                    Edge tempedge(graph.edge_list[edgeid].dst,deletedPath.dst,  graph.edge_list[edgeid].label+graph.number_of_labels);
                    candidatePathList_dst.push_back(tempedge);
                }
            }
            for (auto edgeid : graph.vertex_inverseedge_list[deletedPath.dst]) {
                if (!labelset_k[for_k][graph.edge_list[edgeid].label]||graph.edge_list[edgeid].label == -1)continue;
                if (aedgeID != edgeid) {
                    Edge tempedge(graph.edge_list[edgeid].src, deletedPath.dst,  graph.edge_list[edgeid].label);
                    candidatePathList_dst.push_back(tempedge);
                }
            }

            bool flag=false;
	        bool flag2=false;
            sort(candidatePathList.begin(), candidatePathList.end(), cmpedgedst);
            sort(candidatePathList_dst.begin(), candidatePathList_dst.end(), cmpedgelabel);

            while(1) {
                flag=false;flag2=false;
                pair<int,int> de = make_pair((*itr_de).src,(*itr_de).dst);
                for (auto cedge: candidatePathList) {
                    for (auto cedge_dst: candidatePathList_dst) {
                        if (cedge.dst == cedge_dst.src) {
                            //cout<<"test"<<endl;
                            if ((cedge.label + 1) * labelnum + cedge_dst.label == (*itr_de).label) {
                                itr_de = addedPathList[for_k].erase(itr_de);
                                flag = true;flag2=true;
                                break;
                            }
                        }
                        else if (cedge.dst < cedge_dst.src) {//cp is smaller than de
                            break;
                        }
                    }
			if(flag2)break;
                }
                if (!flag)itr_de++;

                if(itr_de == addedPathList[for_k].end())break;
                pair<int,int> de_next= make_pair((*itr_de).src,(*itr_de).dst);

                if(de != de_next)break;
            }

        }

    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) +
                       (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout << "klength path  time: " << maintainTime << endl;
    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);


    vector<Edge> addedPathListAll;
    for(int i =0;i<k;i++){
        for(int j =0;j<addedPathList[i].size();j++) {
            addedPathListAll.push_back(addedPathList[i][j]);
        }
    }
    sort(addedPathListAll.begin(), addedPathListAll.end(), cmpedgelabel);

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

    vector<EdgeWithLabelset> addedPathWLabelset;
    int previousSrc=-1;
    int previousDst=-1;
    for(auto dPath : addedPathListAll){

        if(dPath.src == previousSrc &&dPath.dst == previousDst){
            addedPathWLabelset[addedPathWLabelset.size()-1].labelset.push_back(dPath.label);
        }
        else{
            EdgeWithLabelset tempedge_labelset(dPath.src,dPath.dst);
            tempedge_labelset.labelset.push_back(dPath.label);
            addedPathWLabelset.push_back(tempedge_labelset);
            previousSrc = dPath.src;
            previousDst = dPath.dst;
        }
    }
    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"label merge time: "<<maintainTime<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    int history_of_dpath;
    bool breakflag=false;
    cout<<"insert path size : "<<addedPathWLabelset.size()<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    //cout<<"path merge time: "<<maintainTime<<"(pathhisotryset = "<<pathhistoryset.size()<<", addedPathWLabelset ="<<addedPathWLabelset.size()<<")"<<endl;
    cout<<"path merge time: "<<maintainTime<<", addedPathWLabelset ="<<addedPathWLabelset.size()<<")"<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);
    sort(addedPathWLabelset.begin(), addedPathWLabelset.end(), cmpedgewithlabelse);

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"sort time: "<<maintainTime<<endl;

    clock_gettime(CLOCK_REALTIME, &maintenance_startTime);


    for(auto aPath : addedPathWLabelset){
        clock_gettime(CLOCK_REALTIME, &h2p_update_startTime);
        breakflag=false;

        string s = to_string(aPath.src)+"-"+to_string(aPath.dst);
        int for_h = path2history[s]-1;

        if(for_h!=-1){
            int left = 0;
            int right = history2path[for_h].size() - 1;
             int for_path;
            while (left <= right) {
                for_path = (left + right) / 2;
                if (history2path[for_h][for_path].first == aPath.src &&
                    history2path[for_h][for_path].second == aPath.dst) {
                    history2path[for_h].erase(history2path[for_h].begin() + for_path);

                    breakflag = true;
                    break;
                } else if (history2path[for_h][for_path].first < aPath.src ||
                           (history2path[for_h][for_path].first == aPath.src &&
                            history2path[for_h][for_path].second < aPath.dst)) {
                    left = for_path + 1;
                } else {
                    right = for_path - 1;
                }
            }
        }

        clock_gettime(CLOCK_REALTIME, &h2p_update_endTime);
        h2p_updateTime += (h2p_update_endTime.tv_sec - h2p_update_startTime.tv_sec) + (h2p_update_endTime.tv_nsec - h2p_update_startTime.tv_nsec) * pow(10, -9);

        clock_gettime(CLOCK_REALTIME, &l2h_update_startTime);

        if(for_h!=-1){
            history_of_dpath=for_h;

            int newhistory = history2path.size();

            for(int i=0;i<aPath.labelset.size();i++){
                label2history[aPath.labelset[i]].push_back(newhistory);
            }
            for (int for_l = 0; for_l < label2history.size(); for_l++) {
                if(label2history[for_l].empty())continue;
                if(label2history[for_l][label2history[for_l].size() -1]<history_of_dpath)continue;

                int left=0;
                int right = label2history[for_l].size()-1;
                //cout<<left<<"l,r"<<right<<endl;
                int for_h;
                while(left<=right){
                    for_h=(left+right)/2;
                    if(label2history[for_l][for_h]==history_of_dpath){

                        bool sameLabelFlag=false;
                        for(int i=0;i<aPath.labelset.size();i++){
                            if(aPath.labelset[i]==for_l){sameLabelFlag=true;break;}
                        }
                        if(!sameLabelFlag){// already inserted
                            label2history[for_l].push_back(newhistory);
                         }
                        break;
                    }
                    else if(label2history[for_l][for_h]<history_of_dpath){
                        left = for_h+1;
                    }
                    else {
                        right = for_h-1;
                    }
                }
            }
            vector<pair<int, int>> historyset = {make_pair(aPath.src, aPath.dst)};
            history2path.push_back(historyset);
            path2history[s]=history2path.size();
        }
        else{ //if there are no same paths
            int newhistory = history2path.size();

            for(auto for_label : aPath.labelset){
                label2history[for_label].push_back(newhistory);
            }

            vector<pair<int, int>> historyset = {make_pair(aPath.src, aPath.dst)};
            history2path.push_back(historyset);
            path2history.insert(make_pair(s,history2path.size()));
        }
        //segcount++;
        clock_gettime(CLOCK_REALTIME, &l2h_update_endTime);
        l2h_updateTime += (l2h_update_endTime.tv_sec - l2h_update_startTime.tv_sec) + (l2h_update_endTime.tv_nsec - l2h_update_startTime.tv_nsec) * pow(10, -9);
    }

    clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
    maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
    cout<<"index update time: "<<maintainTime<<"(h2p:"<<h2p_updateTime<<", l2h:"<<l2h_updateTime<<")"<<endl;

    Edge edge(v1, v2, label);
    if(v1>graph.max_vertexid)graph.max_vertexid=v1;
    if(v2>graph.max_vertexid)graph.max_vertexid=v2;
    graph.edge_list.push_back(edge);
    graph.vertex_edge_list[v1].push_back(graph.edge_list.size()-1);
    graph.vertex_inverseedge_list[v2].push_back(graph.edge_list.size()-1);
}





void BisimulationIndex::DeleteWorkload(vector<int> labelsequences_, OriginalGraph& graph_) {
    int labelid = encodeLabel(labelsequences_, labelnum, k);
    workload[labelid] = false;
    label2history[labelid].clear();
}

void BisimulationIndex::AddWorkload(vector<int> labelsequence_, OriginalGraph& graph_) {

    struct timespec h2p_update_startTime,h2p_update_endTime,l2h_update_startTime,l2h_update_endTime;
    double maintainTime, h2p_updateTime, l2h_updateTime;
    h2p_updateTime=0;
    l2h_updateTime=0;

    int labelid = encodeLabel(labelsequence_, labelnum, k);
    if(workload[labelid])return;
    workload[labelid]=true;

    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    label2history.resize(labelidsize);
    vector<int> labelset;

    int label=labelsequence_[0];
    labelset.push_back(label);
    vector<bool>labelset_k;
    labelset_k.resize(labelidsize);

    for(int i=1;i<labelsequence_.size();i++){
        label = (label+1)*labelnum+labelsequence_[i];
        labelset.push_back(label);
        labelset_k[label]=true;
    }

    vector<PathwithIDs*> sortpath; // for sort


    vector<PathwithIDs> pathset;
    PathwithIDs tempPath;
    Segment tempSegment;

    vector<PathwithIDs> newPathSet;
    vector<google::dense_hash_map< int, int>> forNewPathlist;

    forNewPathlist.resize(graph_.number_of_vertices);
    for(int i=0;i<graph_.number_of_vertices;i++) {
        forNewPathlist[i].set_empty_key(-1);
    }
    Path pathkey;

    vector<google::dense_hash_map< int, int>> pathlist;
    vector<google::dense_hash_map< int, int>> pathlist0;
    pathkey.src=-1;
    pathkey.dst=-1;
    pathlist.resize(graph_.number_of_vertices);
    pathlist0.resize(graph_.number_of_vertices);
    vector<int> tempvector={-1};
    for(int i=0;i<graph_.number_of_vertices;i++) {
        pathlist[i].set_empty_key(-1);
        pathlist0[i].set_empty_key(-1);
    }
    vector<vector<vector<pair<int,int>>>> pathlist_k; // for efficient join
    pathlist_k.resize(k);
    for(int i=0;i<k;i++)pathlist_k[i].resize(graph_.number_of_edges);

    vector<vector<Segment>> seg;
    seg.resize(k);
    vector<google::dense_hash_map< Path,int,pathhash,patheqr>> path2segment;
    vector<int> numberofsegid;

    path2segment.resize(k);
    numberofsegid.resize(k);


    for(auto edge : graph_.edge_list){

        int pathid;
        for(int i=0;i<labelsequence_.size();i++){
            Segment tempSegment;
            if(labelsequence_[i]==edge.label){

               pathkey.src = edge.src;pathkey.dst=edge.dst;
               pathid = pathlist0[pathkey.src][pathkey.dst];
               tempSegment.clear();
               if (pathid == 0) {
                    tempSegment.path = pathkey;
                    tempSegment.labelsequence.insert(edge.label);
                    seg[0].push_back(tempSegment);
                    pathlist0[pathkey.src][pathkey.dst] = seg[0].size();
               } else {
                    seg[0][pathid - 1].labelsequence.insert(edge.label);
               }
            }
            else if(labelsequence_[i]==edge.label+graph_.number_of_labels){
                pathkey.src = edge.dst;pathkey.dst=edge.src;
                tempSegment.clear();
                pathid = pathlist[pathkey.src][pathkey.dst];
                if (pathid == 0) {
                    tempSegment.path = pathkey;
                    tempSegment.labelsequence.insert(edge.label + graph_.number_of_labels);
                    seg[0].push_back(tempSegment);
                    pathlist0[pathkey.src][pathkey.dst] = seg[0].size();
                } else {
                    seg[0][pathid - 1].labelsequence.insert(edge.label + graph_.number_of_labels);
                }
            }

        }

    }

    vector<vector<Segment>> seg_prefix;
    seg_prefix.resize(k);
    for(int i=0;i<k;i++){//for efficient join
        for(auto segment : seg[0]){
            for(auto label: segment.labelsequence) {
                if(labelsequence_[i]==label) {
                    pathlist_k[i][segment.path.src].push_back(make_pair(segment.path.dst, pathlist0[segment.path.src][segment.path.dst]-1));
                    if(i==0){
                        seg_prefix[0].push_back(segment);
                    }
                    break;
                }
            }
        }
    }



    for(int i=0;i<graph_.number_of_vertices;i++) {
        pathlist[i].clear();
    }
    pathset.clear();


     int count_enumeration=0;

    vector<Segment> segmentVectorSameSrc;
    tempSegment.clear();
    segmentVectorSameSrc.clear();


    for(int for_k = 1;for_k<k;for_k++) {// for all k length path

        count_enumeration=0;
        cout<<"length" <<for_k<< " size = "<<seg[for_k - 1].size()<<": start"<<endl;

        if(for_k==1) {
            for (Segment segment : seg_prefix[0]) {
                count_enumeration++;
                Path pathl;
                pathl.src = segment.path.src;
                pathl.dst = segment.path.dst;
                int id_path0;

                for (auto endVertex : pathlist_k[for_k][segment.path.dst]) {
                    pathkey.src = segment.path.src;
                    pathkey.dst = endVertex.first;

                    id_path0 = endVertex.second;
                    Segment seg0 = seg[0][id_path0];

                    int pathid = pathlist[pathkey.src][pathkey.dst];

                    if (pathid == 0) {
                        tempSegment.clear();
                        tempSegment.path = pathkey;

                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * labelnum + label_0;
                                if (labelset_k[edgelabel])tempSegment.labelsequence.insert(edgelabel);
                            }
                        }
                        if(!tempSegment.labelsequence.empty()){
                            seg[for_k].push_back(tempSegment);
                            pathlist[pathkey.src][pathkey.dst]=seg[for_k].size();
                        }
                    }
                    else{
                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * labelnum + label_0;
                                if (labelset_k[edgelabel])seg[for_k][pathid-1].labelsequence.insert(edgelabel);
                            }
                        }
                    }
                }
            }
        }
        else{
            for (Segment segment : seg[for_k]) {
                count_enumeration++;
                Path pathl;
                pathl.src = segment.path.src;
                pathl.dst = segment.path.dst;
                int id_path0;

                for (auto endVertex : pathlist_k[for_k][segment.path.dst]) {
                    pathkey.src = segment.path.src;
                    pathkey.dst = endVertex.first;

                    id_path0 = endVertex.second;
                    Segment seg0 = seg[0][id_path0];

                    int pathid = pathlist[pathkey.src][pathkey.dst];

                    if (pathid == 0) {
                        tempSegment.clear();
                        tempSegment.path = pathkey;

                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * labelnum + label_0;
                                if (labelset_k[edgelabel])tempSegment.labelsequence.insert(edgelabel);
                            }
                        }
                        if(!tempSegment.labelsequence.empty()){
                            seg[for_k].push_back(tempSegment);
                            pathlist[pathkey.src][pathkey.dst]=seg[for_k].size();
                        }
                    }
                    else{
                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * labelnum + label_0;
                                if (labelset_k[edgelabel])seg[for_k][pathid-1].labelsequence.insert(edgelabel);
                            }
                        }
                    }
                }
            }

        }

    }//for_k

    cout<<"enumeration DONE"<<endl;
    cout<<"length size = "<<seg[k - 1].size()<<endl;

    int history_of_dpath;
    int pathcount=0;

    vector<PathHistory> pathhistoryset;
    PathHistory pathhistory;
    for(int for_h=0;for_h < history2path.size();for_h++) {
        pathhistory.history=for_h;
        for(int for_path=0; for_path < history2path[for_h].size();for_path++){
            Path path;
            path.src=history2path[for_h][for_path].first;
            path.dst=history2path[for_h][for_path].second;

            pathhistory.path = path;
            pathhistoryset.push_back(pathhistory);
        }
    }
    sort(pathhistoryset.begin(), pathhistoryset.end(), cmppathhistorypath);
    sort(seg[k-1].begin(), seg[k-1].end(), cmpsegmentpath);

    vector<int> pathhistoryid;
    pathhistoryid.resize(seg[k-1].size());

    int segcount=0;
    int pathhistorycount=0;
    for(auto segment : seg[k-1]){
        Path aPath = segment.path;
        while(1){

            if(pathhistoryset[pathhistorycount].path == aPath){
                pathhistoryid[segcount] = pathhistoryset[pathhistorycount].history;
                pathhistorycount++;
                pathcount++;
                break;
            }
            else if(pathhistoryset[pathhistorycount].path.src > aPath.src || (pathhistoryset[pathhistorycount].path.src == aPath.src && pathhistoryset[pathhistorycount].path.dst > aPath.dst)){
                pathhistoryid[segcount] = -1;
                break;
            }
            else{
                pathhistorycount++;
            }

        }
        segcount++;
    }

    cout<<"historycheck DONE"<<endl;
    cout<<"history included path = "<<pathcount<<endl;
    segcount=0;
    pathcount=0;
    for(auto segment : seg[k-1]){
        pathcount++;
//
        Path aPath = segment.path;


        clock_gettime(CLOCK_REALTIME, &h2p_update_startTime);
        if(pathhistoryid[segcount]!= -1) {
            int for_h = pathhistoryid[segcount];

            int left = 0;
            int right = history2path[for_h].size() - 1;
            int for_path;
            while (left <= right) {
                for_path = (left + right) / 2;
                if (history2path[for_h][for_path].first == aPath.src &&
                    history2path[for_h][for_path].second == aPath.dst) {
                    history2path[for_h].erase(history2path[for_h].begin() + for_path);

                    break;
                } else if (history2path[for_h][for_path].first < aPath.src ||
                           (history2path[for_h][for_path].first == aPath.src &&
                            history2path[for_h][for_path].second < aPath.dst)) {
                    left = for_path + 1;
                } else {
                    right = for_path - 1;
                }
            }

        }

        clock_gettime(CLOCK_REALTIME, &h2p_update_endTime);
        h2p_updateTime += (h2p_update_endTime.tv_sec - h2p_update_startTime.tv_sec) + (h2p_update_endTime.tv_nsec - h2p_update_startTime.tv_nsec) * pow(10, -9);
        clock_gettime(CLOCK_REALTIME, &l2h_update_startTime);
        if(pathhistoryid[segcount]!= -1) {// if there is  same paths

            history_of_dpath=pathhistoryid[segcount];
            int newhistory = history2path.size();

            label2history[labelid].push_back(newhistory);
            //}
            for (int for_l = 0; for_l < label2history.size(); for_l++) {
                if(label2history[for_l].empty())continue;
                if(label2history[for_l][label2history[for_l].size() -1]<history_of_dpath)continue;

                int left=0;
                int right = label2history[for_l].size()-1;
                int for_h;
                while(left<=right){
                    for_h=(left+right)/2;
                    if(label2history[for_l][for_h]==history_of_dpath){
                        label2history[for_l].push_back(newhistory);
                        break;
                    }
                    else if(label2history[for_l][for_h]<history_of_dpath){
                        left = for_h+1;
                    }
                    else {
                        right = for_h-1;
                    }
                }
            }
            vector<pair<int, int>> historyset = {make_pair(aPath.src, aPath.dst)};
            history2path.push_back(historyset);
        }
        else { //if there are no same paths

            label2history[labelid].push_back(history2path.size());
            vector<pair<int, int>> historyset = {make_pair(aPath.src, aPath.dst)};
            history2path.push_back(historyset);
        }

        clock_gettime(CLOCK_REALTIME, &l2h_update_endTime);
        l2h_updateTime += (l2h_update_endTime.tv_sec - l2h_update_startTime.tv_sec) + (l2h_update_endTime.tv_nsec - l2h_update_startTime.tv_nsec) * pow(10, -9);

        segcount++;
    }


}



void BisimulationIndex::Output(string indexfilename){


    string outputL2H = indexfilename+"_workload_l2h";
    string outputH2P = indexfilename+"_workload_h2p";
    std::ofstream fout_l2h(outputL2H, ios::app);
    std::ofstream fout_h2p(outputH2P, ios::app);


    cout<<"output label2history"<<endl;

    fout_l2h<<k<<" "<<labelnum<<endl;
    for(int i=0; i<label2history.size();i++) {

        if(label2history[i].empty())continue;
        fout_l2h<<i<<" ";
        for(auto history : label2history[i]) {
          fout_l2h<<history<<" ";
        }
        fout_l2h<<endl;
    }
    cout<<"output history2path"<<endl;
    fout_h2p<<history2path.size()<<endl;
    for(int i=0; i<history2path.size();i++) {

        if(history2path[i].empty())continue;

        fout_h2p<<i<<" ";
        for(auto path : history2path[i]) {
           fout_h2p << path.first << " " << path.second <<" ";
        }
        fout_h2p<<endl;
    }


    int labelid=0;
    for(auto l2h : label2history){
        cout<<labelid<<" "<<l2h.size()<<endl;
        labelid++;
    }


}
