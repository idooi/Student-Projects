//
// Created by IdoSarel on 04/25/19.
//
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "eurovision.h"
#include "euro_utility.h"
#include "map.h"
#include "state.h"
#include "judge.h"
#include "string.h"
#define STATE 0
#define JUDGE 1
#define INI_STATE_SCORE -1
#define DUMMY_STATE_ID  -1
//****************************************************
#define CHECK_MEMORY(b,ret)                                \
  if((b) == EUROVISION_OUT_OF_MEMORY)  {                   \
      OUT_OF_MEMORY_ERROR(eurovision);                     \
      return ret ;                                         \
    }                                                      \

#define CHECKING(addResult)                                \
  if(addResult != EUROVISION_SUCCESS){                     \
    CHECK_MEMORY(addResult, EUROVISION_OUT_OF_MEMORY);     \
    return addResult;                                      \
}
//===================== Our FUNCCSS ==================

//Declaration (#stillsucks)

//checking if the state already exists
bool StateExists(Eurovision eurovision, int stateId);

//checking if the judge already exists
bool JudgeExists(Eurovision eurovision, int judgeId);

//changing the result from a Map function to a eurovision result
EurovisionResult MapToEuroResult(MapResult result, int obj);

//copying the map of the judges
MapDataElement JudgeCopyMap(MapDataElement judge);

//destroying the map of the judges
void JudgeDestroyMap(MapDataElement judge);

//initializing the maps of the results
EurovisionResult InitializeResultMaps(Eurovision eurovision);

//calculates the results from the judges
EurovisionResult CalculateJudgeResults(Eurovision eurovision);

//calculates the final results
EurovisionResult CalculateFinalResults(Eurovision eurovision, int audiencePercent);

List GetFinalResults(Eurovision eurovision);

//comparing the scores of two states and sorting the list of winning states
int CompareStateScore(ListElement state1, ListElement state2);

//remove from each state the results that were given to a state we removed
EurovisionResult RemoveResultState(Map states, int stateId);

//checking if a vote is legal
EurovisionResult IsVoteLegal(Eurovision eurovision, int stateGiver,
                             int stateTaker);

//making a string of two friendly states names
char * GetFriendsString(State s1, State s2);

//copying function to a list
ListElement stringCopy(ListElement str );

//checking if two states are already friendly
bool AlreadyFriends( List friends, char * friendsString);

//calculating the results from each state
EurovisionResult CalculateStateResults(Eurovision eurovision);

//checking if two states are friendly
bool IsFriendly(State currState, State state);

//inserting the top 10 votes into the array
int * GetTopVotes(Map stateVotes, int * topVotes, int numOfTop);

// initializing the array
void InitialArray(int * arr, int size);

//rounding the score
double RoundScore(double score);

//in case the memory allocation failed, destroying the eurovision
EurovisionResult OUT_OF_MEMORY_ERROR(Eurovision eurovision);

//removing a judge that voted to a state we want to delete
void RemoveJudgesOfState(Eurovision eurovision, int stateId);
//====================================================

struct eurovision_t {
    Map states;
    Map judges;
    Map stateResults;
    Map judgeResults;
};

Eurovision eurovisionCreate() {
    Eurovision eurovision = malloc(sizeof(*eurovision));
    Map states =       mapCreate((copyMapDataElements)StateCopy, CopyInt,
                                 (freeMapDataElements)StateDestroy,FreeIntOrDouble, CompareInts);
    Map judges =       mapCreate(JudgeCopyMap, CopyInt, JudgeDestroyMap, FreeIntOrDouble, CompareInts);
    Map stateResults = mapCreate(CopyInt,      CopyInt, FreeIntOrDouble, FreeIntOrDouble, CompareInts);
    Map judgeResults = mapCreate(CopyInt,      CopyInt, FreeIntOrDouble, FreeIntOrDouble, CompareInts);
    if (!eurovision || !judges || !states ||
        !stateResults || !judgeResults) {
        mapDestroy(states);
        mapDestroy(judges);
        mapDestroy(stateResults);
        mapDestroy(judgeResults);
        free(eurovision);
        return NULL;
    }
    eurovision->states = states;
    eurovision->judges = judges;
    eurovision->stateResults = stateResults;
    eurovision->judgeResults = judgeResults;
    return eurovision;
}

