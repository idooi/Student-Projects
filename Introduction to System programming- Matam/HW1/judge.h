//
// Created by IdoSarel on 04/24/19.
//

#ifndef EUROVISION_JUDGE_H
#define EUROVISION_JUDGE_H
#define JUDGE_RANK_LEN 10
#define FAILURE -1
#define PLACE_1 12
#define PLACE_2 10
#define PLACE_3 8
//Header Section

//Strange Section
typedef enum JudgeResult_t {
    JUDGE_SUCCESS,
    JUDGE_OUT_OF_MEMORY,
    JUDGE_NULL_ARGUMENT,
    JUDGE_ITEM_ALREADY_EXISTS,
    JUDGE_ITEM_DOES_NOT_EXIST
} JudgeResult;

//Implementation Section
typedef struct judge_t *Judge;

/**
* JudgeCreate: Allocates a new empty map.
*
* @param judgeId - int id of the new judge
*
* @param judgeName - char array of name of the new judge
*
* @param judgeResults - an array containing the state ranking of the judge. The array contains stateIds
* 		the map
* @return
* 	NULL - if one of the parameters is NULL or allocations failed.
* 	A new Judge in case of success.
*/

Judge JudgeCreate(int judgeId, const char* judgeName, int *judgeResults);

/**
* JudgeDestroy: Deallocates an existing Judge.
*
* @param judge - Target Judge to be deallocated. If judge is NULL nothing will be
* 		done
*/
void JudgeDestroy(Judge judge);

/**
 *
 * @param arr - an array to be copied
 * @param size - size of the array to be copied
 * @return NULL if allocation failed or invalid size or NULL array input. The copied array otherwise
 */
int * CopyArray(int *arr, int size);

/**
* JudgeCopy: Creates a copy of target Judge.
*
* @param judge - Target Judge.
* @return
* 	NULL if a NULL was sent or a memory allocation failed.
* 	A copy of judge otherwise.
*/
Judge JudgeCopy(Judge judge);

/**
 *  RankIndexToScore: converts index of ranking array to score value according to the contest rules.
 * @param index - the index of the element to be evaluated
 * @return the corresponding score
 */
int RankIndexToScore(int index) ;

struct judge_t {
    int id;
    const char* name;
    int * ranking;
};

#endif //EUROVISION_JUDGE_H
