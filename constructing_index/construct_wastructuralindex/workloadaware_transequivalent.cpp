//
// Created by sasaki on 18/12/06.
//

#include "workloadaware_transequivalent.h"
#include "graph.h"



WorkloadawareTransequivalent::WorkloadawareTransequivalent(int k_ , OriginalGraph graph_,WorkLoad workload) {


    struct timespec enumerate_startTime, enumerate_endTime, sort_startTime, sort_endTime, id_startTime, id_endTime;

    k=k_;
    number_of_labels = graph_.number_of_labels*2;
    number_of_vertices=graph_.number_of_vertices;
    number_of_edges=graph_.number_of_edges;

    seg.resize(k);

    Path pathkey;


    vector<google::dense_hash_map< int, int>> pathlist;
    vector<google::dense_hash_map< int, int>> pathlist0;
    pathkey.src=-1;
    pathkey.dst=-1;
    pathlist.resize(number_of_vertices);
    pathlist0.resize(number_of_vertices);
    vector<int> tempvector={-1};
    for(int i=0;i<number_of_vertices;i++) {
        pathlist[i].set_empty_key(-1);
        pathlist0[i].set_empty_key(-1);
    }

    vector<vector<vector<pair<int,int>>>> pathlist_k; // for efficient join
    pathlist_k.resize(k);
    for(int i=0;i<k;i++)pathlist_k[i].resize(graph_.number_of_edges);
    int count_enumeration=0;
    clock_gettime(CLOCK_REALTIME, &enumerate_startTime);
    vector<vector<Segment>> seg_prefix;
    seg_prefix.resize(k);

    for(int i=0;i<k;i++)seg_prefix[i].clear();   
        vector<Segment> seg0;
        cout<<"path 0 start"<<endl;

    for(auto edge : graph_.edge_list){
        //cout<<count_enumeration<<"/"<<number_of_edges<<endl;
        //count_enumeration++;

        Segment tempSegment;
        int pathid;
        pathkey.src = edge.src;pathkey.dst=edge.dst;

        pathid = pathlist0[pathkey.src][pathkey.dst];

        if(pathid == 0) {
            tempSegment.clear();
            tempSegment.path = pathkey;
            tempSegment.labelsequence.insert(edge.label);
            tempSegment.order = seg[0].size();
            seg[0].push_back(tempSegment);
            pathlist0[pathkey.src][pathkey.dst] = seg[0].size();
        }
        else{
            seg[0][pathid-1].labelsequence.insert(edge.label);
        }

        pathkey.src = edge.dst;pathkey.dst=edge.src;
        pathid = pathlist0[pathkey.src][pathkey.dst];
         if(pathid == 0) {
             tempSegment.clear();
             tempSegment.path = pathkey;
             tempSegment.labelsequence.insert(edge.label + graph_.number_of_labels);
             tempSegment.order = seg[0].size();
             seg[0].push_back(tempSegment);
             pathlist0[pathkey.src][pathkey.dst] = seg[0].size();
         }
        else{
             seg[0][pathid-1].labelsequence.insert(edge.label+graph_.number_of_labels);
         }
    }

    for(int i=0;i<k;i++){//for efficient join
        for(auto segment : seg[0]){
            for(auto label: segment.labelsequence) {
                if(workload.labelset_k[i][label]) {
                    pathlist_k[i][segment.path.src].push_back(make_pair(segment.path.dst, segment.order));
                    if(i==0){
                        seg_prefix[0].push_back(segment);
                    }
                    break;
                }
            }
        }
    }

    vector<Segment> prefixPath;


    clock_gettime(CLOCK_REALTIME, &enumerate_endTime);

    cout<<"length0 DONE ";
    cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;


    Segment tempSegment;
    Segment prefixtempSegment;
    vector<Segment> segmentVectorSameSrc;
    segmentVectorSameSrc.clear();

    for(int for_k = 1;for_k<k;for_k++) {// for all k length path
        count_enumeration=0;
        //cout<<"length" <<for_k<< " size = "<<seg.size()<<": start"<<endl;
        clock_gettime(CLOCK_REALTIME, &enumerate_startTime);
    	if(for_k==1){
            for (Segment segment : seg_prefix[0]) {

                count_enumeration++;

                Path pathl; pathl.src=segment.path.src;pathl.dst=segment.path.dst;
                int id_path0;

                for (auto endVertex : pathlist_k[for_k][segment.path.dst]) {
                    pathkey.src= segment.path.src;
                    pathkey.dst= endVertex.first;

                    int pathid = pathlist[pathkey.src][pathkey.dst];
                    id_path0 = endVertex.second;
                    if(pathid == 0) {
                        tempSegment.clear();
                        tempSegment.path = pathkey;
                        tempSegment.order = seg[for_k].size();

                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * number_of_labels + label_0;
                                if (workload.prefixlabelset[edgelabel]||workload.labelset[edgelabel])tempSegment.labelsequence.insert(edgelabel);
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
                                int edgelabel = (1 + label_l) * number_of_labels + label_0;
                                if (workload.prefixlabelset[edgelabel]||workload.labelset[edgelabel])seg[for_k][pathid-1].labelsequence.insert(edgelabel);
                            }
                        }
                    }
                }
            }
        }
        else{
            for (Segment segment : seg[for_k - 1]) {

                count_enumeration++;

                Path pathl; pathl.src=segment.path.src;pathl.dst=segment.path.dst;
                int id_path0;

                for (auto endVertex : pathlist_k[for_k][segment.path.dst]) {
                    pathkey.src= segment.path.src;
                    pathkey.dst= endVertex.first;

                    int pathid = pathlist[pathkey.src][pathkey.dst];
                    id_path0 = endVertex.second;
                    if(pathid == 0) {
                        tempSegment.clear();
                        tempSegment.path = pathkey;
                        tempSegment.order = seg[for_k].size();

                        for (auto label_l : segment.labelsequence) {
                            for (auto label_0 : seg[0][id_path0].labelsequence) {
                                int edgelabel = (1 + label_l) * number_of_labels + label_0;
                                if (workload.prefixlabelset[edgelabel]||workload.labelset[edgelabel])tempSegment.labelsequence.insert(edgelabel);
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
                                int edgelabel = (1 + label_l) * number_of_labels + label_0;
                                if (workload.prefixlabelset[edgelabel]||workload.labelset[edgelabel])seg[for_k][pathid-1].labelsequence.insert(edgelabel);
                            }
                        }
                    }
                }
            }
        }

        for(int i=0;i<number_of_vertices;i++) {
            pathlist[i].clear();
        }

        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        cout<<"length" <<for_k<<": enumarate DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;
        cout<<seg.size()<<endl;
       cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

    }//for_k


   for(int for_k=1;for_k <k ;for_k++){
	for(Segment segment : seg[for_k]){	
                pathkey.src= segment.path.src;
                pathkey.dst= segment.path.dst;

            int pathid = pathlist0[pathkey.src][pathkey.dst];
            if(pathid == 0) {
                tempSegment.clear();
                tempSegment.path = pathkey;
                tempSegment.order = seg[0].size();

                for (auto label_l : segment.labelsequence) {
                        if (workload.labelset[label_l])tempSegment.labelsequence.insert(label_l);
                    }

                if(!tempSegment.labelsequence.empty()){
                    seg[0].push_back(tempSegment);
                    pathlist0[pathkey.src][pathkey.dst]=seg[0].size();
		    }
                }
                else{
                    for (auto label_l : segment.labelsequence) {
                            if (workload.labelset[label_l])seg[0][pathid-1].labelsequence.insert(label_l);
                        }
                    }
		}
   }

    cout<<"workloadaware-transequivalent DONE"<<endl;

}







void WorkloadawareTransequivalent::Output(string indexfilename, OriginalGraph graph_){

    std::ofstream fout(indexfilename, ios::app);

    for(int for_k=0;for_k<k;for_k++){
        fout<<"length = "<<for_k<<endl;
        for(auto segment : seg[for_k]){
            fout<<segment.path.src<<"->"<<segment.path.dst<<"(";
            for (auto ittA = segment.labelsequence.begin(); ittA != segment.labelsequence.end(); ittA++) {
                fout << *ittA;
                fout<<",";
            }
            fout <<")"<<endl;

        }

    }

}



