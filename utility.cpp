#include "utility.h"

//Do not support threads yet
std::chrono::time_point<std::chrono::system_clock> start, end;

void setStartTime(){
    start = std::chrono::system_clock::now();
}

void setEndTime(){
    end = std::chrono::system_clock::now();
}

double getSeconds(){
    std::chrono::duration<double> elapsed_seconds = end - start;
    return elapsed_seconds.count();
}

int vecWriter(double* vec, unsigned nodeID, unsigned nodeNum, std::string fileName, double threshold, bool app){
    FILE * pFile = NULL;
    if(app)
        pFile = fopen(fileName.c_str(), "ab");
    else
        pFile = fopen(fileName.c_str(), "wb");
    if(pFile == NULL){
        std::cerr << "Open file failed: " << fileName << std::endl;
        return 1;
    }
    int ret = 0;
    unsigned vecSize = 0;
    // for(unsigned i = 0; i < nodeNum; i++)
    //     std::cerr << "From: " << nodeID << " to: " << i << " val: " << vec[i] << std::endl;
    for(unsigned i = 0; i < nodeNum; i++)
        if(vec[i] > threshold)
            vecSize++;
    std::fwrite(&nodeID,  sizeof(unsigned), 1, pFile);
    std::fwrite(&vecSize, sizeof(unsigned), 1, pFile);
    ret += 2;
    for(unsigned i = 0; i < nodeNum; i++)
        if(vec[i] > threshold){
            std::fwrite(&i, sizeof(unsigned), 1, pFile);
#ifdef INT_ROUND
            int t = _double2int(vec[i]);
            std::fwrite(&t, sizeof(int), 1, pFile);
            ret += 2;
#else
            std::fwrite(&vec[i], sizeof(double), 1, pFile);
            ret += 3;
#endif
        }
    fclose(pFile);
    return ret;
}

int _double2int(double value){
    return int(value * intUnit);
}

double _int2double(int value){
    return double(1.0 * value / intUnit);
}