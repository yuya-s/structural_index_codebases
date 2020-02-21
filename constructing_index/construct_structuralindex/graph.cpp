//
// Created by sasaki on 18/12/05.
//

#include "graph.h"

void OriginalGraph::InputFile(std::string input_file_name)
    {

        const char *graphfile=input_file_name.c_str();
        std::ifstream input(graphfile);

        if(!input){
            std::cout<<"error: cannot open graph file"<<std::endl;
            exit(1);
        }

        input>>number_of_vertices>>number_of_edges>>number_of_labels;


        string str = "1";

        degrees.resize(number_of_vertices);
		for(int i=0;i<number_of_vertices;i++)degrees[i]=0;
        vertex_edge_list.resize(number_of_vertices);

        string v1,v2,label;

        while(!input.eof()){

            input >> v1 >> v2 >>label;
            if(input.fail())break;

            Edge edge(stoi(v1), stoi(v2), stoi(label));
            edge_list.push_back(edge);
            degrees[stoi(v1)]++;
			degrees[stoi(v2)]++;

        }
        std::sort(edge_list.begin(), edge_list.end(), cmpedge);

        for(int i=0;i<number_of_vertices;i++){
            if(max_degree<degrees[i])max_degree=degrees[i];
        }

    }