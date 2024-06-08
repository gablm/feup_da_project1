#ifndef FIRST_PROJECT_DA_GRAPH_H
#define FIRST_PROJECT_DA_GRAPH_H

#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_map>
#include "Element.h"

class Edge;
class Vertex;
class Graph;

#define INF std::numeric_limits<double>::max()

/************************* Vertex  **************************/

class Vertex {
public:
    Vertex(Element *in);
    inline bool operator<(Vertex& vertex) const;

    Element *getInfo() const;
    std::vector<Edge *> getAdj() const;
    inline bool isVisited() const;
    inline bool isProcessing() const;
    unsigned int getIndegree() const;
    double getDist() const;
    Edge *getPath() const;
    std::vector<Edge *> getIncoming() const;

    inline void setInfo(Element *info);
    inline void setVisited(bool visited);
    inline void setProcesssing(bool processing);
    inline void setIndegree(unsigned int indegree);
    inline void setDist(double dist);
    inline void setPath(Edge *path);
    Edge *addEdge(Vertex *dest, double w);
    inline bool removeEdge(Element *in);
    inline void removeOutgoingEdges();
	
protected:
    Element *info;
    std::vector<Edge *> adj;

    bool visited = false;
    bool processing = false;
    unsigned int indegree;
    double dist = 0;
    Edge *path = nullptr;

    std::vector<Edge *> incoming;

    inline void deleteEdge(Edge *edge);
};

/********************** Edge  ****************************/

class Edge {
public:
    Edge(Vertex *orig, Vertex *dest, double w);

    Vertex *getDest() const;
    double getWeight() const;
    inline bool isSelected() const;
    Vertex *getOrig() const;
    Edge *getReverse() const;
    double getFlow() const;

    inline void setSelected(bool selected);
    inline void setReverse(Edge *reverse);
    inline void setFlow(double flow);
	
protected:
	Vertex *orig;
    Vertex *dest;
    double weight;

    bool selected = false;
    Edge *reverse = nullptr;

    double flow;
};

/********************** Graph  ****************************/

class Graph {
public:
    ~Graph();

    Vertex *findVertex(Element *in) const;
	Vertex *findVertexByCode(std::string code) const;
    inline bool addVertex(Element *in);
    inline bool removeVertex(Element *in);

    inline bool addEdge(Element *sourc, Element *dest, double w);
    inline bool removeEdge(Element *source, Element *dest);
    inline bool addBidirectionalEdge(Element *sourc, Element *dest, double w);

    int getNumVertex() const;
    std::vector<Vertex *> getVertexSet() const;
	Graph getCopy();
protected:
    std::vector<Vertex *> vertexSet;
	std::unordered_map<std::string, Vertex *> vertexMap;

    double ** distMatrix = nullptr;
    int **pathMatrix = nullptr;

    int findVertexIdx(const Element *in) const;
};

/**
 * Creates a copy of the graph
 * @note Complexity: O(V + E)
*/
inline Graph Graph::getCopy()
{
	Graph result;

	for (auto i : vertexSet)
	{
		result.addVertex(i->getInfo());
	}

	for (auto i : vertexSet)
	{
		for (auto j : i->getAdj())
		{
			result.addEdge(i->getInfo(), j->getDest()->getInfo(), j->getWeight());
		}
	}
	return result;
}

/************************* Vertex  **************************/

inline Vertex::Vertex(Element *in) : info(in) {}
/*
 * Auxiliary function to add an outgoing edge to a vertex (this),
 * with a given destination vertex (d) and edge weight (w).
*/
inline Edge *Vertex::addEdge(Vertex *d, double w) {
    auto newEdge = new Edge(this, d, w);
    adj.push_back(newEdge);
    d->incoming.push_back(newEdge);
    return newEdge;
}

/*
 * Auxiliary function to remove an outgoing edge (with a given destination (d))
 * from a vertex (this).
 * Returns true if successful, and false if such edge does not exist.
 */
inline bool Vertex::removeEdge(Element *in) {
    bool removedEdge = false;
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge *edge = *it;
        Vertex *dest = edge->getDest();
        if (dest->getInfo() == in) {
            it = adj.erase(it);
            deleteEdge(edge);
            removedEdge = true; // allows for multiple edges to connect the same pair of vertices (multigraph)
        }
        else {
            it++;
        }
    }
    return removedEdge;
}

/*
 * Auxiliary function to remove an outgoing edge of a vertex.
 */
inline void Vertex::removeOutgoingEdges() {
    auto it = adj.begin();
    while (it != adj.end()) {
        Edge *edge = *it;
        it = adj.erase(it);
        deleteEdge(edge);
    }
}

inline bool Vertex::operator<(Vertex & vertex) const {
    return this->dist < vertex.dist;
}

inline Element *Vertex::getInfo() const {
    return this->info;
}

inline std::vector<Edge *> Vertex::getAdj() const {
    return this->adj;
}

inline bool Vertex::isVisited() const {
    return this->visited;
}

inline bool Vertex::isProcessing() const {
    return this->processing;
}

inline unsigned int Vertex::getIndegree() const {
    return this->indegree;
}

inline double Vertex::getDist() const {
    return this->dist;
}

inline Edge *Vertex::getPath() const {
    return this->path;
}

