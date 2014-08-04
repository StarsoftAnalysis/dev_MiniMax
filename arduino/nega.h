#ifndef __TTT_H__
#define __TTT_H__

#include <stdint.h>
#include "pca.h"

/**
 * @brief maximum tree generation/evaluation depth
 */
#define MAX_DEPTH 9

/**
 * @brief minimal and maximal evaluation scores
 */
#define MIN_SCORE -100
#define MAX_SCORE 100


// board dimensions
#define GRID_SIZE_X 3
#define GRID_SIZE_Y 3
#define GRID_SIZE_TOTAL (GRID_SIZE_X * GRID_SIZE_Y)


/**
 * @brief symbols
 */
typedef enum _e_mark {
	EMPTY = 0,
	CIRCLE,
	CROSS,
	LAST
} e_mark;


/**
 * @brief the game play board
 */
typedef struct { uint8_t g[GRID_SIZE_TOTAL]; } grid_t;



/// board access functions
#define get_grid(__g, __x, __y) __g->g[ (__x) + ((__y)*GRID_SIZE_X) ]
#define set_grid(__g, __x, __y, __m) __g->g[ (__x) + ((__y)*GRID_SIZE_X) ] = __m 

e_mark get_opposite(e_mark mark);
uint8_t board_full(grid_t *g);
uint8_t check_win(grid_t *g, e_mark m);
uint8_t count_potential_wins(grid_t *g, e_mark m);
int8_t evaluate_node(grid_t *g, e_mark player);

void dump_grid(grid_t *g, uint8_t d);
void get_human_move(grid_t *g, e_mark m);
uint8_t get_best_move(void);
int8_t nega_max(grid_t *g, uint8_t d, uint8_t player);

#endif /* __TTT_H__ */
