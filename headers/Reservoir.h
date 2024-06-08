#ifndef FIRST_PROJECT_DA_RESERVOIR_H
#define FIRST_PROJECT_DA_RESERVOIR_H

#include <string>
#include "Element.h"

/**
 * Represents a reservoir in the network.
 * Contains information about its municipality and maximum delivery of water.
*/
class Reservoir : public Element {
    private:
        std::string name;
        std::string municipality;
        int maxDelivery = 0;
    public:
        Reservoir(int id, std::string code, std::string name, std::string municipality, int maxDelivery);
        std::string getName() const;
        std::string getMunicipality() const;
        int getMaxDelivery() const;
};

/**
 * Constructor, fills all varibles within the class.
*/
inline Reservoir::Reservoir(int id, std::string code, std::string name, std::string municipality, int maxDelivery)
    : Element(id, code), name(name), municipality(municipality), maxDelivery(maxDelivery) {}

/**
 * Getter for name
*/
inline std::string Reservoir::getName() const {
    return name;
}

/**
 * Getter for municipality
*/
inline std::string Reservoir::getMunicipality() const {
    return municipality;
}

/**
 * Getter for the maximum delivery of water supported by the reservoir.
*/
inline int Reservoir::getMaxDelivery() const {
    return maxDelivery;
}

#endif //FIRST_PROJECT_DA_RESERVOIR_H
