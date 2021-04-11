//
// Created by pc1 on 22/06/2019.
//

#include "eurovision.h"
#define STATE_NOT_EXIST -1
#define PLACE_1 12
#define PLACE_2 10
#define PLACE_3 8

//"PARTICIPANT.CPP: START//

Participant::Participant(const string& state, const string& song,
                         const int& timeLength, const string& singer):
        stateName(state),    songName(song),
        timeLen(timeLength), singerName(singer),
        _registered(0)
{}

const string Participant::state() const {
    return this->stateName;
}

const string Participant::song() const {
    return this->songName;
}

int Participant::timeLength() const {
    return this->timeLen;
}

const string Participant::singer() const {
    return this->singerName;
}

bool Participant::isRegistered() const {
    return this->_registered;
}

void Participant::updateRegistered(bool registered){
    this->_registered = registered;
}

void Participant::update(const string& song, int timeLength, const string& singer){
    if (this->isRegistered()) return;
    if (!song.empty()){
        this->songName   = song;
    }
    if (timeLength != 0){
        this->timeLen    = timeLength;
    }
    if (!singer.empty()){
        this->singerName   = singer;
    }
}

ostream& operator<<(ostream& os, const Participant& p) {
    os << "[" + p.state() + "/" + p.song() + "/"
       << p.timeLength() << "/" << p.singer() << "]" ;
    return os;
}

//END//

//VOTER.CPP: START//

Voter::Voter(const string name): state_Name(name) {
    this->voter_type = Regular;
}

Voter::Voter(const string name, VoterType vote_Type): state_Name(name) {
    this->voter_type=vote_Type;
}

VoterType Voter::voterType()const{
    return this->voter_type;
}

int Voter::timesOfVotes() const{
    return this->number_Of_Votings;
}

const string Voter::state() const {
    return this->state_Name;
}

ostream& operator<<(ostream& os, const Voter& vr) {
    if(vr.voterType()==Regular){
        os << "<" + vr.state() + "/Regular" << ">" ;
        return os;
    }
    os << "<" + vr.state() + "/Judge"<< ">" ;
    return os;
}

void Voter::operator++() {
    this->number_Of_Votings++;
}

//END//

//VOTE.CPP: START//

Vote::Vote( Voter& vtr, const string& state1, const string& state2,
            const string& state3, const string& state4 ,
            const string& state5 , const string& state6,
            const string& state7, const string& state8 ,
            const string& state9, const string& state10):vtr(vtr)
        ,array_Of_Votes(new string[10]) {
    if(vtr.voterType() == Judge){//taking care of the judge's vote
        if(vtr.timesOfVotes() == 1) return;// judge cannot vote anymore
        this->array_Of_Votes[0]=state1, this->array_Of_Votes[1]=state2;
        this->array_Of_Votes[2]=state3, this->array_Of_Votes[3]=state4;
        this->array_Of_Votes[4]=state5, this->array_Of_Votes[5]=state6;
        this->array_Of_Votes[6]=state7, this->array_Of_Votes[7]=state8;
        this->array_Of_Votes[8]=state9, this->array_Of_Votes[9]=state10;
        for(int i = 0 ; i < 10 ; i++){
            if(this->array_Of_Votes[i] != vtr.state() && !this->array_Of_Votes[i].empty()) {//made a legal vote.
                continue;
            }else{
                this->array_Of_Votes[i] = ""; //did not made a legal vote therefor will not be counted
            }
        }
    }else{ // regular votes
        // we will take the first one only
        if(state1 != vtr.state() ){ //made a legal vote
            this->single_string = state1;

        }else{
            this->single_string="";
        }

    }
}

Vote::~Vote() {
    delete[] this->array_Of_Votes;
}

//END//

//MAINCONTROL.CPP: START//


MainControl::MainControl(int maxSongLength, int maxParticipants, int maxVotes):
        _phase(Registration), _maxParticipants(maxParticipants),
        _maxSongLength(maxSongLength), _maxVotes(maxVotes),
        _participants(new Participant * [maxParticipants + 1]),
        _regular_Votes(new int [maxParticipants]),
        _judge_Votes(new int [maxParticipants]),  _total_Votes(new int [maxParticipants])

