#include "construct.h"

hubPPVCstruct::~hubPPVCstruct(){
    if(ppv != NULL)
        delete [] ppv;
    if(webSkeleton != NULL)
        delete [] webSkeleton;
    if(hubPartialVector != NULL)
        delete [] hubPartialVector;
    if(partialVec != NULL)
        delete [] partialVec;
}

void
hubPPVCstruct::setUserNum(unsigned userNum){
    this->userNum = userNum;
}

void
hubPPVCstruct::setMachineInfo(unsigned machineId, unsigned machineNum){
    this->machineId = machineId;
    this->machineNum = machineNum;
}

void
hubPPVCstruct::addHubnode(unsigned hid){
    hubnode.push_back(hid);
}

void
hubPPVCstruct::cleanPPV(){
    if(ppv == NULL){
        ppv = new double [userNum];
    }
    for(unsigned i = 0; i < userNum; i++)
        ppv[i] = 0.0;
}

double*
hubPPVCstruct::getPPV(unsigned seed){
    if(seed >= userNum){
        fprintf(stderr, "Seed Id: %u out of range (0 - %u).\n", seed, userNum);
        return NULL;
    }

    //partial vector
    for(auto& pair : partialVec[seed]){
#ifdef INT_ROUND
        // ppv[pair.first] = _int2double(pair.second);
        ppv[pair.first] = pair.second;
#else
        ppv[pair.first] = pair.second;
#endif
    }

    // for(auto& pair:hubPartialVector[0]){
    //     ppv[pair.first] = pair.second;
    // }

    for(auto& hubPair : webSkeleton[seed]){
        for(auto& tarPair : hubPartialVector[hubPair.first]){
            unsigned tarId = tarPair.first;
#ifdef INT_ROUND
            // double value = _int2double(tarPair.second) * _int2double(hubPair.second) / teleport;
            double value = tarPair.second * (hubPair.second);
#else
            double value = tarPair.second * hubPair.second / teleport;
#endif
            ppv[tarId] += value;
        }
    }
    return ppv;
}

unsigned
hubPPVCstruct::countNonEmptyEntry(){
    unsigned cnt = 0;
    for(unsigned i = 0; i < userNum; i++)
        if(ppv[i] > 1e-8)
            cnt++;
    return cnt;
}

void
hubPPVCstruct::loadWebSkeleton(std::string path){
    //allocate memory for webSkeleton
    webSkeleton = new std::vector<kv> [userNum];

    FILE * pFile = fopen(path.c_str(), "rb");
    if(pFile == NULL){
        fprintf(stderr, "Open File Failed: %s", path.c_str());
        return;
    }
    while(true){
        unsigned hubID;
        unsigned hubIndex;
        unsigned vecSize;
        if(std::fread(&hubID,  sizeof(unsigned), 1, pFile) != 1){
            break;
        }
        std::fread(&vecSize, sizeof(unsigned), 1, pFile);
        //hubID -> index
        for(unsigned i = 0; i < hubnode.size(); i++){
            if(hubnode[i] == hubID)
                hubIndex = i;
        }
        for(unsigned i = 0; i < vecSize; i++){
            unsigned nodeID;
            std::fread(&nodeID, sizeof(unsigned), 1, pFile);
#ifdef INT_ROUND
            int intvalue;
            std::fread(&intvalue, sizeof(int), 1, pFile);
            double value = _int2double(intvalue) / teleport;
#else
            double value;
            std::fread(&value, sizeof(double), 1, pFile);
#endif
            // webSkeleton[nodeID].push_back(std::make_pair(hubIndex, value));
            if(hubIndex % machineNum == machineId)
                if(value > storeTolerance)
                    webSkeleton[nodeID].push_back(std::make_pair(hubIndex, value));
        }
    }
    fclose(pFile);
}

void
hubPPVCstruct::loadHubPartialVector(std::string path){
    //allocate memory for hubPartialVector
    hubPartialVector = new std::vector<kv> [hubnode.size()];

    FILE * pFile = fopen(path.c_str(), "rb");
    if(pFile == NULL){
        fprintf(stderr, "Open File Failed: %s\n", path.c_str());
        return;
    }

    std::map<unsigned, unsigned> hubID2index;
    for(unsigned i = 0; i < hubnode.size(); i++){
        hubID2index[hubnode[i]] = i;
    }

    while(true){
        unsigned hubID;
        unsigned hubIndex;
        unsigned vecSize;
        if(std::fread(&hubID,  sizeof(unsigned), 1, pFile) != 1){
            break;
        }
        std::fread(&vecSize, sizeof(unsigned), 1, pFile);
        //hubID -> index
        //for(unsigned i = 0; i < hubnode.size(); i++){
        //    if(hubnode[i] == hubID)
        //        hubIndex = i;
        //}
        hubIndex = hubID2index[hubID];
        for(unsigned i = 0; i < vecSize; i++){
            unsigned nodeID;
            std::fread(&nodeID, sizeof(unsigned), 1, pFile);
#ifdef INT_ROUND
            int intvalue;
            std::fread(&intvalue, sizeof(int), 1, pFile);
            double value = _int2double(intvalue);
#else
            double value;
            std::fread(&value, sizeof(double), 1, pFile);
#endif
            // hubPartialVector[hubIndex].push_back(std::make_pair(nodeID, value));
            if(hubIndex % machineNum == machineId)
                if(value > storeTolerance)
                    hubPartialVector[hubIndex].push_back(std::make_pair(nodeID, value));
        }
    }
    fclose(pFile);
}

void
hubPPVCstruct::loadPartialVector(std::string path){
    //allocate memory for PartialVector
    partialVec = new std::vector<kv> [userNum];
    FILE * pFile = fopen(path.c_str(), "rb");
    if(pFile == NULL){
        fprintf(stderr, "Open File Failed: %s", path.c_str());
        return;
    }
    // int acc = 0;
    while(true){
        unsigned sourceID;
        unsigned vecSize;
        if(std::fread(&sourceID,  sizeof(unsigned), 1, pFile) != 1){
            break;
        }
        std::fread(&vecSize, sizeof(unsigned), 1, pFile);
        for(unsigned i = 0; i < vecSize; i++){
            unsigned targetID;
            std::fread(&targetID, sizeof(unsigned), 1, pFile);
#ifdef INT_ROUND
            int intvalue;
            std::fread(&intvalue, sizeof(int), 1, pFile);
            double value = _int2double(intvalue);
#else
            double value;
            std::fread(&value, sizeof(double), 1, pFile);
#endif
            // partialVec[ sourceID ].push_back(std::make_pair(targetID, value));
            if(sourceID % machineNum == machineId)
                if(value > storeTolerance)
                    partialVec[ sourceID ].push_back(std::make_pair(targetID, value));
        }
    }
    fclose(pFile);
}
