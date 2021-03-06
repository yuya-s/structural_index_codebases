#include "utility.h"
#include "index.h"
#include "query.h"

int main(int argc, char* argv[])
{

    string resultFile;
    string inputFile;
    string workloadFile;
    string indexFile;
    int given_k;
    string string_k;

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
    string graphfile =  inputFile +".edge";
    result.k = given_k;
    result.inputFile = inputFile;
    result.outputFile = "./result/"+resultFile;

    struct timespec query_startTime, query_endTime, index_startTime, index_endTime;
    long double responseTime;
    cout<<"Index Input Start"<<endl;
    //string indexfilename = "./index/"+inputFile+"_"+string_k+"_path";
    PathIndex pathindex(indexFile);

    cout<<"Index Input Done"<<endl;

    QuerySet queryset=QuerySet();
    //string workload_file = "./query/"+workloadFile;
    queryset.InputFile(workloadFile);
    result.querynum=queryset.rootqueries.size();

    int querynum=1;
    Query query=Query();

    string outputeach;
    outputeach = "./result/answernum_"+resultFile;
    std::ofstream fout(outputeach, ios::app);

    vector<pair<int,int>> answers;
    for(int for_query =0; for_query<queryset.rootqueries.size();for_query++) {
    
	//cout<<"querystart"<<endl;
	vector<double> querytime;
	querytime.clear();
	for(int q =0;q<querynum;q++){
        answers.clear();
            clock_gettime(CLOCK_REALTIME, &query_startTime);
            queryset.rootqueries[for_query].Evaluation(answers, pathindex);

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