{
    for(int i = 0; i < maxParticipants; i++){
        this->_regular_Votes[i] = 0;
        this->_judge_Votes  [i] = 0;
        this->_total_Votes  [i] = 0;
        this->_participants [i] = nullptr;
    }
    this->_participants[maxParticipants]=nullptr; // will be needed for the iterator
}

int Max(int a, int b) {
    if(a > b) return 1;
    if(a < b) return -1;
    return 0;
}

MainControl::~MainControl() {
    delete[] _participants;
    delete[] _regular_Votes;
    delete[] _judge_Votes;
    delete[] _total_Votes;
}

MainControl& MainControl::operator+=(Participant& p){
    if (this->_phase == Registration &&//Registration phase
        GetNumOfParticipants() < this->_maxParticipants && // there is still room for participants
        participate(p.state()) == 0 &&       // no two participants of the same state
        legalParticipant(p) &&                             //validates participant properties
        !p.isRegistered() ){                             // p is not already registered
        Register(p);
    }
    //else do nothing apparently
    return  *this;
}



MainControl& MainControl::operator-=(Participant& p){
    if (this->_phase == Registration &&         //Registration phase
        GetNumOfParticipants() > 0 &&           // there are registered participants
        participate(p.state()) && p.isRegistered()){ // the participant is registered
        DeRegister(p);
    }
    //else do nothing apparently
    return *this;
}

int MainControl::RankIndexToScore(int index) const{
    switch (index) {
        case 0:
            return PLACE_1;
        case 1:
            return PLACE_2;
        case 2:
            return PLACE_3;
        default:
            return PLACE_2 - index;
    }
}

MainControl& MainControl::operator+=(Vote v) {
    int index,score;
    if(this->_phase == Voting){
        if(!participate(v.vtr.state())) return *this; //the state of the voter is not a participant
        if(v.vtr.voterType() == Judge){
            if(v.vtr.timesOfVotes() == 1) return *this;// can not vote anymore;
            for(int i = 0 ; i < 10 ; i++){
                if(!v.array_Of_Votes[i].empty()){       //checking if there is a state name
                    index=StateToIndex(v.array_Of_Votes[i]);
                    if(index != -1){
                        score=RankIndexToScore(i);
                        this->_judge_Votes[index] += score; // updating the score from the judges
                        this->_total_Votes[index] += score;
                        if(v.vtr.timesOfVotes() == 0) { //made a legal vote.
                            ++v.vtr;
                        }
                    }
                }
            }
        }else{
            if(v.vtr.timesOfVotes() < _maxVotes){
                if(!v.single_string.empty()){
                    index=StateToIndex(v.single_string);
                    if(index == -1)  return *this;//vote is not legal
                    this->_regular_Votes[index] += 1; // updating the score from the regular voters
                    this->_total_Votes[index] += 1;
                    ++v.vtr;
                }
            }

        }
    }

    return *this;
}
int MainControl::GetNumOfParticipants() const{
    int len = 0 ;
    for (int i = 0; i < this->_maxParticipants; i++){
        if (this->_participants[i] == nullptr) return len;
        len++;
    }
    return len;
}

void MainControl::setPhase(Phase phase){
    if ((this->_phase == Registration && phase == Contest) ||
        (this->_phase == Contest && phase == Voting)){ // only two valid scenarios
        this->_phase = phase;
    }
}

void MainControl::Register(Participant& p){
    this->_participants[GetNumOfParticipants()] = &p;
    p.updateRegistered(true);
}

void MainControl::DeRegister(Participant& p){// assuming exists
    int participantSize =GetNumOfParticipants();
    for (int i = 0; i < participantSize; i++){
        if (_participants[i]->state() == p.state()){//assuming unique
            //replace with the last registered participant
            _participants[i] = _participants[participantSize - 1];
            //the size function will return an incorrect value:
            _participants[GetNumOfParticipants() - 1] = nullptr;
            break;//assuming unique
        }
    }
    p.updateRegistered(false);
}

int  MainControl::participate(const string& state) const {
    return (StateToIndex(state) == STATE_NOT_EXIST ? 0 : 1);
}

int MainControl:: StateToIndex(const string& state) const{
    for (int i = 0; i < this->GetNumOfParticipants(); i++){
        if (this->_participants[i]->state() == state
            && this->_participants[i]->isRegistered()){
            return i;
        }
    }
    return STATE_NOT_EXIST;
}

