#ifndef _GRAPH_
#define _GRAPH_

#include <vector>
#include <iostream>

struct Edge{
    double		weight;
    unsigned	neighborId;
};

class Graph{
protected:
    unsigned userNum;	//0-based
    std::vector<Edge>*edge;
    unsigned * outDegree;

public:

    //directed edge
    void addEdge(unsigned u, unsigned v, double weight = 1.0);

	std::vector<Edge>* getNeighbors(unsigned u);

    int getNeiNum(unsigned u);

    virtual void setUserNum(unsigned userNum);

    unsigned getUserNum(){return userNum;}

    int setOutDegree(unsigned uid, unsigned degree);

    unsigned getOutDegree(unsigned uid);

    Graph(unsigned userNum);

    Graph(){
        edge = NULL;
        outDegree = NULL;
    }

    virtual ~Graph(){
        if(edge != NULL)
            delete [] edge;
        if(outDegree != NULL)
            delete [] outDegree;
    }
};

class hubGraph : public Graph
{
protected:
    std::vector<unsigned> hubNodes;
    //hubIndex = -1 : non hubnode
    int* hubIndex;
public:
    hubGraph();
    ~hubGraph();
    void setUserNum(unsigned userNum);
    std::vector<unsigned> getHubNodes();
    int addHubNode(unsigned nodeId, int level);
    int getHubIndex(unsigned nodeId);
    bool isHubNode(unsigned nodeId);
};

#endif