void eurovisionDestroy(Eurovision eurovision) {
    if (!eurovision) return;
    mapDestroy(eurovision->states);
    mapDestroy(eurovision->judges);
    mapDestroy(eurovision->stateResults);
    mapDestroy(eurovision->judgeResults);
    free(eurovision);
}

EurovisionResult eurovisionAddState(Eurovision eurovision, int stateId,
                                    const char *stateName,
                                    const char *songName) {
    if (!eurovision || !stateName || !songName) return EUROVISION_NULL_ARGUMENT;
    if (!IdIsValid(stateId))              return EUROVISION_INVALID_ID;
    if (!NameIsValid(stateName) || !NameIsValid(songName)) {
        return EUROVISION_INVALID_NAME;
    }
    if (StateExists(eurovision, stateId)) return EUROVISION_STATE_ALREADY_EXIST;
    //all good
    //copy elements
    const char * stateNameCopy = CopyName(stateName);
    const char * songNameCopy = CopyName(songName);
    State newState = CreateState(stateId, stateNameCopy, songNameCopy);
    if (!stateNameCopy || !songNameCopy|| !newState){
        free((char*)stateNameCopy);
        free((char*)songNameCopy);
        StateDestroy(newState);
        return OUT_OF_MEMORY_ERROR(eurovision);
    }
    EurovisionResult addResult = MapToEuroResult(mapPut(eurovision->states, &stateId, newState), STATE);
    StateDestroy(newState);//mapPut copies
    CHECKING(addResult)
    return addResult;//will have the right eurovision result
}

EurovisionResult eurovisionRemoveState(Eurovision eurovision, int stateId) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    if (!IdIsValid(stateId)) return EUROVISION_INVALID_ID;
    if (!StateExists(eurovision, stateId)) return EUROVISION_STATE_NOT_EXIST;
    RemoveResultState(eurovision->states, stateId);
    RemoveJudgesOfState(eurovision, stateId);//Removes the judges that voted for that state
    MapResult result = mapRemove(eurovision->states, &stateId);
    if (result == MAP_NULL_ARGUMENT || result == MAP_ITEM_DOES_NOT_EXIST){
        return EUROVISION_STATE_NOT_EXIST;
    }
    //else the right node has been deleted
    RemoveResultState(eurovision->states, stateId);
    return MapToEuroResult(result, STATE);
}

EurovisionResult eurovisionAddJudge(Eurovision eurovision, int judgeId,
                                    const char *judgeName,
                                    int *judgeResults) {
    if (!eurovision || !judgeName || !judgeResults ) return  EUROVISION_NULL_ARGUMENT;
    if (!IdIsValid(judgeId))              return EUROVISION_INVALID_ID;
    for (int i = 0; i < JUDGE_RANK_LEN; i++) {
        if (!IdIsValid              (judgeResults[i]))  return EUROVISION_INVALID_ID;
        for(int j = 0;j <JUDGE_RANK_LEN; j++){//checks for two votes for the same state
            if (judgeResults[j] == judgeResults[i] && j != i){
                return EUROVISION_INVALID_ID;
            }

        }
    }//separated because of error hierarchy
    for (int i = 0; i < JUDGE_RANK_LEN; i++) {
        if (!StateExists(eurovision, judgeResults[i])) return EUROVISION_STATE_NOT_EXIST;
    }
    if (!NameIsValid(judgeName))          return EUROVISION_INVALID_NAME;
    if (JudgeExists(eurovision, judgeId)) return EUROVISION_JUDGE_ALREADY_EXIST;
    //copy elements:
    int  * judgeResultsCopy = CopyArray(judgeResults, JUDGE_RANK_LEN);
    const char * judgeNameCopy = CopyName(judgeName);
    Judge newJudge = JudgeCreate(judgeId, judgeNameCopy, judgeResultsCopy);
    if (!judgeResultsCopy || !judgeNameCopy || !newJudge){
        free(judgeResultsCopy);
        free((char*)judgeNameCopy);
        JudgeDestroy(newJudge);
        return OUT_OF_MEMORY_ERROR(eurovision);
    }

    EurovisionResult addResult =MapToEuroResult( mapPut(eurovision->judges, &judgeId, newJudge), JUDGE);
    JudgeDestroy(newJudge);// mapPut copies
    CHECKING(addResult)
    return EUROVISION_SUCCESS;
}

