#include <string.h>
#include <stdlib.h>
#include "map.h"
#include "euro_utility.h"
#include "state.h"

/*create a state*/
State CreateState(int id, const char* stateName, const char* songName) {
    State state = malloc(sizeof(*state));
    if (!state) return NULL;
    state->finalScore = 0;
    state->id = id;
    state->stateName = stateName;
    state->songName = songName;
    state->votes = mapCreate(CopyInt, CopyInt,
                             free, free, CompareInts);
    if (!state->votes||!state->songName||!state->stateName) {
        /*free((int*)state->id);*/
        free((char*)state->songName);
        free((char*)state->stateName);
        mapDestroy(state->votes);
        free(state);
        return NULL;
    }
    return state;
}
void StateDestroy(State state) {
    if (!state) return;
    /*free((int*)state->id);*/
    free((char*)state->songName);
    free((char*)state->stateName);
    mapDestroy(state->votes);
    free(state);
}
State StateCopy(State state) {
    if (!state) return NULL;
    const char* stateNameCopy = CopyName(state->stateName);
    const char* songNameCopy = CopyName(state->songName);
    State newstate = CreateState(state->id, stateNameCopy, songNameCopy);
    if (!stateNameCopy || !songNameCopy || !newstate) {
        free((char*)stateNameCopy);
        free((char*)songNameCopy);
        StateDestroy(newstate);
        return NULL;
    }
    mapDestroy(newstate->votes);
    newstate->votes = mapCopy(state->votes);
    newstate->finalScore = state->finalScore;
    return newstate;
}
