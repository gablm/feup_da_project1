#include "../headers/Manager.h"
#include <limits.h>

/**
 * Finds the farthest augmenting path between the source and the sink
 * @note Complexity: O(n * V * E) where n is the number of reservoirs
 * @return Flow of the path
*/
double Manager::FarthestAugmentingPath(std::list<Edge*>& biggestPath, Vertex*& last)
{
    // the values for the final flow
    double finalFlow = 0;
    int actualTopDistance = 0;

    // Go through each reservoir and detect more distant path
    for (const auto& [code, reservoir] : reservoirs) {

        // Reset the values for that reservoir
        int topDistance = 0;
        double newFlow = 0;
        Vertex* fakeLast;

        // find reservoir
        Vertex* source = network.findVertex(reservoir);
        // Calculate already sending flow
        double alreadySending=0;
        for (Edge* e : source->getAdj()){
            alreadySending+=e->getFlow();
        }

        // Create queue with reservoir and remaining sending flow
        std::queue<std::pair<Vertex*, double>> q;
        q.push({source, reservoir->getMaxDelivery()-alreadySending});

        // Clean the graph 
        for (Vertex* v : network.getVertexSet()) {
            v->setPath(nullptr);
            v->setDist(1);
        }

        while (!q.empty()) {
            Vertex* v = q.front().first;
            double flow = q.front().second;
            q.pop();
            if (!flow) continue;

            // Save info of the last visited City
            if (v->getInfo()->getCode()[0] == 'C') {
                double totalIncoming = 0;
                for (Edge* e : v->getIncoming()){
                    totalIncoming+=e->getFlow();
                }
                double availableSpace = dynamic_cast<City*>(v->getInfo())->getDemand()-totalIncoming;
                if (availableSpace){
                    fakeLast = v;
                    topDistance = v->getDist();
                    newFlow = std::min(flow, availableSpace);
                }
            }

            // See outgoing edges
            for (Edge* e : v->getAdj()) {
                Vertex* d = e->getDest();
                Edge* reverseEdge = e->getReverse();
                // Only follow direct path if there is space and no reverse edge
                // Or no flow in reverse Edge
                if ((d->getPath() == nullptr) &&
                    (e->getFlow() < e->getWeight()) && (!reverseEdge || reverseEdge->getFlow()==0)) {
                    d->setPath(e);
                    double possibleFlow = std::min(flow, e->getWeight() - e->getFlow());
                    d->setDist(v->getDist() + 1);
                    q.push({d, possibleFlow});
                }
            }


            for (Edge* e : v->getIncoming()) {
                Vertex* o = e->getOrig();
                // Only if path has already flow 
                // So its possible to redirect water
                if ((o->getPath() == nullptr) && (e->getFlow() > 0)) {
                    double possibleFlow;
                    o->setPath(e);

                    if (e->getReverse()){
                        possibleFlow = std::min(flow, e->getWeight()+e->getFlow());
                        o->setDist(v->getDist());
                    }else{
                        possibleFlow = std::min(flow, e->getFlow());
                        o->setDist(v->getDist() -1);
                    }
                    q.push({o, possibleFlow});
                }
            }
        }

        // Check if distant path from reservoir is the biggest
        if (topDistance > actualTopDistance || (topDistance == actualTopDistance && newFlow > finalFlow)) {
            std::list<Edge*> newBiggestPath;
            Vertex* node = fakeLast;
            while (node != source) {
                Edge* e = node->getPath();
                newBiggestPath.push_back(e);
                if (e->getOrig() == node){
                    // Backedge
                    node = e->getDest();
                    if (e->getReverse() && newFlow < e->getFlow()){
                        // Edge direction is not reversed then flow in that pipe is decreased
                        // So update distance because it increaseses empty space
                        topDistance--;
                    }
                }else{
                    node = e->getOrig();
                }
            }

            if (topDistance > actualTopDistance || (topDistance == actualTopDistance && newFlow > finalFlow)) {
                // update the values
                biggestPath = newBiggestPath;
                actualTopDistance = topDistance;
                last = fakeLast;
                finalFlow = newFlow;
            }
        }
    }
    return finalFlow;
}

/**
 * Balances the network in order to reduce the variation of capacity/flow in the graph.
 * @note Complexity: O(n * V * E^2), n being the number of reservoirs
 * @return Pair of tuples containg stats before and after balancing the network.
*/
std::pair<AnaliseResult, AnaliseResult> Manager::balanceNetwork()
{
    CalculateMaxFlow(network);
    AnaliseResult initial = AnalyzeBalance();

    // Clean graph
    for (Vertex* v : network.getVertexSet()) {
        for (Edge* e : v->getAdj()) {
            e->setFlow(0);
        }
    }
    std::unordered_set<std::string> reservoirs;
    std::list<Edge*> biggestPath;
    Vertex* last;
    double flow;
    while (flow = FarthestAugmentingPath(biggestPath, last)) {
        for (Edge* e : biggestPath) {
            if (last == e->getDest()) {
                last = e->getOrig();
                e->setFlow(e->getFlow() + flow);
            } else {
                last = e->getDest();
                if (flow > e->getFlow()){
                    e->getReverse()->setFlow(flow-e->getFlow());
                    e->setFlow(0);
                }else{
                    // Detect if flow in bidirected changes direction
                    e->setFlow(e->getFlow() - flow);
                }
            }
        }
        reservoirs.insert(last->getInfo()->getCode());
        biggestPath.clear();
    }

    AnaliseResult final = AnalyzeBalance();

    return {initial, final};
}

/**
 * Analizes the balance of a graph.
 * @note Complexity: O(n * V * E^2), n being the number of reservoirs
 * @return Tuple about differences between capacity and flow <average, variance, max difference>
*/
std::tuple<double, double, double> Manager::AnalyzeBalance() {
    double biggest = std::numeric_limits<double>::min();
    double smallest = std::numeric_limits<double>::max();
    double sum = 0;
    double numEdges = numberEdges;
	double variance = 0;
	double mean = 0;
	double maxDiff = std::numeric_limits<double>::min();

    for (Vertex* v : network.getVertexSet()) {
        for (Edge* e : v->getAdj()) {
            double diff = e->getWeight() - e->getFlow();
			if (diff > maxDiff)
				maxDiff = diff;
            sum += diff;
            biggest = std::max(biggest, diff);
            if (!e->getReverse() || !e->getReverse()->getFlow()) {
                smallest = std::min(smallest, diff);
            }
        }
    }

	mean = sum / numEdges;
	for (Vertex *v : network.getVertexSet())
	{
		for (Edge* e : v->getAdj()) {
            double prc = (e->getWeight() - e->getFlow() - mean);
            variance += prc * prc;
        }
	}
	variance = variance / numEdges;
    return {mean, variance, maxDiff};
}