EurovisionResult eurovisionRemoveJudge(Eurovision eurovision, int judgeId) {
    if (!eurovision)                       return EUROVISION_NULL_ARGUMENT;
    if (!IdIsValid(judgeId))               return EUROVISION_INVALID_ID;
    if (!JudgeExists(eurovision, judgeId)) return EUROVISION_JUDGE_NOT_EXIST;
    MapResult remResult = mapRemove(eurovision->judges, &judgeId);
    return MapToEuroResult(remResult, JUDGE);//Because I want to
}

EurovisionResult eurovisionAddVote(Eurovision eurovision, int stateGiver,
                                   int stateTaker) {
    if(!eurovision) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult addResult;
    EurovisionResult result = IsVoteLegal(eurovision, stateGiver,
                                          stateTaker);
    CHECKING(result)
    int vote=1;
    State state = (State)mapGet(eurovision->states, &stateGiver);
    if(!mapContains(state->votes,&stateTaker)){
         addResult  = MapToEuroResult( mapPut(state->votes,&stateTaker,&vote), STATE);
         CHECKING(addResult)
         return addResult;
        }
    vote = *(int*)mapGet(state->votes, &stateTaker);
    vote += 1;
    addResult  = MapToEuroResult( mapPut(state->votes,&stateTaker,&vote), STATE);
    CHECKING(addResult)
    return EUROVISION_SUCCESS;

}
//CHANGED
EurovisionResult eurovisionRemoveVote(Eurovision eurovision, int stateGiver,
                                      int stateTaker) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    EurovisionResult addResult;
    EurovisionResult result = IsVoteLegal(eurovision, stateGiver,
                                          stateTaker);
    if (result != EUROVISION_SUCCESS) return result;
    int vote=0;
    State state = (State)mapGet(eurovision->states, &stateGiver);
    if(!mapContains(state->votes,&stateTaker)){//No votes to that state yet, so nothing
        vote = 0;
        addResult  = MapToEuroResult( mapPut(state->votes,&stateTaker,&vote), STATE);
        CHECKING(addResult)
        return addResult;
        return EUROVISION_SUCCESS;

    }
    vote = *(int*)mapGet(state->votes, &stateTaker);
    if (vote != 0) {// if 0 nothing to take from
        vote -= 1;
        addResult = MapToEuroResult(mapPut(state->votes, &stateTaker, &vote), STATE);
        CHECKING(addResult)
    }
    return EUROVISION_SUCCESS;
}

List eurovisionRunContest(Eurovision eurovision, int audiencePercent) {
    if (!eurovision) return NULL;
    if (audiencePercent > 100 || audiencePercent < 1) return NULL;
    EurovisionResult initialResult = InitializeResultMaps(eurovision);
    if (initialResult != EUROVISION_SUCCESS){
        OUT_OF_MEMORY_ERROR(eurovision);
        return NULL;
    }
    EurovisionResult calculateJudge = 0;
    if (audiencePercent == 100){// Audience's Favorite - no need for judge calculation
        calculateJudge = EUROVISION_SUCCESS;}
    else {
        calculateJudge = CalculateJudgeResults(eurovision);
    }
    if (calculateJudge                    != EUROVISION_SUCCESS ||
        CalculateStateResults(eurovision) != EUROVISION_SUCCESS){
        OUT_OF_MEMORY_ERROR(eurovision);
        return NULL;
    }
    CalculateFinalResults(eurovision, audiencePercent);
    List finalResults = GetFinalResults(eurovision);
    return finalResults;
}

List eurovisionRunAudienceFavorite(Eurovision eurovision){
    return eurovisionRunContest(eurovision, 100);
}

