//
// Created by sasaki on 18/12/05.
//

#include "graph.h"

void OriginalGraph::InputFile(std::string input_file_name)
    {

        const char *graphfile=input_file_name.c_str();
        std::ifstream input(graphfile);

        if(!input){
            std::cout<<"error: cannot open file"<<std::endl;
            exit(1);
        }

        input>>number_of_vertices>>number_of_edges>>number_of_labels;


        string str = "1";
	    id2vertexname.set_empty_key(number_of_vertices);
        id2labelname.set_empty_key(number_of_labels);
        vertexname2id.set_empty_key("-1");
        labelname2id.set_empty_key("-1");

        degrees.resize(number_of_vertices);
		for(int i=0;i<number_of_vertices;i++)degrees[i]=0;
        vertex_edge_list.resize(number_of_vertices);

        string v1,v2,label;
        int v1Id,v2Id,labelId;
        int countVertexNum=0;
        int countLabelNum=0;

        while(!input.eof()){

            input >> v1 >> v2 >>label;
            if(input.fail())break;


            auto it = vertexname2id.find(v1);
            if(it == vertexname2id.end()){
                vertexname2id.insert(make_pair(v1,countVertexNum));
                id2vertexname.insert(make_pair(countVertexNum,v1));
                countVertexNum++;
            }
            it = vertexname2id.find(v2);
            if(it == vertexname2id.end()){
                vertexname2id.insert(make_pair(v2,countVertexNum));
                id2vertexname.insert(make_pair(countVertexNum,v2));
                countVertexNum++;
            }
            it = labelname2id.find(label);
            if(it == labelname2id.end()){
                labelname2id.insert(make_pair(label,countLabelNum));
                id2labelname.insert(make_pair(countLabelNum,label));
                countLabelNum++;
            }

            v1Id = vertexname2id[v1];
            v2Id = vertexname2id[v2];
            labelId = labelname2id[label];

            //cout<<v1Id<<","<<v2Id<<","<<labelId<<endl;

            Edge edge(stoi(v1), stoi(v2), stoi(label));
            edge_list.push_back(edge);
//            vertex_edge_list[stoi(v1)].push_back(edge_list.size()-1);
            degrees[stoi(v1)]++;
			degrees[stoi(v2)]++;



            if(countVertexNum>number_of_vertices || countLabelNum>number_of_labels){
                std::cout<<"error: graph size"<<std::endl;
                exit(1);
            }
        }
        std::sort(edge_list.begin(), edge_list.end(), cmpedge);

        average_degrees=0;
        for(int i=0;i<number_of_vertices;i++){
            if(max_degree<degrees[i])max_degree=degrees[i];
            average_degrees+=degrees[i];
        }
        average_degrees = average_degrees/number_of_vertices;

        //number_of_edges = edge_list.size();
    }