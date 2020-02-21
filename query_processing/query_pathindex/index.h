//
// Created by sasaki on 19/04/16.
//

#ifndef QUERY_PATHINDEX_INDEX_H
#define QUERY_PATHINDEX_INDEX_H
#include "utility.h"

class PathIndex{

public:
    int k;
    int labelnum; //original label num * 2;
    vector< vector< pair<int,int> > > label2path;

    PathIndex(std::string);
    void Output(string output_filename);
};

inline int encodeLabel(vector<int>& labellist, int labelnum, int k){

    int labelsequence=0;
    for(int i=0;i<labellist.size();i++){

        if(i==0)labelsequence += labellist[i];
        else{
            labelsequence = (labelsequence+1)*labelnum+labellist[i];
        }
    }
    return labelsequence;
}
#endif //QUERY_PATHINDEX_INDEX_H