List eurovisionRunGetFriendlyStates(Eurovision eurovision){
    if (!eurovision) return NULL;
    List friendlyStates = listCreate(stringCopy, (FreeListElement)free);
    if (!friendlyStates){
        OUT_OF_MEMORY_ERROR(eurovision);
        return NULL;
    }
    char * friendsString;
    MAP_FOREACH(int *,iter, eurovision->states){
        State currState = (State)mapGet(eurovision->states, iter);

        MAP_FOREACH(int *,iter2, eurovision->states){
            State state = (State)mapGet(eurovision->states, iter2);
            if (currState->id == state->id) continue;//a state never votes for itself
            if (IsFriendly(currState, state)){
                friendsString = GetFriendsString(currState, state);
                if (!AlreadyFriends(friendlyStates, friendsString)){//prevents duplications
                    listInsertLast(friendlyStates,friendsString);
                }
                free(friendsString);
            }
        }
        //set iter back to currState
        iter = mapGetFirst(eurovision->states);
        while (*iter != *(int*)currState){
            iter = mapGetNext(eurovision->states);
        }
    }
    //now sort alphabetically and you better sort it well
    ListResult sortResult = listSort(friendlyStates, (CompareListElements)strcmp);
    if (sortResult != LIST_SUCCESS){
        OUT_OF_MEMORY_ERROR(eurovision);
        listDestroy(friendlyStates);
        return NULL;
    }
    return  friendlyStates;
}

//===================== Our FUNCCSS ==================

//Implementation

bool StateExists(Eurovision eurovision, int stateId) {
    if (!eurovision) return false;
    return mapContains(eurovision->states, &stateId);
}

bool JudgeExists(Eurovision eurovision, int judgeId) {
    if (!eurovision) return false;
    return mapContains(eurovision->judges, &judgeId);
}

EurovisionResult MapToEuroResult(MapResult result, int obj) {
    switch (result) {
        case MAP_SUCCESS:
            return EUROVISION_SUCCESS;
        case MAP_OUT_OF_MEMORY:
            return EUROVISION_OUT_OF_MEMORY;
        case MAP_NULL_ARGUMENT:
            return EUROVISION_NULL_ARGUMENT;
        case MAP_ITEM_ALREADY_EXISTS:
            return (obj == STATE ?
                    EUROVISION_JUDGE_ALREADY_EXIST :
                    EUROVISION_STATE_ALREADY_EXIST);
        case MAP_ITEM_DOES_NOT_EXIST:
            return (obj == STATE ?
                    EUROVISION_JUDGE_NOT_EXIST :
                    EUROVISION_STATE_NOT_EXIST);
        default:
            return EUROVISION_OUT_OF_MEMORY;
    }
}

MapDataElement JudgeCopyMap(MapDataElement judge) {
    if (!judge) return NULL;
    Judge judgeCopy = JudgeCopy((Judge)judge);
    return (MapDataElement)judgeCopy;
}

void JudgeDestroyMap(MapDataElement judge) {
    if (!judge) return;
    JudgeDestroy((Judge)judge);
}

EurovisionResult RemoveResultState(Map states, int stateId) {
    MAP_FOREACH(int *, iter, states) {
        int currStateId = *iter;
        State currState = (State)mapGet(states, &currStateId);
        if (mapContains(currState->votes, &stateId)){
            mapRemove(currState->votes, &stateId);
        }
    }
    return MapToEuroResult(MAP_SUCCESS, STATE);
}

EurovisionResult IsVoteLegal(Eurovision eurovision, int stateGiver,
                             int stateTaker) {
    if (!IdIsValid(stateGiver) || !IdIsValid(stateTaker)) return EUROVISION_INVALID_ID;
    if (!StateExists(eurovision, stateGiver) || !StateExists(eurovision, stateTaker))
        return EUROVISION_STATE_NOT_EXIST;
    if (stateGiver == stateTaker) return EUROVISION_SAME_STATE;
    return EUROVISION_SUCCESS;
}

