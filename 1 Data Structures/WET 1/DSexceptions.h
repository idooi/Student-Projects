//
// Created by IdoSarel on 12/09/19.
//

#ifndef DATASTRUCTURES1_DSEXCEPTIONS_H
#define DATASTRUCTURES1_DSEXCEPTIONS_H

#include <exception>

//using namespace std;
class DSException : public std::exception{
    //DSException() = default;
};
class InvalidInputException : public  DSException{
public:
    InvalidInputException() = default;
};
class FailureException : public DSException{
public :
    FailureException() = default;
};

#endif //DATASTRUCTURES1_DSEXCEPTIONS_H