int MainControl::legalParticipant(const Participant& p) const{
    if (p.state().empty() || p.timeLength() > this->_maxSongLength ||
        p.song().empty() || p.singer().empty() )
        return 0;
    return 1;
}

ostream& PrintPrefix(ostream& os, const MainControl& mainControl){
    os << "{\n";
    if (mainControl._phase == Registration) {
        os << "Registration\n";
    }else if(mainControl._phase == Contest){
        os << "Contest\n";
    }else {//Voting phase
        os << "Voting\n";
    }
    return os;
}


ostream& SortedPrint(ostream& os, const MainControl& mainControl,
                     Participant ** partsCopy, int partSize){
    for (int j = 0; j < partSize; j++){
        string minState = "" ;
        int minIndex = 0;
        for (int i = 0; i < partSize; i++) {//checking for remaining participants
            if(partsCopy[i] == nullptr) continue;
            minState = partsCopy[i]->state();
            minIndex = i;
            break;
        }
        if (minState.empty()){// no more participants found
            break;
        }
        for (int i = 0; i < partSize; i++) {// searching for current min state
            if(partsCopy[i] == nullptr) continue;
            if (partsCopy[i]->state() < minState ) {// found min
                minState = partsCopy[i]->state();
                minIndex = i;
            }
        }
        if (mainControl._phase == Registration || mainControl._phase == Contest){
            os << *partsCopy[minIndex];
            os<<"\n";
        }
        else {//Voting phase
            os << mainControl._participants[minIndex]->state() +
                  " : Regular(" << mainControl._regular_Votes[minIndex] << ")"<<
               + " Judge("   << mainControl._judge_Votes[minIndex] << ")\n";
        }
        partsCopy[minIndex] = nullptr;//"deleting"
    }
    return os;
}


ostream& operator<<(ostream& os, const MainControl& mainControl) {
    PrintPrefix(os, mainControl);
    if(mainControl._phase != Contest){
        int partSize = mainControl.GetNumOfParticipants();
        Participant ** partsCopy = new Participant * [partSize];
        for (int i = 0; i < partSize; i++){//coping original participant array
            partsCopy[i] = mainControl._participants[i];
        }
        SortedPrint(os, mainControl, partsCopy, partSize);
        delete[] partsCopy;
    }
    os << "}\n";
    return os;
}

template <class T, class Compare>  T* get(T* start, T* end, int i, Compare& compare){
    if (i == 0 || start == end)//troll
        return end;
    T * temp = start;
    for (int j = 0; j < i; j++ ){//invalid container length
        if (temp == end) return end;
    }
    T * min   = start, * currMax = start, ** arr = new T* [i];
    temp   = start + 1;
    while (temp != end){// found the first maximum
        if(compare(*temp , *currMax) == 1){
            currMax = temp;
        }
        if(compare(*temp , *min) == -1){
            min = temp;
        }
        temp++;

    }
    arr[0] = currMax;
    if(i == 1){//container has length of 1
        delete[] arr;
        return currMax;
    }
    for(int j = 1 ; j < i ; j++){//each iteration finds the next max element
        temp = start;
        T * prevMax = arr[j - 1];
        currMax = min;
        while (temp != end){

            if(compare(*temp , *currMax) == 1 && compare(*temp ,*prevMax) == -1 ){
                //found a new maximum value
                currMax = temp;

            }
            temp++;
        }

        if(compare(*currMax ,*prevMax) == 0){// no new maximum found
            break;
        } else{// new max found
            arr[j] = currMax;

        }
    }
    //temp  = arr[i-1];
    delete[] arr;
    return currMax;
}

int ParticipantCompare( Participant* p1 , Participant* p2 ) {
    if(p2 == nullptr || p1->state() < p2->state()) return -1;
    if(p1->state() > p2->state()) return 1;
    return 0;
}
int ParticipantCompareT( Participant* p1 , Participant* p2 ) {
    //the greater participant has the lower state lexicographically
    if(p1 == nullptr ) return 0;
    if(p2 == nullptr || p1->state() < p2->state()) return 1;
    if(p1->state() > p2->state()) return -1;
    return 0;
}

MainControl::Iterator_Type MainControl::begin() {
    return  _participants;
}

MainControl::Iterator_Type  MainControl::end(){
    int numOfParticipants = GetNumOfParticipants();
    return _participants + numOfParticipants;
}

