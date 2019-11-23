#include "precompute.h"
#include <cmath>


void readConfig(){
    ifstream ifs("config");
    //Read first 3 lines
    for(int i = 0; i < 3; i++){
        string line;
        getline(ifs, line);
        istringstream iss(line);
        string tmp;
        if(i == 0){
            iss >> tmp >> tmp >> level;
        }
        else if(i == 1){
            iss >> tmp >> tmp >> path;
        }
        else if(i == 2){
            iss >> tmp >> tmp >> dataset;
        }
    }
    ifs.close();
    cout << "Level : " << level << endl;
    cout << "Path : " << path << endl;
    cout << "Dataset: " << dataset << endl;
}

double maxDiff(double *a, double *b, unsigned len){
    double maxDiff = 0;
    for(unsigned i = 0; i < len; i++){
        maxDiff = max(maxDiff, fabs(a[i] - b[i]));
        // if(fabs(a[i] - b[i]) > 0.015)
            // cout << i << " : " << a[i] << " " << b[i] << endl;
    }
    return maxDiff;
}

double sumUp(double *a, unsigned len){
    double sum = 0;
    for(unsigned i = 0; i < len; i++){
        sum += a[i];
    }
    return sum;
}

void loadGraph(hubRWR *rwr){
    string graphFile = path + dataset + ".graph";

    ifstream ifp(graphFile.c_str());
    string line;

    n = -1;
    int u = 0;
    while(getline(ifp, line)){
        istringstream iss(line);
        if(n == -1){
            iss >> n >> m;
            rwr->setUserNum(n);
            // cout << n << " " << m << endl;
        }
        else{
            //0-based node id
            int v;
            while(iss >> v){
                v--;
                rwr->addEdge(u, v);
            }
            u++;
        }
    }
    cerr << "Graddddph loaded" << endl;
    ifp.close();
}

void loadHubInfo(hubRWR *rwr){
    string hubFile = path + dataset + ".hub";

    ifstream ifs(hubFile.c_str());
    string line;
    while(getline(ifs, line)){
        istringstream iss(line);
        int u, lvl;
        iss >> u >> lvl;
        //0-based node id
        rwr->addHubNode(u - 1, lvl);
    }
    cerr << "Hub nodes added loaded" << endl;

    ifs.close();
}

void threadPrecom(int threadId, int H, hubRWR *rwr, unsigned nodeNum, vector<unsigned>* hubNodes, unsigned numberOfMachine, unsigned idOfMachine){
    int percent = 0;
    setStartTime();
    for(int i = threadId; i < H; i += threadNum){
        unsigned hubnodeId = hubNodes->at(i);
        if(hubnodeId % numberOfMachine != idOfMachine)
            continue;
        double * webSkeleton = rwr->basicDP_s(hubnodeId);
        string wsPath = path + dataset + ".ws";
        mtx.lock();
        /* cerr << "Writing skeleton to file" << endl; */
        vecWriter(webSkeleton, hubnodeId, nodeNum, wsPath, storeTolerance, wsWriteFlag);
        wsWriteFlag = true;
        mtx.unlock();
        delete [] webSkeleton;

        double * partialVec = rwr->selectiveExpansion(hubnodeId);
        wsPath = path + dataset + ".hubpv";
        mtx.lock();
        /* cerr << "Writing hub partial vector to file" << endl; */
        vecWriter(partialVec, hubnodeId, nodeNum, wsPath, storeTolerance, hubpvWriteFlag);
        hubpvWriteFlag = true;
        mtx.unlock();
        delete [] partialVec;

        if(!threadId && i > H * percent / 100){
            cout << "percent: " << percent << "% " << i << " / " << H << "\t";
            setEndTime();
            cout << "Time used: " << getSeconds() / 3600 << " h" << endl;
            while(i > H * percent / 100)
                percent += 10;
        }
    }
}

