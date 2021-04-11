#ifndef EUROVISION_STATE_H
#define EUROVISION_STATE_H
typedef enum State_t {
    STATE_SUCCESS,
    STATE_OUT_OF_MEMORY,
    STATE_NULL_ARGUMENT,
    STATE_ITEM_ALREADY_EXISTS,
    STATE_ITEM_DOES_NOT_EXIST
} StateResult;
typedef struct state_t {
    int id;
    const char* stateName;
    const char* songName;
    Map votes;
    double finalScore;
}*State;
State CreateState(int id, const char* stateName, const char* songName);
void StateDestroy(State state);
State StateCopy(State state);
#endif
