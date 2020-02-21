#include "utility.h"
#include "index.h"
#include "query.h"
#include "graph.h"

int main(int argc, char* argv[])
{

    string resultFile;
    string inputFile;
    string workloadFile;

    bool enumeratequery=false;
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
                else if (s == "qw") {
                    workloadFile = argv[++i];
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
    result.k = 0;
    result.inputFile = inputFile;
    result.outputFile = "./result/"+resultFile;

    struct timespec query_startTime, query_endTime;
    long double responseTime;

    OriginalGraph originalgraph = OriginalGraph();
    originalgraph.InputFile(graphfile);

    cout<<"Graph Input Done"<<endl;

    QuerySet queryset=QuerySet();
    queryset.InputFile(workloadFile);
    result.querynum=queryset.rootqueries.size();

    int querynum=1;
    Query query=Query();

    string outputeach;
    outputeach = "./result/answernum_"+resultFile;
    std::ofstream fout(outputeach, ios::app);


    vector<pair<int,int>> answers;
    cout<<"querystart"<<endl;
    for(int for_query =0; for_query<queryset.rootqueries.size();for_query++) {
    

        vector<double> querytime;
        querytime.clear();
        for(int q =0;q<querynum;q++){
        answers.clear();
            clock_gettime(CLOCK_REALTIME, &query_startTime);
            queryset.rootqueries[for_query].Evaluation(answers, originalgraph);

            clock_gettime(CLOCK_REALTIME, &query_endTime);
            responseTime = (query_endTime.tv_sec - query_startTime.tv_sec) +
                           (query_endTime.tv_nsec - query_startTime.tv_nsec) * pow(10, -9);
         //   cout << responseTime << endl;
            querytime.push_back(responseTime);
        }
        sort(answers.begin(), answers.end());
        answers.erase(std::unique(answers.begin(), answers.end()), answers.end());
        //for (auto answer : answers)cout << answer.first << "->" << answer.second << endl;
        sort(querytime.begin(),querytime.end());
	    responseTime = querytime[querynum/2];
	    cout << for_query<<": # of answers: " << answers.size() << ", time: " << responseTime << endl;
        fout <<answers.size() <<","<<responseTime<< endl;
        result.querySumTime+=responseTime;
    }
    result.Output();


    return 0;
}
