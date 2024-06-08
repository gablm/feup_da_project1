#include "../headers/Manager.h"
#include <limits.h>
#include <list>

/**
 * Calculates the Max Flow in the network by using the Edmonds-Kart algorithm.
 * @note Complexity: O(VE^2)
 * @return Max flow for the current network
*/
double Manager::CalculateMaxFlow(Graph &network)
{
    // Add super source
    Element* superSource = new Element(0, "SS");
    network.addVertex(superSource);

    for (const auto& [code, source]  : reservoirs){
        network.addEdge(superSource, source, source->getMaxDelivery());
    }

    // Add super sink
    Element* superSink = new Element(0, "ST");
    network.addVertex(superSink);

    for (const auto& [code, sink] : cities){
        network.addEdge(sink, superSink, sink->getDemand());
    }

    // Clean graph
    for (Vertex* v : network.getVertexSet()){
        for (Edge* e : v->getAdj()){
            e->setFlow(0);
        }
        v->setPath(nullptr);
    }

    std::unordered_set<Vertex*> affected;
    for (auto e : allElements){
        affected.insert(network.findVertex(e.second));
    }
    affected.insert(network.findVertex(superSource));
    affected.insert(network.findVertex(superSink));

    EdmondsKarp(network, superSource, superSink, affected);

    double total = 0;
    Vertex* superSinkVertex = network.findVertex(superSink);
    for (Edge* e : superSinkVertex->getIncoming()){
        total += e->getFlow();
    }

    // Just to check result 
    //std::cout << "Max flow obtained was: " << total << std::endl;

    // Clean graph
    network.removeVertex(superSource);
    network.removeVertex(superSink);
    delete superSink;
    delete superSource;

    return total;
}

/**
 * Edmonds-Karp algorithm.
 * @note Complexity: O(V * E^2)
*/
void Manager::EdmondsKarp(Graph &network, Element* source, Element* target, const std::unordered_set<Vertex*>& affected)
{
    double new_flow;

    auto sc = network.findVertex(source);
    auto tg = network.findVertex(target);

    while ((new_flow = EdmondsBFS(network, sc, tg, affected))){
        Vertex* node = tg;
        while (node != sc){
            Vertex* next;
            Edge* e = node->getPath();
            if (e->getDest() == node){
                next = e->getOrig();
                e->setFlow(e->getFlow() + new_flow);
            }else{
                next = e->getDest();
                e->setFlow(e->getFlow() - new_flow);
            }
            node = next;
        }
    }
}

/**
 * Breadth-first search used in the Edmonds-Kart algorithm to find 
 * the shortest path that has available capacity.
 * @note Complexity: O(V * E)
 * @return Always returns 0
*/
double Manager::EdmondsBFS(Graph &network, Vertex* source, Vertex* target, const std::unordered_set<Vertex*>& affected)
{
    std::queue<std::pair<Vertex*, double>> q;
    q.push({source,std::numeric_limits<double>::max()});

    for (Vertex* v : network.getVertexSet()){
        v->setPath(nullptr);
    }

    while (!q.empty()){
        Vertex* v = q.front().first;
        double flow = q.front().second;
        q.pop();

        for (Edge* e : v->getAdj()){
            Vertex* d = e->getDest();
            if (affected.find(d) == affected.end()) continue;
            Edge* reverseEdge = e->getReverse();
            if ((d->getPath()== nullptr) && (e->getFlow() < e->getWeight()) && (reverseEdge ? reverseEdge->getFlow()==0:true)){
                d->setPath(e);
                double new_flow = std::min(flow, e->getWeight()-e->getFlow());
                if (d == target) return new_flow;
                q.push({d, new_flow});
            }
        }
        for (Edge* e : v->getIncoming()){
            Vertex* o = e->getOrig();
            if (affected.find(o) == affected.end()) continue;
            if ((o->getPath()== nullptr) && (e->getFlow() > 0)){
                o->setPath(e);
                double new_flow = std::min(flow, e->getFlow());
                if (o == target) return new_flow;
                q.push({o, new_flow});
            }
        }
    }
    return 0;
}

/**
 * Calls CalculateMaxFlow and save the results to an unordered_map inside the class.
 * @note Complexity: O(V * E^2)
*/
void Manager::maxFlowCities()
{ 
    totalNetworkFlow = CalculateMaxFlow(network);
    for (const auto& [code, city] : this->cities)
	{
    	int flow = 0;
    	for (auto incoming : network.findVertexByCode(code)->getIncoming())
    	    flow += incoming->getFlow();
		maxFlows[code] = flow;
    }
}

/**
 * Goes through the resulting map from maxFlowCites to save which cities are in deficit of water.
 * @note Complexity: O(n) where n is the number of cities
*/
void Manager::citiesInDeficit()
{ 
    for (const auto& [code, city] : this->cities)
	{
    	int maxFlowCity = maxFlows[code];
    	if (maxFlowCity < city->getDemand())
			deficitcities[code] = city->getDemand() - maxFlowCity;
    }
}
