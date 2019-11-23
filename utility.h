#ifndef __UTILITY__
#define __UTILITY__

#include <ctime>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <map>
#include <chrono>

//store a double in int by multiply intUnit
#define INT_ROUND
const int intUnit = 1000000000; //1e9

void setStartTime();

void setEndTime();

double getSeconds();
/*
 * results presented by array
 * hubnodeNum is assigned by the graph, not in this file
 *
 * (hubnodeNum "lines"):
 *      nodeID, sizeofVec(unsigned)
 *      (sizeofVec key-value pairs):
 *          nodeid(unsigned) value(double)
 */
int vecWriter(double* vec, unsigned nodeID, unsigned nodeNum, std::string fileName, double threshold, bool app);

int _double2int(double value);

double _int2double(int value);

#endif