void precom(int _threadNum, int numberOfMachine, int idOfMachine){
    threadNum = _threadNum;
    hubRWR *rwr = new hubRWR();
    loadGraph(rwr);
    loadHubInfo(rwr);
    vector<unsigned> hubNodes = rwr->getHubNodes();
    unsigned H = hubNodes.size();
    rwr->setTeleport(teleport);
    rwr->setTolerance(tolerance);
    unsigned nodeNum = rwr->getUserNum();

    //multi thread
    thread mThread[maxThread];

    if((int)H < threadNum){
        threadNum = H;
    }

    for(int i = 0; i < threadNum; i++){
        mThread[i] = thread(threadPrecom, i, H, rwr, nodeNum, &hubNodes, numberOfMachine, idOfMachine);
    }
    for(int i = 0; i < threadNum; i++){
        mThread[i].join();
    }
    delete rwr;
}

void threadPvPrecom(int threadId, unsigned userNum, hubRWR *rwr, unsigned numberOfMachine, unsigned idOfMachine){
    int percent = 0;
    setStartTime();
    for(unsigned i = threadId; i < userNum; i += threadNum){
        if(rwr->getHubIndex(i) != -1){
            continue;
        }
        if(i % numberOfMachine != idOfMachine)
            continue;
        double *ppv = rwr->selectiveExpansion(i);
        string pvPath = path + dataset + ".pv";
        mtx.lock();
        /* cerr << "Writing partial vector to file" << endl; */
        vecWriter(ppv, i, userNum, pvPath, storeTolerance, pvWriteFlag);
        pvWriteFlag = true;
        mtx.unlock();
        if(!threadId && i > userNum * percent / 100){
            cout << "percent: " << percent << "% " << i << " / " << userNum << "\t";
            setEndTime();
            cout << "Time used: " << getSeconds() / 3600 << " h" << endl;
            while(i > userNum * percent / 100){
                percent += 10;
            }
        }
        delete [] ppv;
    }
}

void pvPrecom(int _threadNum, unsigned numberOfMachine, unsigned idOfMachine){
    threadNum = _threadNum;

    hubRWR *rwr = new hubRWR();
    loadGraph(rwr);
    loadHubInfo(rwr);
    rwr->setTeleport(teleport);
    rwr->setTolerance(tolerance);
    unsigned userNum = rwr->getUserNum();

    //multi thread
    thread mThread[maxThread];
    for(int i = 0; i < threadNum; i++){
        mThread[i] = thread(threadPvPrecom, i, userNum, rwr, numberOfMachine, idOfMachine);
    }
    for(int i = 0; i < threadNum; i++)
        mThread[i].join();

    delete rwr;
}

void loadPrec(hubPPVCstruct* ppvC){
    cerr << "Load user number" << endl;
    //load user number
    {
        string graphPath = path + dataset + ".graph";
        FILE * pFile = NULL;
        pFile = fopen(graphPath.c_str(), "r");
        if(pFile == NULL){
            fprintf(stderr, "Open File Failed: %s", graphPath.c_str());
            return;
        }
        int uNum;
        fscanf(pFile, "%d", &uNum);
        ppvC->setUserNum(uNum);
        fclose(pFile);
    }
    cerr << "Load hub nodes" << endl;
    //load hub
    {
        string hubPath = path + dataset + ".hub";
        FILE * pFile = NULL;
        pFile = fopen(hubPath.c_str(), "r");
        if(pFile == NULL){
            fprintf(stderr, "Open File Failed: %s", hubPath.c_str());
            return;
        }
        int uid, lvl;
        while(fscanf(pFile, "%d %d", &uid, &lvl) != EOF){
            //cerr << gid << " " << uid << " " << lvl << endl;
            ppvC->addHubnode(uid - 1);
        }
        fclose(pFile);
    }

    cerr << "Load web skeleton" << endl;
    //load web skeleton
    {
        string wsPath = path + dataset + ".ws";
        ppvC->loadWebSkeleton(wsPath);
    }

    cerr << "Load hub partial vector" << endl;
    //load hub partial vector
    {
        string hubpvPath = path + dataset + ".hubpv";
        ppvC->loadHubPartialVector(hubpvPath);
    }

    cerr << "Load partial vector" << endl;
    //load partial vector
    {
        string pvPath = path + dataset + ".pv";
        ppvC->loadPartialVector(pvPath);
    }
}
