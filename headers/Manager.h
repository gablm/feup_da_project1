#ifndef FIRST_PROJECT_DA_MANAGER_H
#define FIRST_PROJECT_DA_MANAGER_H

#include "Graph.h"
#include "City.h"
#include "Reservoir.h"
#include "Station.h"
#include <list>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

typedef std::unordered_map<std::string, int> FlowComb;
typedef std::tuple<double, double, double> AnaliseResult;
typedef std::unordered_map<std::string, std::unordered_map<std::string, int>> RmResult;

/**
 * Class containg all the information and methods the manipulate it.
 * Essential module of the project.
*/
class Manager {
    private:
		bool useSmallSet;
        double numberEdges = 0;

    public:
		double totalNetworkFlow = -1;
		FlowComb maxFlows;
        FlowComb deficitcities;
        RmResult rmPS;
        RmResult rmPipelines;

		Graph network;
        std::unordered_map<std::string, Reservoir *> reservoirs;
        std::unordered_map<std::string, Station *> stations;
        std::unordered_map<std::string, City *> cities;
		std::unordered_map<std::string, Element *> allElements;

		inline Manager(bool useSmallSet) : useSmallSet(useSmallSet) {};
		~Manager();

        // Loaders
        void loadReservoirs();
        void loadStations();
        void loadCities();
        void loadPipes();

        // Basic Service Metrics
        double CalculateMaxFlow(Graph &network);
        void maxFlowCities();
        void citiesInDeficit();
        std::pair<AnaliseResult, AnaliseResult> balanceNetwork();

        // Reliability
        std::vector<std::tuple<Vertex *, double, double>> removeReservoir(Reservoir* reservoir);
        void maintenancePS();
        void maintenancePipes();

    private:
        double FarthestAugmentingPath(std::list<Edge*>& biggestPath, Vertex*& last);
        void EdmondsKarp(Graph &network, Element* source, Element* target, const std::unordered_set<Vertex*>& affected);
        double EdmondsBFS(Graph &network, Vertex* source, Vertex* target, const std::unordered_set<Vertex*>& affected);
        std::tuple<double, double, double> AnalyzeBalance();
};

/**
 * Destructor for the manager class.
 * Erases all allocated memory, including the one allocated by the Graph class.
*/
inline Manager::~Manager()
{
	for (const auto& [code, e] : allElements)
		delete e;
	for (auto vtx : network.getVertexSet())
	{
		for (auto edg : vtx->getAdj())
			delete edg;
		delete vtx;
	}
		
}

#endif //FIRST_PROJECT_DA_MANAGER_H