const Participant& MainControl::Iterator::operator*() const
{
    return (const Participant&)*(this->_value);
}

MainControl::Iterator& MainControl::Iterator::operator=(Iterator_Type p1){
    this->_data    = p1;
    this->_data_end = GetEnd(_data);
    this->_value   = *get(_data, _data_end, 1 , ParticipantCompareT);
    this->_position = 1;
    return *this;
}

bool MainControl::Iterator::operator<(MainControl::Iterator_Type it2) const{
    if (this->_value == *it2)
        return false;
    return ParticipantCompare(this->_value, *it2) == -1;
}

bool MainControl::Iterator::operator==(MainControl::Iterator_Type it2) const {
    if (this->_value == *it2)
        return true;
    return (ParticipantCompare(this->_value, *it2) == 0);
}

void MainControl::Iterator::operator++() {
    if (_data + _position == _data_end ) {//reached end of container
        this->_value = nullptr;
    }else{
        this->_value = *get(_data, _data_end,  _position + 1 , ParticipantCompareT);
        _position++;
    }
}

Participant ** GetEnd(Participant** data){
    Participant** p = data;
    while (*p != nullptr){
        p++;
    }
    return  p;
}

string MainControl::scoreDuplication(VoterType type){
    int numOfParticipants = GetNumOfParticipants();
    Participant** pCopy = new Participant *[numOfParticipants];
    int k = 0;
    for(int j = 0 ; j < numOfParticipants; j++){
        if( this->_participants[j] != nullptr){
            // type is static in every function call:
            if((type == Regular && this->_regular_Votes[j] == this->scoreOfDuplication)
               || (type == Judge && this->_judge_Votes[j] == this->scoreOfDuplication)
               || (type == All &&  this->_judge_Votes[j] + this->_regular_Votes[j] == this->scoreOfDuplication)){
                //found a state with the required score
                pCopy[k] = this->_participants[j];
                k++;

            }

        }

    }
    Participant participant = **get(pCopy, pCopy + k , this->countOfDuplication, ParticipantCompareT);
    string st = participant.state();
    this->countOfDuplication--;
    delete[] pCopy;
    return st;


}

string MainControl::operator()(int i , VoterType type){
    if(i == 1){
        countOfDuplication = 0;
    }
    int numOfParticipants = GetNumOfParticipants();
    if(i > numOfParticipants){// there are no more participants
        return "";
    }
    string ranked_state;
    if(this->countOfDuplication > 0){
        return scoreDuplication(type);
    }
    int * origVotes;

    switch(type){
        case Regular:
            origVotes = this->_regular_Votes;
            break;
        case Judge:
            origVotes = this->_judge_Votes;
            break;
        case All:
            origVotes = this->_total_Votes;
            break;

    }
    ranked_state = GetRankedVote(i, type, origVotes);
    return ranked_state;

}

string MainControl::GetRankedVote(int i, VoterType , int * origVotes){
    int numOfParticipants = GetNumOfParticipants();
    string rankedState;
    int* votesCopy = new int [numOfParticipants];
    if(i == 1){
        this->amountOfDuplication = 0; // first place, no duplication
    }
    int k = 0, l = 0, copies = 0;
    while(l < numOfParticipants){//copying only non null values
        if(this->_participants[k] != nullptr){
            votesCopy[l] = origVotes[k];
            l++;
        }
        k++;
    }
    // Multiple states with same score handling:
    int num = *get(votesCopy, votesCopy+ numOfParticipants, i - this->amountOfDuplication, Max);
    Participant** pCopy = new Participant *[numOfParticipants];
    for(int j = 0; j < this->_maxParticipants; j++){
        if(this->_participants[j] != nullptr){//copying only non null values
            if(origVotes[j] == num){//found a state with the required score
                pCopy[copies] = this->_participants[j];
                copies++;// counting duplicates
            }
        }

    }
    if(copies > 1) {// there are more than one state with the ranked score
        this->countOfDuplication = copies -1;
        this->scoreOfDuplication = num;
        this->amountOfDuplication += copies -1;
        Participant part = **get(pCopy, pCopy + copies, copies, ParticipantCompareT);
        rankedState  = part.state();
    }else{// no duplication
        rankedState = pCopy[0]->state();
    }
    delete[] pCopy;
    delete[] votesCopy;
    return  rankedState;
}

//END//