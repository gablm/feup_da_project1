#ifndef FIRST_PROJECT_DA_STATION_H
#define FIRST_PROJECT_DA_STATION_H

#include "Element.h"

/**
 * Represents a station in the network.
 * It does not contain any extra field not present in the parent class.
*/
class Station : public Element {
    public:
		/**
		 * Only available constructor. 
		 * Fills all the required information of the parent class.
		*/
        Station(int id, std::string code) : Element(id, code) {};
};

#endif //FIRST_PROJECT_DA_STATION_H
