//
// Created by sasaki on 19/03/11.
//
#include "index.h"
#include "pathset.h"

PathIndex::PathIndex(PathSet& k_pathset){

    struct timespec index_startTime, index_endTime;
    long double responseTime;

    k=k_pathset.k;
    labelnum=k_pathset.number_of_labels;

    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    label2path.resize(labelidsize);

    clock_gettime(CLOCK_REALTIME, &index_startTime);

    //computing label to path
    for(int for_k=0;for_k < k;for_k++) {
        for (auto segment : k_pathset.seg[for_k]) {
            pair<int,int> path = make_pair(segment.path.src,segment.path.dst);
            for (auto label : segment.labelsequence) {
                label2path[label].push_back(path);
            }
        }
    }
    for(int i=0;i<label2path.size();i++) {
        sort(label2path[i].begin(), label2path[i].end(),paircmp);
    }

    clock_gettime(CLOCK_REALTIME, &index_endTime);
    responseTime =
                (index_endTime.tv_sec - index_startTime.tv_sec) + (index_endTime.tv_nsec - index_startTime.tv_nsec) * pow(10, -9);
    cout<<responseTime<<endl;
}

void PathIndex::Output(string output_filename_){

    string outputpath = "./index/"+output_filename_+"_path";
    std::ofstream fout(outputpath, ios::app);

    fout<<k<<" "<<labelnum<<endl;
    for(int i=0; i<label2path.size();i++) {

        if(label2path[i].empty())continue;

        fout <<i<<" ";
        for(auto path : label2path[i]) {
            fout<<path.first<<" "<<path.second<<" ";
        }
        fout<<endl;
     }

}