//
// Created by sasaki on 19/04/16.
//
#include "index.h"

PathIndex::PathIndex(std::string input_file_name)
{

    std::ifstream input(input_file_name);

    if(!input){
        std::cout<<"error: cannot open file"<<std::endl;
        exit(1);
    }

    input>>k>>labelnum;

    int labelidsize=labelnum;
    for(int i=1;i<k;i++)labelidsize += pow(labelnum,i+1);
    label2path.resize(labelidsize);

    string v1,v2,label;
    int v1Id,v2Id,labelId;

    string line;
    while(std::getline(input,line)){
//        cout<<line<<endl;
        std::stringstream ss(line);
        ss >> labelId;
        ss.ignore(line.size(), ' ');
        //std::cout << name << " = ";
       while(ss >> v1Id >>v2Id)
        {
            label2path[labelId].push_back(make_pair(v1Id,v2Id));
        }
        //std::cout << "\n";
    }
//
//    while(!input.eof()){
//
//        input >> labelId >> v1Id >>v2Id;
//        if(input.fail())break;
//        label2path[labelId].push_back(make_pair(v1Id,v2Id));
//    }
    //number_of_edges = edge_list.size();
}