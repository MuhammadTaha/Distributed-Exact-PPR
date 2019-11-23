#ifndef __CONSTRCT__
#define __CONSTRCT__

#include <cstdio>
#include <string>
#include <vector>
#include <map>

#include "utility.h"

extern double storeTolerance;

//key - value pair
#ifdef INT_ROUND
    // typedef std::pair<unsigned, int> kv;
    typedef std::pair<unsigned, double> kv;
#else
    typedef std::pair<unsigned, double> kv;
#endif

class hubPPVCstruct{
protected:
    //0.15 for default
    double teleport;
    //results
    double* ppv;
    unsigned userNum;
    std::vector<unsigned> hubnode;
    //webSkeleton[ graph id ][ node id ] : hub node index - value
    std::vector<kv>* webSkeleton;
    //hubPartialVector[ graph id ][ hub node index ] : node - value
    std::vector<kv>* hubPartialVector;
    //partialVec[ node id (in root graph) ] : node - value
    std::vector<kv>* partialVec;

    //node with id % machineNum = machineId will be loaded
    unsigned machineId;
    unsigned machineNum;
public:
    hubPPVCstruct(){
        ppv = NULL;
        webSkeleton = NULL;
        hubPartialVector = NULL;
        partialVec = NULL;
        teleport = 0.15;
    }
    void setUserNum(unsigned userNum);

    void setMachineInfo(unsigned machineId, unsigned machineNum);

    unsigned getUserNum(){
        return userNum;
    }

    void addHubnode(unsigned hid);

    void cleanPPV();
    //invoke cleanPPV() before using
    double* getPPV(unsigned seed);

    unsigned countNonEmptyEntry();

    virtual ~hubPPVCstruct();

    virtual void loadWebSkeleton(std::string path);

    virtual void loadHubPartialVector(std::string path);

    virtual void loadPartialVector(std::string path);
};

#endif