EurovisionResult InitializeResultMaps(Eurovision eurovision) {//assuming exists
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    //in case of rerun
    mapClear(eurovision->stateResults);
    mapClear(eurovision->judgeResults);
    int initialStateScore = INI_STATE_SCORE;
    MAP_FOREACH(int *, iter, eurovision->states) {
        int stateId = *iter;
        MapResult statePutResult = mapPut(eurovision->stateResults, &stateId, &initialStateScore);
        MapResult judgePutResult = mapPut(eurovision->judgeResults, &stateId, &initialStateScore);
        if (statePutResult != MAP_SUCCESS || judgePutResult != MAP_SUCCESS)
        {
            mapClear(eurovision->stateResults);
            mapClear(eurovision->judgeResults);
            CHECK_MEMORY(MapToEuroResult(statePutResult,STATE),EUROVISION_OUT_OF_MEMORY)
            CHECK_MEMORY(MapToEuroResult(judgePutResult,JUDGE),EUROVISION_OUT_OF_MEMORY)
            bool error = (statePutResult != MAP_SUCCESS ? statePutResult : judgePutResult);
            return MapToEuroResult(error, (error == statePutResult ? STATE : JUDGE));
        }
    }
    return EUROVISION_SUCCESS;
}

EurovisionResult CalculateStateResults(Eurovision eurovision) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    int * topVotes = malloc(sizeof(int)*JUDGE_RANK_LEN);
    if (!topVotes){
        free(topVotes);
        return OUT_OF_MEMORY_ERROR(eurovision);
    }
    int stateGiverId = 0, stateTakerId = 0, oldScore = 0, newScore = 0;
    Map stateResults = eurovision->stateResults;
    MAP_FOREACH(int *, iter, eurovision->states) {
        stateGiverId = *iter;
        State currState =(State) mapGet(eurovision->states, &stateGiverId);
        if(!mapGetFirst(currState->votes)){//if currState didnt vote at all there is no point checking
            continue;
        }//if currState didnt vote at all there is no point checking
        int * stateTopVotes = GetTopVotes(currState->votes, topVotes, JUDGE_RANK_LEN);//return an array of the Top 10 stateIds
        for (int i = 0; i < JUDGE_RANK_LEN; i++){
            stateTakerId = stateTopVotes[i];//one of the top voted states of stateGiver
            if (stateTakerId == DUMMY_STATE_ID){//no more votes
                break; //Go to next giver's votes
            }
            oldScore = *(int*)mapGet(stateResults, &stateTakerId);//updating score
            if(oldScore < 0 ){//initial score is -1
                oldScore = 0;
            }
            newScore = RankIndexToScore(i) + oldScore;
            EurovisionResult result = MapToEuroResult(mapPut(stateResults, &stateTakerId, &newScore), STATE);
            if (result != EUROVISION_SUCCESS){
                mapClear(stateResults);
                free(topVotes);
                CHECK_MEMORY(result,EUROVISION_OUT_OF_MEMORY)
                return result;
            }
        }
    }
    free(topVotes);
    return EUROVISION_SUCCESS;
}

EurovisionResult CalculateJudgeResults(Eurovision eurovision) {
    if (!eurovision) return EUROVISION_NULL_ARGUMENT;
    Map judgeResults = eurovision->judgeResults;
    int newScore = 0, oldScore = 0, stateId = 0;
    //assuming the results map's key values are stateIds
    //for each judge, take his scores and add it to the right state
    MAP_FOREACH(int *, iter, eurovision->judges) {
        Judge judge = (Judge)mapGet(eurovision->judges, iter);// *iter is judgeId
        for (int j = 0; j < JUDGE_RANK_LEN; j++) {
            stateId = judge->ranking[j];
            oldScore = *(int*)mapGet(judgeResults, &stateId);//updating score
            if(oldScore < 0){//initial score is -1
                oldScore = 0;
            }
            newScore = RankIndexToScore(j) + oldScore;
            EurovisionResult result = MapToEuroResult(mapPut(judgeResults, &stateId, &newScore), STATE);
            CHECKING(result)
        }
    }
    return EUROVISION_SUCCESS;
}

