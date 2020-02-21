//
// Created by sasaki on 18/12/06.
//

#include "pathset.h"
#include "graph.h"



PathSet::PathSet(int k_ , OriginalGraph graph_) {


    struct timespec test_startTime, test_endTime, enumerate_startTime, enumerate_endTime, sort_startTime, sort_endTime, id_startTime, id_endTime;

    k=k_;
    number_of_labels = graph_.number_of_labels*2;
    number_of_vertices=graph_.number_of_vertices;
    number_of_edges=graph_.number_of_edges;

    seg.resize(k);
    sortref.resize(k);
    path2segment.resize(k);

//    google::dense_hash_map<Path,int, pathhash, patheqr> p2s;// for tentative path2segment
      Path pathkey;
//    pathkey.src=graph_.number_of_vertices;
//    pathkey.dst=graph_.number_of_vertices;
//    p2s.set_empty_key(pathkey);
//    for(int i=0; i<path2segment.size();i++){
//        path2segment[i].set_empty_key(pathkey);
//    }

    vector<google::dense_hash_map< int, int>> pathlist;
    pathkey.src=-1;
    pathkey.dst=-1;
    pathlist.resize(number_of_vertices);
    vector<int> tempvector={-1};
    for(int i=0;i<number_of_vertices;i++) {
        pathlist[i].set_empty_key(-1);
    }

    int count_enumeration=0;
    clock_gettime(CLOCK_REALTIME, &enumerate_startTime);
    for(auto edge : graph_.edge_list){
        //cout<<count_enumeration<<"/"<<number_of_edges<<endl;
        //count_enumeration++;

        Segment tempSegment;
        int pathid;
        pathkey.src = edge.src;pathkey.dst=edge.dst;

        pathid = pathlist[pathkey.src][pathkey.dst];

        if(pathid == 0) {
            tempSegment.clear();
            tempSegment.path = pathkey;
            tempSegment.labelsequence.insert(edge.label);
            tempSegment.order = seg[0].size();
            seg[0].push_back(tempSegment);
            pathlist[pathkey.src][pathkey.dst] = seg[0].size();
        }
        else{
            seg[0][pathid-1].labelsequence.insert(edge.label);
        }


        pathkey.src = edge.dst;pathkey.dst=edge.src;
        pathid = pathlist[pathkey.src][pathkey.dst];
         if(pathid == 0) {
             tempSegment.clear();
             tempSegment.path = pathkey;
             tempSegment.labelsequence.insert(edge.label + graph_.number_of_labels);
             tempSegment.order = seg[0].size();
             seg[0].push_back(tempSegment);
             pathlist[pathkey.src][pathkey.dst] = seg[0].size();
         }
        else{
             seg[0][pathid-1].labelsequence.insert(edge.label+graph_.number_of_labels);
         }

    }


    int countid=1;

    vector<vector<pair<int,int>>> pathlist0(graph_.number_of_edges); // for efficient join
    int count;
    int previousSegvalue;
    int previousloop;

    count=0;
    countid=1;
    for(Segment segment : seg[0]){
        Path path = segment.path;
        pathlist0[path.src].push_back(make_pair(path.dst,segment.order));//for efficient join [src] -> set of dst
    }

    for(int i=0;i<number_of_vertices;i++) {
        pathlist[i].clear();
    }

    clock_gettime(CLOCK_REALTIME, &enumerate_endTime);

    cout<<"length0 DONE ";
    cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;


    Segment tempSegment;
    vector<Segment> segmentVectorSameSrc;
    segmentVectorSameSrc.clear();

    for(int for_k = 1;for_k<k;for_k++) {// for all k length path
        //computing label sequence for all paths
        count_enumeration=0;
        cout<<"length" <<for_k<< " size = "<<seg[for_k - 1].size()<<": start"<<endl;
        clock_gettime(CLOCK_REALTIME, &enumerate_startTime);

        for (Segment segment : seg[for_k - 1]) {

            //cout<<count_enumeration<<"/"<<seg[for_k-1].size()<<endl;
            //count_enumeration++;

            Path pathl; pathl.src=segment.path.src;pathl.dst=segment.path.dst;
            int id_path0;

            for (auto endVertex : pathlist0[segment.path.dst]){
                pathkey.src=segment.path.src;
                pathkey.dst= endVertex.first;

                int pathid = pathlist[pathkey.src][pathkey.dst];
                id_path0 = endVertex.second;
                if(pathid == 0) {
                    tempSegment.clear();
                    tempSegment.path = pathkey;
                    tempSegment.order = seg[for_k].size();
                    for (auto label_l : segment.labelsequence) {
                        for (auto label_0 : seg[0][id_path0].labelsequence) {
                            tempSegment.labelsequence.insert((1 + label_l) * number_of_labels + label_0);
                        }
                    }
                    seg[for_k].push_back(tempSegment);
                    pathlist[pathkey.src][pathkey.dst]=seg[for_k].size();
                }
                else{
                    for (auto label_l : segment.labelsequence) {
                        for (auto label_0 : seg[0][id_path0].labelsequence) {
                            seg[for_k][pathid-1].labelsequence.insert((1 + label_l) * number_of_labels + label_0);
                        }
                    }
                }
            }
        }

        //for (Segment segment : seg[for_k]) {
        //    cout<<segment.path.src<<endl;
        // }
        for(int i=0;i<number_of_vertices;i++) {
            pathlist[i].clear();
        }
        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        cout<<"length" <<for_k<<": enumarate DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

        cout<<seg[for_k].size()<<endl;
        //std::sort(seg[for_k].begin(), seg[for_k].end(),segmentcmp);

        //clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        //cout<<"length" <<for_k<<": sort DONE ";
        //cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;


        //seg[for_k].erase(std::unique(seg[for_k].begin(), seg[for_k].end()), seg[for_k].end());
        //clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        //cout<<seg[for_k].size()<<endl;
        //cout<<"length" <<for_k<<": duplicate delete DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

    }//for_k

    cout<<"k-path-set DONE"<<endl;

}







void PathSet::Output(string indexfilename, OriginalGraph graph_){

    std::ofstream fout(indexfilename, ios::app);

    for(int for_k=0;for_k<k;for_k++){
        fout<<"length = "<<for_k<<endl;
        for(auto segment : seg[for_k]){
            fout<<segment.path.src<<"->"<<segment.path.dst<<"(";
            for (auto ittA = segment.labelsequence.begin(); ittA != segment.labelsequence.end(); ittA++) {
                //for(auto label : hash2pathlabel[*ittA]){
                fout << *ittA;
                //}
                fout<<",";
            }
            fout <<")"<<endl;

        }

    }

}






int PathSet::find(vector<pair<int,int>>& p2s, int dst){

    for(int i=0; i< p2s.size();i++){
        if(p2s[i].first == dst) return p2s[i].second;
    }
    return -1;
}

void PathSet::update(vector<pair<int,int>>& p2s, int dst, int id){

    for(int i=0; i< p2s.size();i++){
        if(p2s[i].first == dst)p2s[i].second = id;
    }

}

