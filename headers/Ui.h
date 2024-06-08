#ifndef FIRST_PROJECT_DA_UI_H
#define FIRST_PROJECT_DA_UI_H

#include "Manager.h"
#include <chrono>

#ifdef __linux__
# define CLEAR (void)system("clear")
#else
# define CLEAR (void)system("cls")
#endif

/**
 * Class containing the methods used for displaying the UI.
 * The Ui itself accessed a Manager to get what information to display
*/
class UI {
	private:
		long loadtime;
		Manager manager;
	public:
		UI(bool useSmallSet);

		void mainMenu();

		// Basic Service
		void maxFlowMenu();
		void meetDemandsMenu();
		void balancedFlowMenu();

		// Reliability
		void removeReservoirMenu();
		void removePSMenu();
		void removePipeMenu();

		// Utils
		void testMenu();
		static bool strFind(const std::string &one, const std::string &two);
		void helpMsg(std::string error, std::string usage);
};

#endif
