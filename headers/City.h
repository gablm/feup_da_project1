#ifndef FIRST_PROJECT_DA_CITY_H
#define FIRST_PROJECT_DA_CITY_H

#include "Element.h"
#include <string>

/**
 * Represents a city in the network.
 * Contains information about its population and water demand.
*/
class City : public Element {
    private:
        std::string name;
        int demand;
        int population;
    public:
        City(int id, std::string code, std::string name, int demand, int population);
        int getDemand() const;
        int getPopulation() const;
        std::string getName() const;
};

/**
 * Getter for water demand.
*/
inline int City::getDemand() const {
    return demand;
}

/**
 * Getter for population.
*/
inline int City::getPopulation() const {
    return population;
}

/**
 * Getter for name.
*/
inline std::string City::getName() const {
    return name;
}

/**
 * Only constructor for the class.
 * The variables are self-explanatory
*/
inline City::City(int id, std::string code, std::string name, int demand, int population)
: Element(id, code), name(name), demand(demand), population(population) {}

#endif //FIRST_PROJECT_DA_CITY_H
