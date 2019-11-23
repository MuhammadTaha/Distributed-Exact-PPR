#include "rwr.h"
#include <cmath>

#define FIXDEGREE

// #define SHOWITERATION 

//Implemented by vector
double *
hubRWR::iteration(unsigned seed){
    if(score != NULL){
        delete [] score;
    }
    score = new double[userNum];

    double *tmpScore = new double[userNum];   
    bool *inQueue = new bool[userNum];
    std::vector<unsigned>& curQueue = valuedNode;
    curQueue.clear();
    for(unsigned i = 0; i < userNum; i++){
        score[i]    = 0.0;
        tmpScore[i] = 0.0;
        inQueue[i]  = false;
    }

    curQueue.push_back(seed);
    inQueue[seed] = true;
    score[seed] = 1.0;

    //interation
    bool converage = false;
    int step = 0;

    while(!converage){
        std::vector<int>incQueue;
        for(auto &node : curQueue){
            std::vector<Edge>* neiList = getNeighbors(node);
            tmpScore[seed] += teleport * score[node];

            for(auto &edge : *neiList){
                unsigned neighborId = edge.neighborId;
                tmpScore[neighborId] +=  (1 - teleport) * score[node] / neiList->size();
                if(!inQueue[neighborId]){
                    inQueue[neighborId] = true;
                    incQueue.push_back(neighborId);
                }
                // if(neighborId == seed)
                //     std::cerr << "From: " << node << " val: " << (1 - teleport) * score[node] / neiList->size() << std::endl;
            }
            //special case when there is no neighbor
            if(neiList->size() == 0)
                tmpScore[node] += (1 - teleport) * score[node];
        }

        for(auto& node : incQueue)
            curQueue.push_back(node);

        converage = true;
        for(auto& node : curQueue){
            if(fabs(score[node] - tmpScore[node]) > tolerance)
                converage = false;
            score[node] = tmpScore[node];
            tmpScore[node] = 0.0;
        }
        step++;
        //show results during every iterations
        // double sumres = 0;
        // for(auto& node : curQueue)
        //     sumres += score[node];
            // std::cerr << node << " : " << score[node] << std::endl;
        // std::cerr << "Sum: " << sumres << std::endl;
        // std::cerr << step << " seed:" << seed << ":" << score[seed] << std::endl;
    }
    // std::cerr << "Step: " << step << std::endl;
    
    delete [] tmpScore;
    delete [] inQueue;
    return score;
}

