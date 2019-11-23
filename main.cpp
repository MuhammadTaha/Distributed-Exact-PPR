#include <iostream>
#include "rwr.h"

#include "precompute.h"


double teleport = 0.15;
double tolerance = 1e-4;
double storeTolerance = tolerance * 1e-3;

string path;
string dataset;
int level;
int n = -1, m = -1;
double accTime = 0;

mutex mtx;
int threadNum = 15;
const int maxThread = 32;

bool wsWriteFlag = false;
bool hubpvWriteFlag = false;
bool pvWriteFlag = false;

/*test related*/
int portno = 55;
char hostName[] = "...";
int repTimes = 1;
string queryFile = "query.txt";

int main() {
    std::cout << "Hello, World!" << std::endl;

    hubRWR *rwr = new hubRWR();
    loadGraph(rwr);
    rwr->setTeleport(teleport);
    rwr->setTolerance(tolerance);



    return 0;
}


//void loadGraph(hubRWR *rwr){
//    string graphFile = path + dataset + ".graph";
//
//    ifstream ifp(graphFile.c_str());
//    string line;
//    n = -1;
//    int u = 0;
//    while(getline(ifp, line)){
//        istringstream iss(line);
//        if(n == -1){
//            iss >> n >> m;
//            rwr->setUserNum(n);
//            // cout << n << " " << m << endl;
//        }
//        else{
//            //0-based node id
//            int v;
//            while(iss >> v){
//                v--;
//                rwr->addEdge(u, v);
//            }
//            u++;
//        }
//    }
//    cerr << "Graddddph loaded" << endl;
//    ifp.close();
//}

//void loadHubInfo(hubRWR *rwr){
//    string hubFile = path + dataset + ".hub";
//
//    ifstream ifs(hubFile.c_str());
//    string line;
//    while(getline(ifs, line)){
//        istringstream iss(line);
//        int u, lvl;
//        iss >> u >> lvl;
//        //0-based node id
//        rwr->addHubNode(u - 1, lvl);
//    }
//    cerr << "Hub nodes added loaded" << endl;
//
//    ifs.close();
//}