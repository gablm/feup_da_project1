#ifndef FIRST_PROJECT_DA_ELEMENT_H
#define FIRST_PROJECT_DA_ELEMENT_H

#include <string>

/**
 * Base class for all elements in the network.
 * All elements contain an id and a code.
*/
class Element {
    private:
        int id;
        std::string code;
    public:
        Element(int id, std::string code);
        int getId() const;
        virtual std::string getCode() const;
};

/**
 * Single constructor of this class.
 * Only used in contructors of derived classes.
*/
inline Element::Element(int id, std::string code)
    : id(id), code(code) {}

/**
 * Getter for code.
*/
inline std::string Element::getCode() const {
    return code;
}

/**
 * Getter for id.
*/
inline int Element::getId() const {
    return id;
}

#endif //FIRST_PROJECT_DA_ELEMENT_H