EurovisionResult CalculateFinalResults(Eurovision eurovision, int audiencePercent) {//updates finalScore of each state
    int numJudges = mapGetSize(eurovision->judges);
    int numStates = mapGetSize(eurovision->states);
    if (numStates == 0) return EUROVISION_SUCCESS;//Nothing left to calculate
    int stateId = 0;
    double statesScore = 0, judgesScore = 0, finalScore = 0;
    MAP_FOREACH(int*, iter, eurovision->states) { //updating finalResults
        judgesScore = 0;
        finalScore  = 0;
        stateId = *iter;
        statesScore = ((double) (*(int *) mapGet(eurovision->stateResults, &stateId)));
        statesScore = (statesScore <= 0.0 ? 0 : statesScore) / numStates;//Nobody voted for it
        if (numJudges != 0) {
            judgesScore = ((double) (*(int *) mapGet(eurovision->judgeResults, &stateId)));
            judgesScore = (judgesScore <= 0.0 ? 0 : judgesScore) / numJudges;
        }
        finalScore = (audiencePercent * statesScore + (100 - audiencePercent) * judgesScore) / 100;
        //finalScore = RoundScore(finalScore);
        State state = mapGet(eurovision->states, &stateId);
        state->finalScore = finalScore;//updating state->finalScore
    }
    //now finalResults holds the final result of each state
    return EUROVISION_SUCCESS;
}

List GetFinalResults(Eurovision eurovision){
    List results = listCreate((CopyListElement)StateCopy, (FreeListElement)StateDestroy);
    if (!results) return NULL;
    int stateId = 0;
    MAP_FOREACH(int*, iter, eurovision->states){ //adding all states to the list and then sorting the list by the score
        stateId = *iter;
        State state = (State)mapGet(eurovision->states, &stateId);
        ListResult insertResult = listInsertFirst(results, state);
        if (insertResult != LIST_SUCCESS) {
            OUT_OF_MEMORY_ERROR(eurovision);
            return NULL;
        }
    }
    ListResult sortResult = listSort(results, CompareStateScore);//sort
    if (sortResult != LIST_SUCCESS){
        listDestroy(results);
        OUT_OF_MEMORY_ERROR(eurovision);
        return NULL;
    }
    List resultsNames = listCreate(stringCopy, (FreeListElement)free);//create stateName list
    if (!resultsNames){
        listDestroy(results);
        OUT_OF_MEMORY_ERROR(eurovision);
        return NULL;
    }
    LIST_FOREACH(State, iter, results){//populate resultNames
        State state = (State)iter;
        char * stateName = (char*)state->stateName;
        //printf("%s, %.2f\n",state->stateName, state->finalScore);
        ListResult insertResult = listInsertFirst(resultsNames, stateName);//inverted order
        if (insertResult != LIST_SUCCESS){
            listDestroy(results);
            listDestroy(resultsNames);
            OUT_OF_MEMORY_ERROR(eurovision);
            return NULL;
        }
    }
    listDestroy(results);
    return  resultsNames;
}

int CompareStateScore(ListElement state1, ListElement state2) {// for the sort of state list
    State s1 = (State)state1;
    State s2 = (State)state2;
    if (s1->finalScore == s2->finalScore){
        return (s1->id < s2->id ? 1 : -1); // never equal
    }
    return (s1->finalScore - s2->finalScore > 0.0 ? 1 : -1) ;
}

int * GetTopVotes(Map stateVotes, int * topVotes, int numOfTop){//return an array of the top numOfTop stateIds
    if (!stateVotes) return NULL;
    Map votesCopy = mapCopy(stateVotes);
    if (!votesCopy){
        //allocation failed -> OUT_OF_MEMORY_ERROR
        mapDestroy(votesCopy);
        return  NULL;
    }
    InitialArray(topVotes, numOfTop);// sets all elements to DUMMY_STATE_ID
    if (!mapGetFirst(stateVotes)){//No votes
        mapDestroy(votesCopy);
        return topVotes;
    }
    int currMax = 0, currTopStateId = DUMMY_STATE_ID, stateId = 0 , stateVote = 0;
    for (int i = 0; i < numOfTop; i++){//find top stateId numOfTop times
        currMax =  INI_STATE_SCORE;
        currTopStateId =  DUMMY_STATE_ID;
        MAP_FOREACH(int * , iter2, votesCopy){//searching for our new star state
            stateId   = *iter2;
            stateVote = *(int*)mapGet(votesCopy, &stateId);
            if (stateVote > currMax && stateVote > 0){//new record!!
                currTopStateId = stateId;
                currMax = stateVote;
            } else if (stateVote == currMax && stateId < currTopStateId ){//two states with same votes: Get the lowest stateId
                currTopStateId = stateId;
            }
        }
        if (currMax == INI_STATE_SCORE){// no more votes
            mapDestroy(votesCopy);
            return topVotes;
        }
        topVotes[i] = currTopStateId ;
        MapResult remTopState = mapRemove(votesCopy, &currTopStateId);//removing the max state
        if (remTopState != MAP_SUCCESS){
            mapDestroy(votesCopy);
            return NULL;
        }
    }
    mapDestroy(votesCopy);
    return topVotes;
}

