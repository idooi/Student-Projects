#ifndef _H_SEAT
#define _H_SEAT

#include <iostream>
#include <string>
using std::string;
using std::exception;


// ---------------------------------------------
class NoPrice: public exception
{
public:
    NoPrice() = default;
    virtual const char* what() const noexcept override ;
    ~NoPrice() override = default;
};
// ---------------------------------------------
class Seat
{
protected:

        int _line;
        int _chair;
        int _basePrice;
        int _priceAdditions = 0;

    Seat(int line, int chair, int basePrice, int id);

public:

    const string line()  const;
    const string chair() const;
    virtual const string location() const;
    virtual const string price()    const ;
    virtual ~Seat() = default;

};

// ---------------------------------------------
class GreenRoomSeat: public Seat
{
public:
    GreenRoomSeat(int line, int chair, int basePrice = 0);
    const string location() const override ;
    const string price() const override ;
    ~GreenRoomSeat() override = default;
};

// ---------------------------------------------
class MainHallSeat: public Seat
{
protected:
    MainHallSeat(int line, int chair, int basePrice, int id);
    ~MainHallSeat() override = default;
};

// ---------------------------------------------
class SpecialSeat: public MainHallSeat
{
protected:
    SpecialSeat(int line, int chair, int basePrice, int id);
    ~SpecialSeat() override = default;
};

// ---------------------------------------------
class GoldenCircleSeat: public SpecialSeat
{
public:
    GoldenCircleSeat(int line, int chair, int basePrice);
    const string location() const override ;
    ~GoldenCircleSeat() override = default;
};

// ---------------------------------------------
class DisablePodiumSeat: public SpecialSeat
{
public:
    DisablePodiumSeat(int line, int chair, int basePrice = 0);
    const string location() const override ;
    const string price() const override ;
    ~DisablePodiumSeat() override = default;
};

// ---------------------------------------------
class RegularSeat: public MainHallSeat
{
protected:
    char _area;
    RegularSeat(char area, int line, int chair, int basePrice, int priceAdd);
    const string location() const override ;
    ~RegularSeat() override = default;
};

// ---------------------------------------------
class FrontRegularSeat: public RegularSeat
{
public:
    FrontRegularSeat(char area, int line, int chair, int basePrice);
    const string location() const override ;
    ~FrontRegularSeat() override = default;
};

// ---------------------------------------------
class MiddleRegularSeat: public RegularSeat
{
public:
    MiddleRegularSeat(char area, int line, int chair, int basePrice);
    const string location() const override;
    ~MiddleRegularSeat() override = default;
};

// ---------------------------------------------
class RearRegularSeat: public RegularSeat
{
public:
    RearRegularSeat(char area, int line, int chair, int basePrice);
    const string location() const override;
    ~RearRegularSeat() override = default;
};

// ---------------------------------------------
#endif
