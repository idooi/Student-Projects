#include "seat.h"
#define MAIN_HALL 100
#define FRONT_REG 500
#define MID_REG   250
#define REAR_REG  0
#define SPECIAL   300
#define GOLDEN    1000
#define DISABLE_PRICE 200
using namespace std ;

//Base Functions: ===========================================

string const Seat::line()  const{
    return to_string(this->_line);
}

string const Seat::chair()  const {
    return to_string(this->_chair);
}
Seat::Seat(const int line, const int chair, const int basePrice, const int priceAdd = 0 ):
_line(line), _chair(chair), _basePrice(basePrice), _priceAdditions(priceAdd){}

const string Seat::price() const{
    return to_string(this->_basePrice + this->_priceAdditions);
}

const string Seat::location() const{
    return "line: " + this->line() + ", chair: " + this->chair() ;
}

// Abstract types: ===========================================

MainHallSeat::MainHallSeat(const int line, const int chair, const int basePrice, const int priceAdd = 0 ) :
    Seat(line, chair, basePrice, priceAdd + MAIN_HALL)
{}

RegularSeat::RegularSeat(const char area, const int line, const int chair, const int basePrice, const int priceAdd = 0):
MainHallSeat(line, chair, basePrice, priceAdd), _area(area)
{}

const string RegularSeat::location() const {
    string locStr = "-> area: " ;
    locStr += this->_area;
    locStr += ", " + Seat::location();
    return locStr;
}

SpecialSeat::SpecialSeat(const int line, const int chair, const int basePrice, const int priceAdd = 0 ):
        MainHallSeat(line, chair, basePrice, priceAdd + SPECIAL)
{}

// Actual types: ================================================

FrontRegularSeat::FrontRegularSeat(const char area, const int line, const int chair, const int basePrice):
        RegularSeat(area, line, chair, basePrice, FRONT_REG)
{}

const string FrontRegularSeat::location() const {
    return "Front" + RegularSeat::location();
}
// Middle Seat:

MiddleRegularSeat::MiddleRegularSeat(const char area, const int line, const int chair, const int basePrice):
        RegularSeat(area, line, chair, basePrice, MID_REG)
{}

const string MiddleRegularSeat::location() const {
    return "Middle" + RegularSeat::location();
}

// Rear:

RearRegularSeat::RearRegularSeat(const char area, const int line, const int chair, const int basePrice):
        RegularSeat(area, line, chair, basePrice, REAR_REG)
{}

const string RearRegularSeat::location() const {
    return "Rear" + RegularSeat::location();
}

// GoldenCircleSeat:

GoldenCircleSeat::GoldenCircleSeat(const int line, const int chair, const int basePrice):
        SpecialSeat(line, chair, basePrice, GOLDEN)
{}

const string GoldenCircleSeat::location() const {
    return "Golden Circle-> " + Seat::location();
}

// DisablePodiumSeat:

DisablePodiumSeat::DisablePodiumSeat(const int line, const int chair, const int basePrice):
        SpecialSeat(line, chair, basePrice)
{}

const string DisablePodiumSeat::location() const {
    return "Disable Podium-> " + Seat::location();
}

const string DisablePodiumSeat::price() const {
    return to_string(DISABLE_PRICE);
}

// GreenRoomSeat:

GreenRoomSeat::GreenRoomSeat(const int line, const int chair, const int basePrice ):
        Seat(line, chair, basePrice)
{}

const string GreenRoomSeat::price() const {
    throw NoPrice() ;
}

const string GreenRoomSeat::location() const {
    return "Green Room-> " + Seat::location();
}

//================================

const char* NoPrice::what() const noexcept {
    return "Not For Sale !";
    
}