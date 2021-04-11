//
// Created by IdoSarel on 12/09/19.
//

#ifndef WET2_DSEXCEPTIONS_H
#define WET2_DSEXCEPTIONS_H

#include <exception>

class DSException : public std::exception{
};
class InvalidInputException : public  DSException{
public:
    InvalidInputException() = default;
};
class FailureException : public DSException{
public :
    FailureException() = default;
};

#endif //WET2_DSEXCEPTIONS_H
