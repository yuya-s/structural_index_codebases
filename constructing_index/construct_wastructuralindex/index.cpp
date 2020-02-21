//
// Created by sasaki on 19/03/11.
//
#include "index.h"
#include "workloadaware_transequivalent.h"

PathIndex::PathIndex(WorkloadawareTransequivalent& k_pathset){

    struct timespec index_startTime, index_endTime;
    long double responseTime;

    k=k_pathset.k;
    labelnum=k_pathset.number_of_labels;

    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    label2history.resize(labelidsize);

    clock_gettime(CLOCK_REALTIME, &index_startTime);

    vector<Segment*> sortref;
    for(Segment& s : k_pathset.seg[0]){
	sortref.push_back(&s);
    }
    sort(sortref.begin(),sortref.end(),segmentcmppointer);

	int historyid=0;
	Segment* previousSegment=NULL;
	vector<Path> te_pathset;
	int loopFlag;
	int previousLoopFlag;
    for(auto seg : sortref){

	if(seg->path.src == seg->path.dst)loopFlag=1;
	else loopFlag=0;

	if(previousSegment == NULL){
		te_pathset.push_back(seg->path);
		for(auto l : seg->labelsequence){
			label2history[l].push_back(historyid);
		}
	}
	else if(seg->labelsequence == previousSegment->labelsequence&&loopFlag==previousLoopFlag){
		te_pathset.push_back(seg->path);	
	}	
	else{
		history2path.push_back(te_pathset);
		te_pathset.clear();
		historyid++;
		te_pathset.push_back(seg->path);
		for(auto l : seg->labelsequence){
			label2history[l].push_back(historyid);
		}
	}
        previousSegment = seg;
        previousLoopFlag=loopFlag;
    }
    history2path.push_back(te_pathset);


    clock_gettime(CLOCK_REALTIME, &index_endTime);
    responseTime =
                (index_endTime.tv_sec - index_startTime.tv_sec) + (index_endTime.tv_nsec - index_startTime.tv_nsec) * pow(10, -9);
    cout<<responseTime<<endl;
}

void PathIndex::Output(string indexfilename){


    string outputL2H = "./index/"+indexfilename+"_workload_l2h";
    string outputH2P = "./index/"+indexfilename+"_workload_h2p";
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
	fout_h2p << i<<" ";
        for(auto path : history2path[i]) {
           fout_h2p << path.src << " " << path.dst <<" ";
        }
        fout_h2p<<endl;
    }


//    int labelid=0;
 //   for(auto l2h : label2history){
  //      cout<<labelid<<" "<<l2h.size()<<endl;
   //     labelid++;
//}
}
