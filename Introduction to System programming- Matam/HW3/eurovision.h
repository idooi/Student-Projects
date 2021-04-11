#ifndef EUROVISION_H_
#define EUROVISION_H_
#include <iostream>
// it's allowed to define here any using statements, according to needs.
// do NOT define here : using namespace std;
#define MAX_SONG_LEN     180
#define MAX_PARTICIPANTS 26
#define MAX_VOTES 5

using std::string;
using std::ostream;
//---------------------------------------------------

enum VoterType { All, Regular, Judge };
enum Phase { Registration, Contest, Voting };

//---------------------------------------------------

class Participant
{
private:
    const string stateName ;
    string       songName  ;
    int          timeLen   ;
    string       singerName;
    bool         _registered ;
// relevant private members can be defined here, if necessary.


public :

// need to define here possibly c'tr and d'tr and ONLY methods that
// are mentioned and demonstrated in the test example that has been published.
// NO OTHER METHODS SHOULD APPEAR HERE.

// NO friend is allowed here.

    Participant(const string& state, const string& song,
                const int& timeLength, const string& singer);

    ~Participant() = default;

    const  string state() const;

    int    timeLength()   const;

    const string song()         const;

    const string singer()       const;

    bool   isRegistered() const;

    void   updateRegistered(bool registered);

    void   update(const string& song, int timeLength, const string& singer);

};

ostream& operator<<(ostream& os, const Participant& p) ;

//---------------------------------------------------


class Voter
{
private:
    const string state_Name;
    VoterType voter_type;
    int number_Of_Votings = 0;

    // relevant private members can be defined here, if necessary.

public :
    Voter(const string name, VoterType vote_Type);
    Voter(const string name);
    ~Voter() = default;
    VoterType    voterType()       const;
    int          timesOfVotes()    const;
    const string state()           const;
    void operator++();

// need to define here possibly c'tr and d'tr and ONLY methods that
// are mentioned and demonstrated in the test example that has been published.
// NO OTHER METHODS SHOULD APPEAR HERE.

// NO friend is allowed here.
};

ostream& operator<<(ostream& os, const Voter& vr) ;

// -----------------------------------------------------------

struct Vote
{
public:
    Voter& vtr;
    string* array_Of_Votes;
    string single_string;
    Vote( Voter& vtr, const string& state1, const string& state2 = "",
          const string& state3 = "", const string& state4 = "",
          const string& state5 = "", const string& state6 = "",
          const string& state7 = "", const string& state8 = "",
          const string& state9 = "", const string& state10 = "");
    ~Vote();
// ALL is public here.
// need to define ONLY data members and c'tr and d'tr.
// NO NEED to define anything else.

};

// -----------------------------------------------------------


class MainControl
{
//relevant private members can be defined here, if necessary.
private:

    Phase _phase;
    int   _maxParticipants;
    int   _maxSongLength;
    int   _maxVotes;
    Participant ** _participants;
    int* _regular_Votes;// score gained from regular voters
    int* _judge_Votes; //score gained from judges
    int* _total_Votes; //score gained from all voters
    int countOfDuplication = 0; // how many times a certain score is appearing
    int scoreOfDuplication = 0; // the score with multiple appearances
    int amountOfDuplication = 0; // the amount of numbers that have multiple appearances

    void Register(Participant& p);

    void DeRegister(Participant& p);// deregister a participant

    //finds the participated state in the _participants array
    int  StateToIndex(const string& state) const ;

    int  GetNumOfParticipants() const;

    //converts the state index in the judge's votes array to its score
    int RankIndexToScore(int index) const ;

    string GetRankedVote(int i, VoterType , int * origVotes);//Calculates the i'nd voted state

    friend ostream& operator<<(ostream& os, const MainControl& m) ;

    friend ostream& PrintPrefix(ostream& os, const MainControl& mainControl);

    // outputs the participants to the stream according to their's state names
    friend ostream& SortedPrint(ostream& os, const MainControl& mainControl,
                                Participant ** partsCopy, int partSize);


    string scoreDuplication(VoterType type);

    typedef  Participant **   Iterator_Type  ;


public :
    class  Iterator
    {
    private:
        Iterator_Type _data;
        Iterator_Type _data_end;
        Participant* _value;
        int _position;
    public:

        ~Iterator() = default;
        Iterator() = default;
        const Participant& operator*() const;
        Iterator& operator=(Iterator_Type p1);
        bool operator==(MainControl::Iterator_Type it2) const;
        bool operator <(MainControl::Iterator_Type it2) const;
        void operator++();
    }  ;


    explicit MainControl(int maxSongLength = MAX_SONG_LEN,
                         int maxParticipants = MAX_PARTICIPANTS, int maxVotes = MAX_VOTES);

    ~MainControl();

    Iterator_Type begin();

    Iterator_Type  end();

    void setPhase(Phase phase);

    int  legalParticipant(const Participant& p) const;

    int  participate(const string& state) const;

    MainControl& operator+=(Participant& p);

    MainControl& operator-=(Participant& p);

    MainControl& operator+=(Vote v);

    string operator()(int i , VoterType type);




// need to define here possibly c'tr and d'tr and ONLY methods that
// are mentioned and demonstrated in the test example that has been published.
// NO OTHER METHODS SHOULD APPEAR HERE.

// Also it's allowed here to define friend.

};

int Max(int a , int b);

template <class T, class Compare>
T* get(T* start, T* end, int i, Compare& compare);

//returns the participant with the greater state name
int ParticipantCompare(Participant* p1 ,Participant* p2 );

//returns the participant with the lower state name
int ParticipantCompareT( Participant* p1 , Participant* p2 );

//returns the address of the element directly ofter the last participant in the array
Participant ** GetEnd(Participant** data);

// -----------------------------------------------------------

#endif