#include "../headers/Manager.h"
#include <forward_list>
#include <tuple>
using namespace std;

/**
 * This algorithm checks save which elements could potentially be affected
 * by the removal of a specific Reservoir, puts them in a set and runs the
 * max flow algorithm for these elements. In the end, it check which flows changes
 * for the potential affected element list.
 * @note Complexity: O(VE^2)
 * @return Vector of tuple <Vertex, old flow, new flow>
*/
std::vector<std::tuple<Vertex *, double, double>> Manager::removeReservoir(Reservoir* reservoir)
{
    CalculateMaxFlow(network);

    // Clean all the edges
    for (Vertex* v : network.getVertexSet()){
        for (Edge* e : v->getAdj()){
            e->setSelected(false);
        }
        v->setVisited(false);
    }

    // Do BFS and store the affected pumping stations
    std::unordered_set<Vertex*> affectedStations;

    std::queue<Vertex*> q;
    q.push(network.findVertex(reservoir));
    network.findVertex(reservoir)->setVisited(true);

    while (!q.empty()){
        Vertex* v = q.front();
        q.pop();

        for (Edge* e : v->getAdj()){
            // Record which pipes are the ones affected
            e->setSelected(true);
            Vertex* d = e->getDest();
            if (!d->isVisited()){
                d->setVisited(true);
                affectedStations.insert(d);
                q.push(d);
            }
        }
    }

    // Detect which stations/cities are the connecting ones
    for (Vertex* station : affectedStations){
        for (Edge* e : station->getAdj()){
            e->setFlow(0);
        }
        bool connectingStation = false;
        for (Edge* e : station->getIncoming()){
            // If has not selected incoming edge then is connecting
            if (!e->isSelected()){
                connectingStation = true;
            }else{
                // Reset the flow of the selected edges
                e->setFlow(0);
            }
        }
        if (connectingStation) q.push(station);
    }

    // Use BFS to get the separating vertices
    std::unordered_set<Vertex*> borderVertices;

    while (!q.empty()){
        Vertex* v = q.front();
        q.pop();

        if (v->getIncoming().size()){
            for (Edge* e : v->getIncoming()){
                Vertex* o = e->getOrig();

                affectedStations.insert(o);
                double flow = e->getFlow();
                e->setFlow(0);
                if (o->isVisited()){
                    continue;
                }

                // If edge is full
                if (flow == e->getWeight()){
                    borderVertices.insert(o);
                }else{
                    // If not remove it from being border
                    if (borderVertices.find(o)!=borderVertices.end()) borderVertices.erase(o);
                    // And add it to queue
                    o->setVisited(true);
                    q.push(o);
                }
            }
        }else{
            // Its a reservoir and then a border Vertice
            borderVertices.insert(v);
        }

        for (Edge* e : v->getAdj()){
            Vertex* d = e->getDest();
            double flow = e->getFlow();
            e->setFlow(0);
            if (d->isVisited()){
                continue;
            }
            if (flow ||(d->getInfo()->getCode()[0] == 'C') ){
                affectedStations.insert(d);
                d->setVisited(true);
                q.push(d);
            }
        }
    }
    
    // Super source for edmondsKarp Algorithm
    Element* superSource = new Element(0, "SS");
    network.addVertex(superSource);

    // Add super sink
    Element* superSink = new Element(0, "ST");
    network.addVertex(superSink);

    for (const auto& [code, sink]  : cities){
        network.addEdge(sink, superSink, sink->getDemand());
    }

    // Connect superSource to the different border Vertices
    for (Vertex* v: borderVertices){

        std::string code = v->getInfo()->getCode();
        double capacity;
        switch (code[0])
        {
        case 'P':
            for (Edge* e : v->getIncoming()){
                capacity += e->getFlow();
            }
            break;
        case 'R':
            capacity = dynamic_cast<Reservoir*>(v->getInfo())->getMaxDelivery();
            break;
        default:
            break;
        }
        network.addEdge(superSource, v->getInfo(), capacity);
    }

    auto ssiIter = affectedStations.insert(network.findVertex(superSink));
    auto ssoIter = affectedStations.insert(network.findVertex(superSource));

    EdmondsKarp(network, superSource, superSink, affectedStations);

    // Clean network
    network.removeVertex(superSource);
    network.removeVertex(superSink);
	affectedStations.erase(ssiIter.first);
	affectedStations.erase(ssoIter.first);

    delete superSink;
    delete superSource;

	std::vector<std::tuple<Vertex *, double, double>> result;
	for (auto aff : affectedStations)
	{
		City *city = dynamic_cast<City *>(aff->getInfo());
		if (city == nullptr)
			continue;
		double total = 0;
		for (auto i : aff->getIncoming())
			total += i->getFlow();
		double oldTotal = maxFlows[city->getCode()];
		if (oldTotal > total)
			result.push_back({aff, oldTotal, total});
	}

	return result;
}

