#include "graph.h"

void Graph::addEdge(unsigned u, unsigned v, double weight){
    Edge ed = {weight, v};
    if(u >= userNum){
        std::cerr << "Out of Index: " << u << "/" << userNum << std::endl;
        return;
    }
    edge[u].push_back(ed);
}

std::vector<Edge>*
Graph::getNeighbors(unsigned u){
    if(u >= userNum){
        std::cerr << "Out of Index: " << u << "/" << userNum << std::endl;
        return NULL;
    }
    return &(edge[u]);
}

int
Graph::getNeiNum(unsigned u){
    if(u >= userNum){
        std::cerr << "Out of Index: " << u << "/" << userNum << std::endl;
        return -1;
    }
    return edge[u].size();
}

void
Graph::setUserNum(unsigned userNum){
    this -> userNum = userNum;
    edge = new std::vector<Edge>[userNum];
    outDegree = new unsigned[userNum];
}

int
Graph::setOutDegree(unsigned uid, unsigned degree){
    if(uid >= this->userNum)
        return 1;
    outDegree[uid] = degree;
    return 0;
}

unsigned
Graph::getOutDegree(unsigned uid){
    if(uid >= this->userNum){
        std::cerr << "User Id out of range." << std::endl;
        return 0;
    }
    return outDegree[uid];
}

hubGraph::hubGraph(){
    hubIndex = NULL;
}

hubGraph::~hubGraph(){
    if(hubIndex != NULL)
        delete[] hubIndex;
}

std::vector<unsigned>
hubGraph::getHubNodes(){
    return hubNodes;
}

void
hubGraph::setUserNum(unsigned userNum){
    this -> userNum = userNum;
    edge = new std::vector<Edge>[userNum];
    outDegree = new unsigned[userNum];
    //for hubGraph
    hubIndex = new int[userNum];
    for(unsigned i = 0; i < userNum; i++){
        hubIndex[i] = -1;
    }
}

int
hubGraph::addHubNode(unsigned nodeId, int level){
    if(nodeId >= userNum){
        return -1;
    }
    hubIndex[nodeId] = level;
    hubNodes.push_back(nodeId);
    return 0;
}

int
hubGraph::getHubIndex(unsigned nodeId){
    if(nodeId >= userNum){
        return -2;
    }
    return hubIndex[nodeId];
}

bool
hubGraph::isHubNode(unsigned nodeId){
    if(nodeId >= userNum){
        return false;
    }
    return hubIndex[nodeId] != -1;
}