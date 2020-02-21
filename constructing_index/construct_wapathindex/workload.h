//
// Created by sasaki on 19/04/25.
//

#ifndef WORKLOADAWARE_K_BISIMULATION_INDEX_WORKLOAD_H
#define WORKLOADAWARE_K_BISIMULATION_INDEX_WORKLOAD_H

#include "utility.h"

class WorkLoad {

public:

    vector<bool> labelset;
    vector<vector<bool>>labelset_k;
    vector<bool> prefixlabelset;
    int k;
    int labelnum;
	WorkLoad(){
    }

    void InputFile(std::string,int k, int labelnum);
};

#endif //WORKLOADAWARE_K_BISIMULATION_INDEX_WORKLOAD_H