/**
 * For each station, this algorithm removes it and edges related to it from the network
 * and runs the Max flow algorithm on the modified network.
 * After, it compares flows to check which cities are affected by the removal
 * @note Complexity: O(NVE^2) where N is the number of stations
*/
void Manager::maintenancePS()
{
    int n = 0;
    for (const auto& [stationCode, station] : this->stations)
	{
		Graph temp = network.getCopy();
        unordered_map<string,int> flowswithoutps;
        unordered_map<Element*,double> outgoing, incoming;

        auto networkStation = network.findVertex(station);

        temp.removeVertex(station);
        
        CalculateMaxFlow(temp);  //edmonds
        for(const auto& [cityCode, city] : this->cities)
		{
            int flow = 0;
            for(auto in : temp.findVertex(city)->getIncoming())
                flow += in->getFlow();
            flowswithoutps[cityCode] = flow;
        }

        unordered_map<string, int> affectedcities;
        for(const auto& [cityCode,city] : this->cities)
		{
            if(flowswithoutps[cityCode] < maxFlows[cityCode])
                affectedcities[cityCode] = flowswithoutps[cityCode];
        }

        if(!affectedcities.empty())
		{
            rmPS[stationCode] = affectedcities;
        }
        
		for (auto vtx : temp.getVertexSet())
		{
			for (auto edg : vtx->getAdj())
				delete edg;
			delete vtx;
		}
    }
}

/**
 * For each pipe/edge, this algorithm removes it from the network
 * and runs the Max flow algorithm on the modified network.
 * After, it compares flows to check which cities are affected by the removal
 * @note Complexity: O(NV^2E^3) where N is the number of stations
*/
void Manager::maintenancePipes()
{
    int n = 0;
    for(auto element : network.getVertexSet())
	{
        for(auto edge : element->getAdj())
		{
			Graph temp = network.getCopy();

            Element* orig = edge->getOrig()->getInfo();
            Element* dest = edge->getDest()->getInfo();
            double w = edge->getWeight();
            bool reverse = false;

            if(edge->getReverse() != nullptr)
			{
                string code = dest->getCode() + " --- " + orig->getCode();
                if(rmPipelines.find(code) != rmPipelines.end())
				{
					for (auto vtx : temp.getVertexSet())
					{
						for (auto edg : vtx->getAdj())
							delete edg;
						delete vtx;
					}
					continue;
				}
                    
                temp.removeEdge(edge->getDest()->getInfo(),edge->getOrig()->getInfo());
                reverse = true; 
            }
            temp.removeEdge(edge->getOrig()->getInfo(),edge->getDest()->getInfo());
        
            unordered_map<string,int> flowswithoutpipe;
            unordered_map<Element*,double> incoming;

            CalculateMaxFlow(temp);  //edmonds
            for (const auto& [cityCode,city] : this->cities)
			{
                int flow = 0;
                for(auto incoming : temp.findVertex(city)->getIncoming())
                    flow += incoming->getFlow();
                flowswithoutpipe[cityCode] = flow;
            }

			for (auto vtx : temp.getVertexSet())
			{
				for (auto edg : vtx->getAdj())
					delete edg;
				delete vtx;
			}

            unordered_map<string, int> affectedcities;
            for (const auto& [cityCode,city] : this->cities)
			{
                if (flowswithoutpipe[cityCode] < maxFlows[cityCode])
                    affectedcities[cityCode] = flowswithoutpipe[cityCode];
            }

            if(!affectedcities.empty())
			{
                string code = orig->getCode() + " --- " + dest->getCode();
                rmPipelines[code] = affectedcities;
            }
        }
    }
}
