#include "../headers/Ui.h"
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

/**
 * Searches all the stations for partial matches with the search term.
 * @param manager Manager containing the station list
 * @param searchTerm The term being searched
 * @note Complexity: O(n)
 * @return Map of matches
*/
std::unordered_map<std::string, Station *> getSearchPS(Manager &manager, std::string searchTerm)
{
	std::unordered_map<std::string, Station *> result;

	if (searchTerm.empty())
		return result;
	for (auto stat : manager.stations)
	{
		if (UI::strFind(stat.second->getCode(), searchTerm))
			result[stat.first] = stat.second;
	}
	return result;
}

/**
 * Displays the menu showing information about affected sites when removing a specific Pump Station.
 * A user is promped to select the Pump Station and after shown the affected locations.
 * @note Overall has a complexity of O(n)
*/
void UI::removePSMenu()
{
	std::unordered_map<std::string, Station *> lst = manager.stations;
	Station *selected = nullptr;

	size_t count = 0;
	std::string str;
	std::string search;
	int totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;

	while (1)
    {
        CLEAR;
        std::cout 
		<< "Reliability - Temporary removal of a pump station\n"
		<< "\n";
		if (selected == nullptr && !lst.empty())
		{
			std::cout << "Select a pump station to remove and display affected sites:\n\n";
			for (size_t i = count; i < std::min(count + 10, lst.size()); i++)
			{
				auto it = lst.begin();
				std::advance(it, i);
				Station *stat = it->second;
				std::cout 
				<< i << ". " 
				<< stat->getCode() << "\n";
			}
			std::cout << "\nPage " << (count + 10 - count % 10) / 10 << " of " 
						<< totalPages << "\n";
			std::cout << "Total count: " << lst.size() << "\n";
		} 
		else if (selected)
		{
			std::unordered_map<std::string, int> answ = manager.rmPS[selected->getCode()];
			std::cout << "Affected sites when removing \"" << selected->getCode() << "\":\n\n";
			for (auto vtx : answ)
			{
				std::cout << vtx.first
				<< "\n Old flow: " << manager.maxFlows[vtx.first]
				<< "\n New flow: " << vtx.second 
				<< "\n\n";
			}
			std::cout << "Total count: " << answ.size() << "\n";
		}
		else
		{
			std::cout
			<< "Select a pump station to remove and display affected sites:\n"
			<< "\n"
			<< "The search for " << str << " returned no results.\n";
		}

		std::cout
		<< "\n"
		<< (lst.empty() || selected != nullptr ? "" : "[back] - Previous page\t[next] - Next page\n")
		<< (lst.empty() || selected != nullptr ? "" : "[page (integer)] - Select a specific page\n")
		<< (lst.empty() || selected != nullptr ? "" : "[select (number)] Select a pump station\n")
		<< "[reset] Reset search/selection\n"
		<< "[B] - Back \t\t[Q] - Exit\n"
		<< "\n"
		<< (lst.empty() || selected != nullptr ? "You can " : "You can search for a specific pump station\nor ")
		<< "use one of the commands above\n"
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

		if (str.substr(0, 4) == "page" && !lst.empty())
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

		if (str.substr(0, 6) == "select")
		{
			if (str.size() <= 7)
			{
				helpMsg("You must provide a number.", "select [num]");
				continue;
			}
			int i = atoi(str.substr(6).c_str());
			if (i < 0 || i >= lst.size()) {
				helpMsg("This number is not a valid option!", "select [num]");
				continue;
			}
			auto it = lst.begin();
			std::advance(it, i);
			selected = it->second;
			continue;
		}

		if (str == "reset")
		{
			search = "";
			lst = manager.stations;
			selected = nullptr;
			totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;
			continue;
		}

		if (!str.empty())
		{
			lst = getSearchPS(manager, str);
			search = str;
			totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;
			continue;
		}

		helpMsg("Invalid command!", "[next/back/reset/b/q/(search term)]");
    }
}