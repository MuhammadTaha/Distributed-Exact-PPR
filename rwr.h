#ifndef _PPV_
#define _PPV_

#include <queue>
#include <cstring>
#include <map>
#include <algorithm>
#include "graph.h"

class hubRWR: public hubGraph{
protected:
    double tolerance;
    double teleport;
    double *score;
    std::vector<unsigned> valuedNode;
public:
    hubRWR(){
        score = NULL;
    };

    ~hubRWR(){
        if(score != NULL)
            delete [] score;
    };

    void setTeleport(double tel){
        teleport = tel;
    }

    void setTolerance(double tol){
        tolerance = tol;
    }

    double *iteration(unsigned seed);
    
    // unsigned getNoneEmptyEntry(){
    //     unsigned res = 0;
    //     for(int i = 0; i < userNum; i++)
    //         if(score[i] > tolerance * 1e-4)
    //             res++;
    //     return res;
    // }

    //for partial vector pre-computation
    double *selectiveExpansion(unsigned seed);

    //for web skeleton pre-computation
    double* basicDP_s(unsigned seed);
};

#endif
