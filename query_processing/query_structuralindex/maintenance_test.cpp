#include "index.h"
#include "query.h"
#include "graph.h"

int main(int argc, char* argv[])
{

    string resultFile;
    string inputFile;
    string workloadFile;
    string graphFile;
    string indexworkloadFile;
    string indexFile;
    int given_k=0;
    string string_k;
    bool workloadindex=false;
    int maintenancegraph =0;
    string maintenanceworkload;
    bool maintenanceworkloadFlag=false;
    try {
        for (int i = 1; i < argc; ++i) {
            std::string s = argv[i];
            if (s[0] == '-') {
                s = s.substr(1);

                if (s == "i") {
                    inputFile = argv[++i];
                }
                else if (s == "if") {
                    indexFile = argv[++i];
                }
                else if (s == "o") {
                    resultFile = argv[++i];
                }
                else if (s == "k"){
                    string_k=argv[++i];
                    given_k = stoi(argv[i]);
                }
                else if (s == "qw") {
                    workloadFile = argv[++i];
                }
                else if(s == "iw"){
                    indexworkloadFile = argv[++i];
                    workloadindex = true;
                }
                else if(s == "mg"){
                    maintenancegraph = stoi(argv[++i]);
                }
                else if(s == "mw"){
                    maintenanceworkload = argv[++i];
                    maintenanceworkloadFlag=true;
                }
                else {
                    throw std::exception();
                }
            }
            else {
                throw std::exception();
            }
        }
    }
    catch (std::exception& e) {
        cout<<"error input"<<endl;
        return 1;
    }


    Result result;
    result.Crear();
    string graphfile = inputFile;
    result.k = given_k;
    result.inputFile = inputFile;
    result.outputFile = "./result/"+resultFile;

    struct timespec maintenance_startTime, maintenance_endTime;

    BisimulationIndex bisimulationindex(indexFile);

    if (workloadindex) {
        bisimulationindex.SetWorkload(indexworkloadFile);
        bisimulationindex.workloadindex = true;
    }


    cout<<"Index Input Done"<<endl;

    OriginalGraph originalgraph = OriginalGraph();
    originalgraph.InputFile(graphfile);




    string maintain_outputeach;
    maintain_outputeach = "./result/graphmaintenance_"+resultFile;
    if(maintenancegraph > 0) {//mentenance test
        std::ofstream fout_maintain(maintain_outputeach, ios::app);
        vector<Edge> maintenanceEdgeSet;
        maintenanceEdgeSet.clear();
        double maintainTime;


        vector<int> deleteedgeidlist;
        deleteedgeidlist.clear();

        for(int g=0;g<maintenancegraph;g++) {
            cout<<g<<":delete"<<endl;
            std::random_device rnd;
            std::mt19937 mt(rnd());
            std::uniform_int_distribution<> rand(0, originalgraph.number_of_edges - 1);

            int deleteedgeid=0;

            while(1) {
                bool breakflag=true;
                deleteedgeid=rand(mt);
                for(int i=0; i < deleteedgeidlist.size();i++){
                    if(deleteedgeidlist[i]==deleteedgeid){
                        breakflag=false;
                        break;
                    }
                }

                if(breakflag)break;
            }
            deleteedgeidlist.push_back(deleteedgeid);

            Edge dedge = originalgraph.edge_list[deleteedgeid];
            maintenanceEdgeSet.push_back(dedge);
            cout<<g<<":delete"<<dedge.src<<"->"<<dedge.dst<<endl;

            clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

            bisimulationindex.DeleteEdge(dedge, originalgraph);

            clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
            maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
            cout<<"delete time: "<<maintainTime<<endl;
            fout_maintain <<originalgraph.vertex_edge_list[dedge.src].size()+originalgraph.vertex_inverseedge_list[dedge.src].size()<<","<< originalgraph.vertex_edge_list[dedge.dst].size()+originalgraph.vertex_inverseedge_list[dedge.dst].size()<<","<<maintainTime<< endl;
            result.maintenanceDeleteEdgeTime += maintainTime;
        }
        cout<<"total delete time: "<<result.maintenanceDeleteEdgeTime<<endl;

        for(int i=0;i<maintenancegraph;i++) {
            Edge aedge = maintenanceEdgeSet[i];
            cout<<i<<":add"<<aedge.src<<"->"<<aedge.dst<<endl;
            clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

            bisimulationindex.AddEdge(aedge, originalgraph);

            clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
            maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);

            cout<<"add time: "<<maintainTime<<endl;
            fout_maintain <<originalgraph.vertex_edge_list[aedge.src].size()+originalgraph.vertex_inverseedge_list[aedge.src].size()<<","<< originalgraph.vertex_edge_list[aedge.dst].size()+originalgraph.vertex_inverseedge_list[aedge.dst].size()<<","<<maintainTime<< endl;
            result.maintenanceAddEdgeTime += maintainTime;
        }

        result.maintenanceEdgeNum=maintenancegraph;
        result.OutputMaintenance();
	    string mainteinedindex;
        if(!bisimulationindex.workloadindex)mainteinedindex = indexFile+"MG";
	    else mainteinedindex=indexFile+"MG_wa";

        bisimulationindex.Output(mainteinedindex);
    }

    string maintainwa_outputeach;
    maintain_outputeach = "./result/workloadmaintenance_"+resultFile;
    if(maintenanceworkloadFlag && bisimulationindex.workloadindex) {

        std::ofstream fout_maintainwa(maintain_outputeach, ios::app);
        vector<vector<int>> maintenancelabelset;
        vector<int> maintenancelabel;
        std::ifstream inputworkmaintenance(maintenanceworkload);
        double maintainTime;
        if(!inputworkmaintenance){
            std::cout<<"error: cannot open maintenance workload file"<<std::endl;
            exit(1);
        }
        string line, label;
        while(std::getline(inputworkmaintenance,line)){
    //        cout<<line<<endl;
            std::stringstream ss;
            ss<<line;
            cout<<ss.str()<<endl;
            maintenancelabel.clear();
            while(ss >> label)
            {
                 if(std::all_of(label.cbegin(),label.cend(), [](char ch) { return isdigit(ch);})) {
                     maintenancelabel.push_back(stoi(label));
                 }
            }
            maintenancelabelset.push_back(maintenancelabel);
            result.maintenanceWorkloadNum++;
        }

        for(auto label : maintenancelabelset) {
           clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

            bisimulationindex.DeleteWorkload(label,originalgraph);

            clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
            maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
            fout_maintainwa <<maintainTime<< endl;

            result.maintenanceDeleteWorkloadTime += maintainTime;
        }

        for(auto label : maintenancelabelset) {
            clock_gettime(CLOCK_REALTIME, &maintenance_startTime);

            bisimulationindex.AddWorkload(label,originalgraph);

            clock_gettime(CLOCK_REALTIME, &maintenance_endTime);
            maintainTime = (maintenance_endTime.tv_sec - maintenance_startTime.tv_sec) + (maintenance_endTime.tv_nsec - maintenance_startTime.tv_nsec) * pow(10, -9);
            fout_maintainwa <<maintainTime<< endl;

            result.maintenanceAddWorkloadTime = maintainTime;
        }


        result.OutputMaintenance();
        string mainteinedindex = indexFile+"MW";
        bisimulationindex.Output(mainteinedindex);
    }

    return 0;
}
