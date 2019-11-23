#ifndef _PRCOM_
#define _PRCOM_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>

#include <thread>
#include <mutex>

#include "rwr.h"
#include "utility.h"
#include "comm.h"
#include "construct.h"

using namespace std;

extern mutex mtx;
extern int threadNum;
extern const int maxThread;

extern double teleport;
extern double tolerance;
extern string path;
extern string dataset;
extern int level;
extern int n;
extern int m;
extern double accTime;

extern double storeTolerance;

extern bool wsWriteFlag;
extern bool hubpvWriteFlag;
extern bool pvWriteFlag;

void readConfig();

double maxDiff(double*, double*, unsigned);
double sumUp(double*, unsigned);

//For hubRWR
void loadGraph(hubRWR*);
void loadHubInfo(hubRWR*);
void precom(int threadNum, int numberOfMachine, int idOfMachine);
void threadPrecom(int, int, hubRWR*, unsigned, vector<unsigned>*, unsigned numberOfMachine, unsigned idOfMachine);

void pvPrecom(int threadNum, unsigned numberOfMachine, unsigned idOfMachine);
void threadPvPrecom(int, unsigned, hubRWR*, unsigned numberOfMachine, unsigned idOfMachine);
void loadPrec(hubPPVCstruct*);

#endif
