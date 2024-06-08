#include "../headers/Ui.h"
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>

/**
 * Shows the average, variance and max difference between capacities 
 * and flows of pipes in the network initially and
 * after running a balacing algorithm
 * @note Discarding the balacing algorithm, the page has a complexity of O(n)
*/
void UI::balancedFlowMenu()
{	
	std::string str;
	std::pair<AnaliseResult, AnaliseResult> res = manager.balanceNetwork();

	while (1)
    {
        CLEAR;
        std::cout 
		<< "Basic Service Metrics - Balanced Network\n"
		<< "\n";
			
		std::cout
		<< ">> Base network stats\n"
		<< " Differences between capacities and flows of pipes\n"
		<< "\tAverage:\t" << std::get<0>(res.first) << "\n"
		<< "\tVariance:\t" << std::get<1>(res.first) << "\n"
		<< "\tMax Difference:\t" << std::get<2>(res.first) << "\n"
		<< "\n";
		
		std::cout
		<< ">> Balanced network stats\n"
		<< " Differences between capacities and flows of pipes\n"
		<< "\tAverage:\t" << std::get<0>(res.second) << "\n"
		<< "\tVariance:\t" << std::get<1>(res.second) << "\n"
		<< "\tMax Difference:\t" << std::get<2>(res.second) << "\n"
		<< "\n";

		std::cout
		<< "\n"
		<< "[B] - Back \t\t[Q] - Exit\n"
		<< "\n"
		<< "You can use one of the commands above\n"
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

		helpMsg("Invalid command!", "[next/back/reset/b/q/(search term)]");
    }
}
