//
// Created by sasaki on 19/03/11.
//
#include "index.h"
#include "kbisimulation.h"

BisimulationIndex::BisimulationIndex(KPathBisimulation& k_pathbisimulation){

    google::dense_hash_map< std::vector<int>, int,vectorhash,vectoreqr> history2id;
    vector<google::dense_hash_map< int, int>> history;

    history.resize(k_pathbisimulation.number_of_vertices);
    vector<int> tempvector={-1};
    for(int i=0;i<k_pathbisimulation.number_of_vertices;i++) {
        history[i].set_empty_key(-1);
    }
    history2id.set_empty_key(tempvector);

    struct timespec index_startTime, index_endTime;
    long double responseTime;



    k=k_pathbisimulation.k;
    labelnum=k_pathbisimulation.number_of_labels;

    vector<int> refNumbers(k);
    vector<int> refsize(k);
    vector<Segment*> currentSegments(k);
    vector<int> segidVector(k);


    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    label2history.resize(labelidsize);

    clock_gettime(CLOCK_REALTIME, &index_startTime);

    for(int for_k =0; for_k < k;for_k++){
        std::sort(k_pathbisimulation.sortref[for_k].begin(), k_pathbisimulation.sortref[for_k].end(), cmpsrc);
        refNumbers[for_k]=0;
        currentSegments[for_k]=k_pathbisimulation.sortref[for_k][0];
        refsize[for_k]=k_pathbisimulation.sortref[for_k].size();
        cout<<"length="<<for_k<<" # of paths = "<<refsize[for_k]<<endl;
    }

    Path minPath;
    Segment terminalSegment;
    terminalSegment.path.src=refsize[k-1];
    terminalSegment.path.dst=refsize[k-1];

    int historyid;
    int countid=1;
    while(1){

        minPath = MinPath(currentSegments);
        for(int for_k =0; for_k < k;for_k++){
            if(minPath == currentSegments[for_k]->path){
                segidVector[for_k]=currentSegments[for_k]->segid;
                if(refNumbers[for_k]+1 < refsize[for_k]) {
                    refNumbers[for_k]++;
                    currentSegments[for_k] = k_pathbisimulation.sortref[for_k][refNumbers[for_k]];
                }
                else currentSegments[for_k] = &terminalSegment;
            }
            else{
                segidVector[for_k]=0;
            }
        }

        if(minPath==terminalSegment.path){
            break;
        }

        historyid=history2id[segidVector];
        if(historyid==0){
            history2id[segidVector]=countid;
            historyid=countid;
            vector<Path> temp ={minPath};
            history2path.push_back(temp);
            countid++;
        }
        else{
            history2path[historyid-1].push_back(minPath);
        }
        history[minPath.src][minPath.dst]=historyid;

    }

     for(int i=0;i<history2path.size();i++) {
        sort(history2path[i].begin(), history2path[i].end(),pathcmp);
    }

    cout<<"history to path DONE"<<endl;
    clock_gettime(CLOCK_REALTIME, &index_endTime);
    responseTime =
                (index_endTime.tv_sec - index_startTime.tv_sec) + (index_endTime.tv_nsec - index_startTime.tv_nsec) * pow(10, -9);
    cout<<responseTime<<endl;

    clock_gettime(CLOCK_REALTIME, &index_startTime);

    //computing label to history
    cout<<"label2history START"<<endl;
    for(int for_k=0;for_k < k;for_k++) {
        for (auto segment : k_pathbisimulation.seg[for_k]) {
            Path path = segment.path;
            historyid = history[path.src][path.dst];
            for (auto label: segment.labelsequence) {
                label2history[label].push_back(historyid-1);
            }
        }
    }

    for(int i=0;i<label2history.size();i++) {
        sort(label2history[i].begin(), label2history[i].end());
        label2history[i].erase(std::unique(label2history[i].begin(), label2history[i].end()), label2history[i].end());
    }

    cout<<"label2history DONE"<<endl;
    clock_gettime(CLOCK_REALTIME, &index_endTime);
    responseTime =
                (index_endTime.tv_sec - index_startTime.tv_sec) + (index_endTime.tv_nsec - index_startTime.tv_nsec) * pow(10, -9);
    cout<<responseTime<<endl;
}

void BisimulationIndex::Output(string indexfilename){


    string outputL2H = "./index/"+indexfilename+"_l2h";
    string outputH2P = "./index/"+indexfilename+"_h2p";
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
           fout_h2p << path.src << " " << path.dst <<" ";
        }
        fout_h2p<<endl;
    }


    int labelid=0;
    for(auto l2h : label2history){
        cout<<labelid<<" "<<l2h.size()<<endl;
        labelid++;
    }

}