//get partial vector
double *
hubRWR::selectiveExpansion(unsigned seed){
    double **D = new double*[2];
    double **E = new double*[2];
    int* updateFlag = new int[userNum];
    int seedIndex = getHubIndex(seed);
    if(seedIndex == -1)
        seedIndex = userNum * 2;
    for(int i = 0; i < 2; i++){
        D[i] = new double[userNum];
        E[i] = new double[userNum];
    }

    std::vector<unsigned>curQueue;
    for(unsigned i = 0; i < userNum; i++){
        D[0][i] = E[0][i] = 0.0;
        D[1][i] = E[1][i] = 0.0;
        updateFlag[i] = -1;
    }
   
    //Initialization
    //start from step 1
    D[0][seed] = teleport;
    updateFlag[seed] = 1;
    curQueue.push_back(seed);
    
    std::vector<Edge>* neiList = getNeighbors(seed);
    for(auto& edge : *neiList){
        unsigned neighborId = edge.neighborId;
        E[0][neighborId] += (1 - teleport) / neiList->size();
        if(updateFlag[neighborId] == -1)
            curQueue.push_back(neighborId);
        updateFlag[neighborId] = 1;
    }

    //Iteration
    bool converage = false;
    unsigned cur = 0;
    int step = 2;//updateFlag : -1 unselected <step un-updated =step updated
    while(!converage){
        std::vector<int>incQueue;
        for(auto& i : curQueue){
            D[1-cur][i] = D[cur][i];
            if(!isHubNode(i) || getHubIndex(i) > seedIndex)
                D[1-cur][i] += E[cur][i] * teleport;

            if(!isHubNode(i) || getHubIndex(i) > seedIndex)
                E[1-cur][i] = 0;
            else
                E[1-cur][i] = E[cur][i];
            updateFlag[i] = step;
        }

        for(auto& i : curQueue){
            if(!isHubNode(i) || getHubIndex(i) > seedIndex){
                std::vector<Edge>* neiList = getNeighbors(i);
                for(auto& edge : *neiList){
                    unsigned neighborId = edge.neighborId;

                    if(updateFlag[neighborId] == step)
                        E[1-cur][neighborId] += E[cur][i] * (1.0 - teleport) / neiList->size();
                    else
                        E[1-cur][neighborId] = E[cur][i] * (1.0 - teleport) / neiList->size();

                    if(updateFlag[neighborId] == -1)
                        incQueue.push_back(neighborId);
                    updateFlag[neighborId] = step;
                }
                //special case when there is no neighbor
            }
        }
        for(auto& node : incQueue)
            curQueue.push_back(node);

        converage = true;
        for(auto& node : curQueue){
            if(E[1-cur][node] > tolerance && (!isHubNode(node) || getHubIndex(node) > seedIndex)){
                converage = false;
                break;
            }
        }
        cur = 1 - cur;
        step++;
#ifdef SHOWITERATION
        if(step % 10 == 0)
            std::cerr << "Step: " << step << " " << D[cur][seed] << std::endl;
#endif
    }

    double *res = D[cur];
    for(unsigned i = 0; i < 2; i++){
        if(i != cur)
            delete [] D[i];
        delete [] E[i];
    }
    delete [] D;
    delete [] E;
    delete [] updateFlag;
    return res;
}


/*
 * Single hubnode webskeleton computation
 * Can be improved
 */
double*
hubRWR::basicDP_s(unsigned hid){
    double **D = new double*[2];
    double **E = new double*[2];
    int seedIndex = getHubIndex(hid);
    /* std::cerr << "Allocating memory" << std::endl; */
    for(int i = 0; i < 2; i++){
        D[i] = new double[userNum];
        E[i] = new double[userNum];
        memset(D[i], 0, sizeof(double) * userNum);
        memset(E[i], 0, sizeof(double) * userNum);
    }
    E[0][hid] = 1.0;
    // std::cerr << "Iteration start" << std::endl;
    //iteration
    int cur = 0;
    int step = 1;
    bool converage = false;
    // int cnt = 0;
    // std::cerr << "IDX: " <<idx << std::endl;
    while(!converage){
        converage = true;
        for(unsigned i = 0; i < userNum; i++)
        {
            if(isHubNode(i) && getHubIndex(i) < seedIndex)
                continue;
            E[1-cur][i] = D[1-cur][i] = 0.0;
            std::vector<Edge>* neiList = getNeighbors(i);
            for(auto& edge : *neiList){
                unsigned neighborId = edge.neighborId;

                E[1-cur][i] += E[cur][neighborId] * (1 - teleport) / neiList->size();
                D[1-cur][i] += D[cur][neighborId] * (1 - teleport) / neiList->size();

                if(converage && E[1-cur][i] > tolerance)
                    converage = false;
            }
        }

        D[1-cur][hid] += teleport;

        cur = 1 - cur;
        step++;
#ifdef SHOWITERATION
        if(step % 10 == 0)
            std::cerr << "Step:" << step << std::endl;
#endif
    }
#ifdef SHOWITERATION
    std::cout << "Terminated in " << step << " steps" << std::endl;
#endif
    /* for(unsigned i = 0; i < userNum; i++) */
    /*     D[cur][i] += E[cur][i]; */

    double * res = D[cur];
    for(int i = 0; i < 2; i++){
        delete [] E[i];
    }
    delete [] E;
    delete [] D[1-cur];
    delete [] D;
    return res;
}
