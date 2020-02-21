#include "utility.h"
#include "graph.h"
#include "workloadaware_transequivalent.h"
#include "index.h"
#include "workload.h"

int main(int argc, char* argv[])
{

    string resultFile;
    string inputFile;
    int given_k;
    string string_k;
    string workloadFile;
    string indexName;

    try {
        for (int i = 1; i < argc; ++i) {
            std::string s = argv[i];
            if (s[0] == '-') {
                s = s.substr(1);

                if (s == "i") {
                    inputFile = argv[++i];
                }
                else if (s == "o") {
                    resultFile = argv[++i];
                }
                else if (s == "k"){
                    string_k=argv[++i];
                    given_k = stoi(argv[i]);
                }
                else if (s == "w"){
                    workloadFile =argv[++i];
                }
                else if (s == "n"){
                    indexName = argv[++i];
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
    string graphfile = inputFile;
    result.k = given_k;
    result.inputFile = inputFile;
    result.outputFile = "./result/"+resultFile;

    struct timespec bisimulation_startTime, bisimulation_endTime, index_startTime, index_endTime;

    OriginalGraph originalgraph = OriginalGraph();
    originalgraph.InputFile(graphfile);

    WorkLoad workload = WorkLoad();
    string workload_file = workloadFile;
    workload.InputFile(workload_file, given_k,originalgraph.number_of_labels*2);

    cout<<"path enumeration START "<<endl;
    clock_gettime(CLOCK_REALTIME, &bisimulation_startTime);

    WorkloadawareTransequivalent path_set(given_k,originalgraph, workload);
    clock_gettime(CLOCK_REALTIME, &bisimulation_endTime);
    long double responseTime = (bisimulation_endTime.tv_sec - bisimulation_startTime.tv_sec) + (bisimulation_endTime.tv_nsec - bisimulation_startTime.tv_nsec) * pow(10, -9);
    cout<<"path enumeration time : "<<responseTime<<endl;

    result.pathenumerationTime=responseTime;

    cout<<"indexing START "<<endl;
    clock_gettime(CLOCK_REALTIME, &index_startTime);
    PathIndex path_index(path_set);
    clock_gettime(CLOCK_REALTIME, &index_endTime);
    responseTime = (index_endTime.tv_sec - index_startTime.tv_sec) + (index_endTime.tv_nsec - index_startTime.tv_nsec) * pow(10, -9);
    cout<<responseTime<<endl;

    result.indexTime=responseTime;


    result.Output();
    path_index.Output(indexName);
    return 0;
}