inline std::vector<Edge *> Vertex::getIncoming() const {
    return this->incoming;
}

inline void Vertex::setInfo(Element *in) {
    this->info = in;
}

inline void Vertex::setVisited(bool visited) {
    this->visited = visited;
}

inline void Vertex::setProcesssing(bool processing) {
    this->processing = processing;
}

inline void Vertex::setIndegree(unsigned int indegree) {
    this->indegree = indegree;
}

inline void Vertex::setDist(double dist) {
    this->dist = dist;
}

inline void Vertex::setPath(Edge *path) {
    this->path = path;
}

inline void Vertex::deleteEdge(Edge *edge) {
    Vertex *dest = edge->getDest();
    // Remove the corresponding edge from the incoming list
    auto it = dest->incoming.begin();
    while (it != dest->incoming.end()) {
        if ((*it)->getOrig()->getInfo() == info) {
            it = dest->incoming.erase(it);
        }
        else {
            it++;
        }
    }
    delete edge;
}

/********************** Edge  ****************************/

inline Edge::Edge(Vertex *orig, Vertex *dest, double w): orig(orig), dest(dest), weight(w) {}

inline Vertex *Edge::getDest() const {
    return this->dest;
}

inline double Edge::getWeight() const {
    return this->weight;
}

inline Vertex *Edge::getOrig() const {
    return this->orig;
}

inline Edge *Edge::getReverse() const {
    return this->reverse;
}

inline bool Edge::isSelected() const {
    return this->selected;
}

inline double Edge::getFlow() const {
    return flow;
}

inline void Edge::setSelected(bool selected) {
    this->selected = selected;
}

inline void Edge::setReverse(Edge *reverse) {
    this->reverse = reverse;
}

inline void Edge::setFlow(double flow) {
    this->flow = flow;
}

/********************** Graph  ****************************/

inline Graph::~Graph() {}

inline int Graph::getNumVertex() const {
    return vertexSet.size();
}

inline std::vector<Vertex *> Graph::getVertexSet() const {
    return vertexSet;
}

inline Vertex *Graph::findVertexByCode(std::string code) const {
	try
	{
		return vertexMap.at(code);
	}
	catch(const std::exception& e)
	{
		return nullptr;
	}
}

/*
 * Auxiliary function to find a vertex with a given content.
 */
inline Vertex *Graph::findVertex(Element *in) const {
	for (auto v : vertexSet)
        if (v->getInfo() == in)
            return v;
    return nullptr;
}

/*
 * Finds the index of the vertex with a given content.
 */
inline int Graph::findVertexIdx(const Element *in) const {
    for (unsigned i = 0; i < vertexSet.size(); i++)
        if (vertexSet[i]->getInfo() == in)
            return i;
    return -1;
}
/*
 *  Adds a vertex with a given content or info (in) to a graph (this).
 *  Returns true if successful, and false if a vertex with that content already exists.
 */
inline bool Graph::addVertex(Element *in) {
	if (findVertex(in) != nullptr)
    	return false;
	
	Vertex *vtx = new Vertex(in);
	vertexMap[in->getCode()] = vtx;
	vertexSet.push_back(vtx);
	return true;
}

/*
 *  Removes a vertex with a given content (in) from a graph (this), and
 *  all outgoing and incoming edges.
 *  Returns true if successful, and false if such vertex does not exist.
 */
inline bool Graph::removeVertex(Element *in) {
    for (auto it = vertexSet.begin(); it != vertexSet.end(); it++) {
        if ((*it)->getInfo() == in) {
            auto v = *it;
            v->removeOutgoingEdges();
            for (auto u : vertexSet) {
                u->removeEdge(v->getInfo());
            }
			vertexMap.erase(v->getInfo()->getCode());
            vertexSet.erase(it);
            delete v;
            return true;
        }
    }
    return false;
}

/*
 * Adds an edge to a graph (this), given the contents of the source and
 * destination vertices and the edge weight (w).
 * Returns true if successful, and false if the source or destination vertex does not exist.
 */
inline bool Graph::addEdge(Element *sourc, Element *dest, double w) {
    auto v1 = findVertexByCode(sourc->getCode());
    auto v2 = findVertexByCode(dest->getCode());
    if (v1 == nullptr || v2 == nullptr)
        return false;
    v1->addEdge(v2, w);
    return true;
}

/*
 * Removes an edge from a graph (this).
 * The edge is identified by the source (sourc) and destination (dest) contents.
 * Returns true if successful, and false if such edge does not exist.
 */
inline bool Graph::removeEdge(Element *sourc, Element *dest) {
    Vertex* srcVertex = findVertexByCode(sourc->getCode());
    if (srcVertex == nullptr) {
        return false;
    }
    return srcVertex->removeEdge(dest);
}

inline bool Graph::addBidirectionalEdge(Element *sourc, Element *dest, double w) {
    auto v1 = findVertexByCode(sourc->getCode());
    auto v2 = findVertexByCode(dest->getCode());
    if (v1 == nullptr || v2 == nullptr)
        return false;
    auto e1 = v1->addEdge(v2, w);
    auto e2 = v2->addEdge(v1, w);
    e1->setReverse(e2);
    e2->setReverse(e1);
    return true;
}

#endif