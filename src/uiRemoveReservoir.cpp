#include "../headers/Ui.h"
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

/**
 * Searches all the reservoirs for partial matches with the search term.
 * @param manager Manager containing the reservoirs list
 * @param searchTerm The term being searched
 * @note Complexity: O(n)
 * @return Map of matches
*/
std::unordered_map<std::string, Reservoir *> getSearchReservs(Manager &manager, std::string searchTerm)
{
	std::unordered_map<std::string, Reservoir *> result;

	if (searchTerm.empty())
		return result;
	for (auto reserv : manager.reservoirs)
	{
		if (UI::strFind(reserv.second->getName(), searchTerm))
			result[reserv.first] = reserv.second;
		else if (UI::strFind(reserv.second->getCode(), searchTerm))
			result[reserv.first] = reserv.second;
		else if (UI::strFind(reserv.second->getMunicipality(), searchTerm))
			result[reserv.first] = reserv.second;
	}
	return result;
}

/**
 * Extremely similar menu to removePSMenu, but instead displays
 * information about affected sites when removing a specific Reservoir.
 * @note Overall has a complexity of O(n)
*/
void UI::removeReservoirMenu()
{
	std::unordered_map<std::string, Reservoir *> lst = manager.reservoirs;
	Reservoir *selected = nullptr;

	size_t count = 0;
	std::string str;
	std::string search;
	int totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;

	while (1)
    {
        CLEAR;
        std::cout 
		<< "Reliability - Temporary removal of a reservoir\n"
		<< "\n";
		if (selected == nullptr && !lst.empty())
		{
			std::cout << "Select a reservoir to remove and display affected sites:\n\n";
			for (size_t i = count; i < std::min(count + 10, lst.size()); i++)
			{
				auto it = lst.begin();
				std::advance(it, i);
				Reservoir *reserv = it->second;
				std::cout 
				<< i << ". " 
				<< reserv->getCode() << " - " 
				<< reserv->getName() << " (" << reserv->getMunicipality() << ") "
				<< "Max delivery: " << reserv->getMaxDelivery() << "\n";
			}
			std::cout << "\nPage " << (count + 10 - count % 10) / 10 << " of " 
						<< totalPages << "\n";
			std::cout << "Total count: " << lst.size() << "\n";
		} 
		else if (selected)
		{
			std::vector<std::tuple<Vertex *, double, double>> answ = manager.removeReservoir(selected);
			std::cout << "Affected sites when removing \"" << selected->getCode() << " " << selected->getName() << "\":\n\n";
			for (auto vtx : answ)
			{
				Element *e = std::get<0>(vtx)->getInfo();
				std::cout << e->getCode() 
				<< "\n Old flow: " <<  std::get<1>(vtx) 
				<< "\n New flow: " << std::get<2>(vtx) << "\n\n";
			}
			std::cout << "Total count: " << answ.size() << "\n";
		}
		else
		{
			std::cout
			<< "Select a reservoir to remove and display affected sites:\n"
			<< "\n"
			<< "The search for " << str << " returned no results.\n";
		}

		std::cout
		<< "\n"
		<< (lst.empty() || selected != nullptr ? "" : "[back] - Previous page\t[next] - Next page\n")
		<< (lst.empty() || selected != nullptr ? "" : "[page (integer)] - Select a specific page\n")
		<< (lst.empty() || selected != nullptr ? "" : "[select (number)] Select a reservoir\n")
		<< "[reset] Reset search/selection\n"
		<< "[B] - Back \t\t[Q] - Exit\n"
		<< "\n"
		<< (lst.empty() || selected != nullptr ? "You can " : "You can search for a specific reservoir\nor ")
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
			lst = manager.reservoirs;
			selected = nullptr;
			continue;
		}

		if (!str.empty())
		{
			lst = getSearchReservs(manager, str);
			search = str;
			totalPages = (lst.size() + 9 - (lst.size() - 1) % 10) / 10;
			continue;
		}

		helpMsg("Invalid command!", "[next/back/reset/b/q/(search term)]");
    }
}