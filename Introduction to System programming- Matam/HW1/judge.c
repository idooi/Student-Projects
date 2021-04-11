#include <stdio.h>
#include <stdlib.h>
#include "judge.h"
#include "euro_utility.h"

//Implementation Section


Judge JudgeCreate(int judgeId, const char* judgeName, int *judgeResults) {
    if (!judgeName || !judgeResults) return NULL;
    Judge judge = malloc(sizeof(*judge));
    if (!judge) {
        return NULL;
    }
    judge->id = judgeId;
    judge->name = judgeName;
    judge->ranking = judgeResults;
    return judge;
}

void JudgeDestroy(Judge judge) {
    if (!judge) return;// MEH
    free((char*)judge->name);
    free(judge->ranking);
    free (judge);
}

int * CopyArray(int *arr, int size) {
    if (!arr || size < 1 ) return NULL;
    int *newArr = malloc(size*sizeof(int));
    if (!newArr) return NULL;
    for (int i = 0; i < size; i++) {
        newArr[i] = arr[i];
    }
    return newArr;
}

Judge JudgeCopy(Judge judge){
    if (!judge) return NULL;
    int* rankingCopy = CopyArray(judge->ranking, JUDGE_RANK_LEN);
    const char * nameCopy = CopyName(judge->name);
    Judge newJudge = JudgeCreate(judge->id, nameCopy, rankingCopy);
    if (!nameCopy || !newJudge|| !rankingCopy){
        free(rankingCopy);
        free((char*)nameCopy);
        free(newJudge);
        return NULL;
    }
    return newJudge;
}

int RankIndexToScore(int index) {//converts rank index of judgeVotes array into actual score
    if (index > JUDGE_RANK_LEN - 1 || index < 0) return 0;//invalid
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