void InitialArray(int * arr, int size){
    if (!arr) return;
    for (int i = 0; i < size; i++){
        arr[i] = DUMMY_STATE_ID;
    }
}

ListElement stringCopy(ListElement str ){
    if (!str) return NULL;
    return (ListElement)CopyName((char*)str);
}

char * GetFriendsString(State s1, State s2){
    if (!s1 || !s2) return NULL;
    const char* name1 = s1->stateName;
    const char* name2 = s2->stateName;
    size_t strLen1 = strlen(name1);
    size_t strLen2 = strlen(name2);
    size_t catLen = strlen(" - ");
    //char* name2 = (char*)CopyName(s2->stateName);
    if (!name1 || !name2){
        return NULL;
    }
    char * friendString = malloc(catLen + strLen1 + strLen2 + sizeof(char)/*for '\0' */);
    if (!friendString) return NULL;
    if (strcmp(name1, name2) < 0){//s1 < s2
        strcpy(friendString, name1);
        strcat(friendString, " - ");
        strcat(friendString, name2);
    }else {//s1 >= s2
        strcpy(friendString, name2);
        strcat(friendString, " - ");
        strcat(friendString, name1);
    }
    return friendString;
}

bool AlreadyFriends( List friends, char * friendsString){
    if (!friends || !friendsString) return false;
    LIST_FOREACH(char** ,iter ,friends){
        if (strcmp((const char*)listGetCurrent(friends),(const char*)friendsString) == 0){//equal
            return true;
        }
    }
    return false;
}

bool IsFriendly(State currState, State state) {
    if(!currState || !state) return NULL;
    int topVotes = DUMMY_STATE_ID;
    int currTopVoteId  = GetTopVotes(currState->votes, &topVotes, 1)[0];//Gets highest vote's stateId
    int stateTopVoteId = GetTopVotes(    state->votes, &topVotes, 1)[0];
    return (currState->id == stateTopVoteId &&
            state->id     == currTopVoteId);
}

double RoundScore(double score){
    if (!score) return 0;
    int midScore = (int)(100 * score);
    return (double)midScore/100;
}
void RemoveJudgesOfState(Eurovision eurovision, int stateId){
    if (!eurovision) return;
    bool judgeRemoved = false;
    int judgeId = 0 ;
    int * iter = mapGetFirst(eurovision->judges);
    while( iter ){

        /*if (judgeRemoved){
            judgeId = *(int*)mapGetFirst(eurovision->judges);//mapRemove resets iterator,
            // and now it points to the second key element
            judgeRemoved = false;
        }else {*/
        judgeId = *iter;
        Judge judge = (Judge)mapGet(eurovision->judges, &judgeId);
        for(int i = 0; i < JUDGE_RANK_LEN; i++){
            if (judge->ranking[i] == stateId){//A sinner!! finish him!!
                mapRemove(eurovision->judges, &judgeId);
                judgeRemoved = true;
                break;//the current judge doesnt exist anymore
            }
        }if (judgeRemoved){
            iter =  (int*)mapGetFirst(eurovision->judges);//mapRemove resets iterator
            judgeRemoved = false;
        }else{
            iter = mapGetNext(eurovision->judges);
        }
    }

}
EurovisionResult OUT_OF_MEMORY_ERROR(Eurovision eurovision){
    eurovisionDestroy(eurovision);
    return EUROVISION_OUT_OF_MEMORY;
}
//====================================================
