//
// Created by sasaki on 18/12/06.
//

#include "kbisimulation.h"
#include "graph.h"



KPathBisimulation::KPathBisimulation(int k_ , OriginalGraph graph_) {


    struct timespec enumerate_startTime, enumerate_endTime, sort_startTime, sort_endTime, id_startTime, id_endTime;



    k=k_;
    number_of_labels = graph_.number_of_labels*2;
    number_of_vertices=graph_.number_of_vertices;
    number_of_edges=graph_.number_of_edges;

    seg.resize(k);
    sortref.resize(k);
    path2segment.resize(k);
    numberofsegid.resize(k);
    vector<PathwithIDs*> sortpath; // for sort


    vector<PathwithIDs> pathset;
    PathwithIDs tempPath;
    Segment tempSegment;

    Path pathkey;

    vector<google::dense_hash_map< int, int>> pathlist;
    pathkey.src=-1;
    pathkey.dst=-1;
    pathlist.resize(number_of_vertices);
    vector<int> tempvector={-1};
    for(int i=0;i<number_of_vertices;i++) {
        pathlist[i].set_empty_key(-1);
    }

    clock_gettime(CLOCK_REALTIME, &enumerate_startTime);
    for(auto edge : graph_.edge_list){

        pathkey.src = edge.src;pathkey.dst=edge.dst;

        int pathid = pathlist[pathkey.src][pathkey.dst];
        tempPath.clear();

        if(pathid ==0) {
            tempPath.path = pathkey;
            tempPath.labelsequence.insert(edge.label);
            pathset.push_back(tempPath);
            pathlist[pathkey.src][pathkey.dst]=pathset.size();
        }
        else{
            pathset[pathid-1].labelsequence.insert(edge.label);
        }

        tempPath.clear();

        pathkey.src = edge.dst;pathkey.dst=edge.src;

        pathid = pathlist[pathkey.src][pathkey.dst];
        if(pathid==0) {
            tempPath.path = pathkey;
            tempPath.labelsequence.insert(edge.label + graph_.number_of_labels);
            pathset.push_back(tempPath);
            pathlist[pathkey.src][pathkey.dst]=pathset.size();
        }
        else{
            pathset[pathid-1].labelsequence.insert(edge.label + graph_.number_of_labels);
        }
    }

    for(auto s : pathset){
        tempSegment.clear();
        tempSegment.path = s.path;
        vector<int> vec(s.labelsequence.begin(), s.labelsequence.end());
        tempSegment.labelsequence = vec;
        tempSegment.order = seg[0].size();
        tempSegment.segvalue = hash_set(s.labelsequence);
        tempSegment.bisimilar = false;
        if(s.path.src==s.path.dst)tempSegment.loop = true;
        else tempSegment.loop = false;
        tempSegment.possiblitilyofbisimilar=true;
        seg[0].push_back(tempSegment);
    }


    for(Segment& s : seg[0]){
        sortref[0].push_back(&s);
    }

    std::sort(sortref[0].begin(), sortref[0].end(), cmpsegvaluepointer_0);

    int countid=1;

    vector<vector<pair<int,int>>> pathlist0(graph_.number_of_edges); // for efficient join
    int count;
    int previousSegvalue;
    bool previousloop;

    count=0;
    countid=1;
    Segment* previoussegment;
    for(Segment *segment : sortref[0]){// set segid
        Path path = segment->path;

        if(count==0){
            segment->segid=countid;
            previousSegvalue=segment->segvalue;
            previousloop = segment->loop;
        }
        else{//if seg values are same and self-loop/not loop, set same segid
            if(segment->segvalue==previousSegvalue){
                if(path.src==path.dst&&previousloop||path.src!=path.dst&&!previousloop){
                    segment->segid=countid;
                    segment->bisimilar=true;
                    previoussegment->bisimilar=true;
                }
                else{
                    countid++;
                    segment->segid=countid;
                    previousloop = segment->loop;
                }
            }
            else{
                countid++;
                segment->segid=countid;
                previousSegvalue=segment->segvalue;
                previousloop = segment->loop;
            }
        }
        previoussegment=segment;
        pathlist0[path.src].push_back(make_pair(path.dst,segment->order));//for efficient join [src] -> set of dst

        count++;
    }
    numberofsegid[0]=countid;

    for(int i=0;i<number_of_vertices;i++) {
        pathlist[i].clear();
    }
    pathset.clear();


    clock_gettime(CLOCK_REALTIME, &enumerate_endTime);

    cout<<"length0 DONE ";
    cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;
    int count_enumeration=0;

    vector<Segment> segmentVectorSameSrc;
    tempSegment.clear();
    segmentVectorSameSrc.clear();


    for(int for_k = 1;for_k<k;for_k++) {// for all k length path
        //computing label sequence for all paths

        count_enumeration=0;
        cout<<"length" <<for_k<< " size = "<<seg[for_k - 1].size()<<": start"<<endl;
        clock_gettime(CLOCK_REALTIME, &enumerate_startTime);

        for (Segment segment : seg[for_k - 1]) {

            count_enumeration++;
            Path pathl;
            pathl.src=segment.path.src;
            pathl.dst=segment.path.dst;
            int id_path0;

            for (auto endVertex : pathlist0[segment.path.dst]) {
                pathkey.src = segment.path.src;
                pathkey.dst = endVertex.first;

                id_path0 = endVertex.second;
                Segment seg0=seg[0][id_path0];

                int pathid = pathlist[pathkey.src][pathkey.dst];

                if(pathid == 0) {
                     tempPath.clear();
                     tempPath.path = pathkey;
                     tempPath.segvalue.insert(segment.segid * numberofsegid[for_k-1] + seg0.segid);
                     tempPath.bisimilar = (seg0.bisimilar + seg0.loop) * (segment.bisimilar + segment.loop);
                     for (auto label_l : segment.labelsequence) {
                         for (auto label_0 : seg0.labelsequence) {
                             tempPath.labelsequence.insert((1 + label_l) * number_of_labels + label_0);
                         }
                     }
                     pathset.push_back(tempPath);
                     pathlist[pathkey.src][pathkey.dst]=pathset.size();
                 }
                else{
                   for (auto label_l : segment.labelsequence) {
                         for (auto label_0 : seg0.labelsequence) {
                             pathset[pathid-1].labelsequence.insert((1 + label_l) * number_of_labels + label_0);
                         }
                     }
                   pathset[pathid-1].bisimilar *= (seg0.bisimilar + seg0.loop) * (segment.bisimilar + segment.loop);
                   pathset[pathid-1].segvalue.insert(segment.segid * numberofsegid[for_k-1] + seg0.segid);
                }
            }
        }
        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        cout<<"length" <<for_k<<": enumarate DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

        cout<<"# of paths : "<<pathset.size()<<endl;

        countid=1;

        for(auto s : pathset){
            tempSegment.clear();

            tempSegment.path = s.path;
            vector<int> vec(s.labelsequence.begin(), s.labelsequence.end());
            tempSegment.labelsequence = vec;
            tempSegment.order = seg[for_k].size();
            vector<int> vec_seg(s.segvalue.begin(), s.segvalue.end());
            tempSegment.segset=vec_seg;
            tempSegment.segvalue = hash_set(tempSegment.segset);

            tempSegment.bisimilar = false;
            if(s.path.src==s.path.dst)tempSegment.loop = true;
            else tempSegment.loop = false;
            tempSegment.possiblitilyofbisimilar=s.bisimilar;
            tempSegment.segid = countid+pathset.size();
            seg[for_k].push_back(tempSegment);
            countid++;
        }

        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);
        cout<<"length" <<for_k<<": duplicate delete DONE　";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

        for(int i=0;i<number_of_vertices;i++) {
            pathlist[i].clear();
        }
        pathset.clear();

        for(Segment& s : seg[for_k]){
            sortref[for_k].push_back(&s);
        }

        cout<<"# of segment : "<<sortref[for_k].size()<<endl;

        std::sort(sortref[for_k].begin(), sortref[for_k].end(), cmpsegvaluepointer);


        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);

        cout<<"length" <<for_k<<": enumarate DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;

        clock_gettime(CLOCK_REALTIME, &id_startTime);
        count=0;
        countid=1;
        int previousSegSetSize;
        vector<int> previousSegSet;
        for(Segment* segment : sortref[for_k]){
            Path path = segment->path;

            if(!segment->possiblitilyofbisimilar){
                break;
            }

            if (count == 0) {
                segment->segid = countid;
                previousSegvalue = segment->segvalue;
                previousSegSetSize = segment->segset.size();
                previousSegSet = segment->segset;
                segment->bisimilar = false;
                if (path.src == path.dst) {
                    segment->bisimilar = true;
                    previousloop = 1;
                } else previousloop = 0;
            } else {
                if (segment->segvalue == previousSegvalue &&segment->segset.size() == previousSegSetSize) {
                    bool segsetFlag=true;
                    for(int i=0;i<previousSegSetSize;i++){
                        if(segment->segset[i] != previousSegSet[i]){segsetFlag=false;break;}
                    }
                    if (segsetFlag&&(path.src == path.dst && previousloop == 1 || path.src != path.dst && previousloop == 0)) {
                        segment->segid = countid;
                        segment->bisimilar = true;
                        previoussegment->bisimilar = true;

                    } else {
                        countid++;
                        segment->segid = countid;
                        previousSegSet = segment->segset;
                        if (path.src == path.dst) {
                            previousloop = 1;
                            segment->bisimilar = true;
                        } else previousloop = 0;
                    }
                } else {
                    countid++;
                    segment->segid = countid;
                    previousSegvalue = segment->segvalue;
                    previousSegSetSize = segment->segset.size();
                    previousSegSet = segment->segset;

                    segment->bisimilar = false;

                    if (path.src == path.dst) {
                        previousloop = 1;
                        segment->bisimilar = true;
                    } else previousloop = 0;
                }

            }
            previoussegment = segment;

            count++;
        }
        numberofsegid[for_k]=countid;

        clock_gettime(CLOCK_REALTIME, &enumerate_endTime);

        cout<<"length" <<for_k<<": segid DONE ";
        cout<<(enumerate_endTime.tv_sec - enumerate_startTime.tv_sec) + (enumerate_endTime.tv_nsec - enumerate_startTime.tv_nsec) * pow(10, -9)<<endl;


    }//for_k

    cout<<"k-path-bisimulation DONE"<<endl;

}




void KPathBisimulation::Output(string output_filename, OriginalGraph graph_){

    std::ofstream fout(output_filename, ios::app);

    for(int for_k=0;for_k<k;for_k++){
        fout<<"length = "<<for_k<<endl;
        for(auto segment : seg[for_k]){
            fout<<segment.path.src<<"->"<<segment.path.dst<<"(";
            for (auto ittA = segment.labelsequence.begin(); ittA != segment.labelsequence.end(); ittA++) {
                fout << *ittA;
                fout<<",";
            }
            fout <<") segid = "<< segment.segid<<endl;

        }

    }

}




