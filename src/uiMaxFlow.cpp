#include "../headers/Ui.h"
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

/**
 * Searches a string in another string.
 * This search is case-insensitive.
 * @param one The string to search
 * @param two The string to be searched
 * @return If there are any matches
*/
bool UI::strFind(const std::string &one, const std::string &two) {
	auto it = std::search(one.begin(), one.end(), two.begin(), two.end(),
    	[](unsigned char a, unsigned char b) { return std::toupper(a) == std::toupper(b);}
  		);
  	return (it != one.end());
}

/**
 * Searches the cities for partial matches with the search term.
 * @param manager Manager containing the city list
 * @param searchTerm The term being searched
 * @note Complexity: O(n)
 * @return Map of matches
*/
std::unordered_map<std::string, int> getSearchVertexes(Manager &manager, std::string searchTerm)
{
	std::unordered_map<std::string, int> result;

	if (searchTerm.empty())
		return result;
	for (auto city : manager.cities)
	{
		if (UI::strFind(city.second->getName(), searchTerm))
			result[city.first] = manager.maxFlows[city.first];
		else if (UI::strFind(city.second->getCode(), searchTerm))
			result[city.first] = manager.maxFlows[city.first];
	}
	return result;
}

/**
 * Gets the flow for a specific vertex of the graph
 * @note Complexity: O(IE) where IE is the number of incoming edges
*/
int getFlow(Vertex *elem)
{
	int total = 0;
	for (auto i : elem->getIncoming())
		total += i->getFlow();
	return total;
}

/**
 * Saves the maxFlow information to a file near the executable path
 * (static path: "./maxFlowOutput.txt").
 * @note Complexity: O(n)
*/
void saveMaxFlow(Graph &graph, std::unordered_map<std::string, int> &lst, int maxFlow)
{
	std::ofstream out("./maxFlowOutput.txt", std::ofstream::trunc);

	auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

	out
	<< "Information saved at " << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << "\n"
	<< "The max flow for the network is: " << maxFlow << "\n"
	<< "\n"
	<< "The max flow for each city is the following:\n\n";

	for (auto w : lst)
	{
		City *city = dynamic_cast<City *>(graph.findVertexByCode(w.first)->getInfo());

		if (city == nullptr)
			continue;
		out << city->getCode() << " (" << city->getName() << ")" << " - " << w.second << "\n";
	}
}

/**
 * Displays the max flow for the network and the flow for each city in it.
 * (static path: "./maxFlowOutput.txt").
 * @note Complexity: O(n)
*/
void UI::maxFlowMenu()
{
	std::unordered_map<std::string, int> lst = manager.maxFlows;

	size_t count = 0;
	std::string str;
	std::string search;
	int totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;

	while (1)
    {
        CLEAR;
        std::cout 
		<< "Basic Service Metrics\n"
		<< "\n"
		<< "The max flow for the network is: " << manager.totalNetworkFlow << "\n\n"
		<< "Max flow for all cities" << (search.empty() ? "" : " containing \"" + search + "\"") << ":\n\n";
		if (!lst.empty())
		{
			for (size_t i = count; i < std::min(count + 10, lst.size()); i++)
			{
				auto it = lst.begin();
				std::advance(it, i);
				Element *x = manager.network.findVertexByCode(it->first)->getInfo();
				City *city = dynamic_cast<City *>(x);
				std::cout << x->getCode();
				if (city != nullptr) 
					std::cout << " (" << city->getName() << ")";
				std::cout 
				<< " -> Max flow: " << it->second << "\n";
			}
			std::cout << "\nPage " << (count + 10 - count % 10) / 10 << " of " 
						<< totalPages << "\n";
			std::cout << "Total count: " << lst.size() << "\n";
		} 
		else
		{
			std::cout << "The search for " << str << " returned no results.\n";
		}

		std::cout
		<< "\n"
		<< (lst.empty() ? "" : "[back] - Previous page\t[next] - Next page\n")
		<< (lst.empty() ? "" : "[page (integer)] - Select a specific page\n")
		<< "[reset] Reset search\n" 
		<< (lst.empty() ? "" : "[save] Save displayed information to a file.\n")
		<< "[B] - Back \t\t[Q] - Exit\n"
		<< "\n"
		<< "You can search the max flow for a specific city\n"
		<< "or use one of the commands above\n"
		<< "\n"
        << "$> ";

		getline(std::cin, str);

        if (str == "Q" || str == "q")
		{
			CLEAR;
            exit(0);
		}
		if (str == "B" || str == "b")
			break;

		if (str == "next" && !lst.empty())
		{
			count = count + 10 < lst.size() + (lst.size() + 1 / 10) % 10 ? count + 10 : count;
			continue;
		}

		if (str == "back" && !lst.empty())
		{
			count = count < 10 ? 0 : count - 10;
			continue;
		}

		if (str == "save")
		{
			saveMaxFlow(manager.network, lst, manager.totalNetworkFlow);
			CLEAR;
			std::cout << "Saved current search to \"./maxFlowOutput.txt\".\nPress ENTER to continue...";
			while (std::cin.get() != '\n') { }
			continue;
		}

		if (str.substr(0, 4) == "page")
		{
			if (str.size() <= 5 || lst.empty()) {
				helpMsg("There is no page to change to!", "page [num] if there is results");
				continue;
			}
			int page = atoi(str.substr(5).c_str());
			if (page <= 0 || page > totalPages) {
				helpMsg("That page does not exist!", "page [num] if there is results");
				continue;
			}
			count = (page - 1) * 10;
			continue;
		}

		if (str == "reset")
		{
			search = "";
			lst = manager.maxFlows;
			totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;
			continue;
		}

		if (!str.empty())
		{
			lst = getSearchVertexes(manager, str);
			search = str;
			totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;
			continue;
		}

		helpMsg("Invalid command!", "[next/back/reset/b/q/(search term)]");
    }
}
