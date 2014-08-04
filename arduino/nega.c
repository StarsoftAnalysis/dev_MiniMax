#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "nega.h"

/**
 * @brief printable symbols
 */
static const char symbols[LAST] = {
	'-',
	'O',
	'X'
};



static uint8_t _best_moves[9] = {0x00};
static uint8_t _best_move = 0x00;
static uint8_t _best_moves_cnt = 0;

#define get_opposite(__mark) (__mark == CIRCLE ? CROSS : CIRCLE)

uint8_t check_win(grid_t *g, e_mark m) {

	for (uint8_t x = 0; x<3; x++) {
		if ((get_grid(g,x,0) == get_grid(g,x,1)) && 
			(get_grid(g,x,0) == get_grid(g,x,2)) &&
			(get_grid(g,x,0) == m)) {
			// winning column found
			return 1;
		}
	}
	
	for (uint8_t y = 0; y<3; y++) {

		if ((get_grid(g,0,y) == get_grid(g,1,y)) && 
			(get_grid(g,0,y) == get_grid(g,2,y)) &&
			(get_grid(g,0,y) == m)) {
			// winning row found
			return 1;
		}
	}

	if ((get_grid(g,0,0) == get_grid(g,1,1)) && 
		(get_grid(g,0,0) == get_grid(g,2,2)) &&
		(get_grid(g,0,0) == m)) {
		// winning left diagonal
		return 1;
	}

	if ((get_grid(g,2,0) == get_grid(g,1,1)) && 
		(get_grid(g,2,0) == get_grid(g,0,2)) &&
		(get_grid(g,2,0) == m)) {
		// winning right diagonal
		return 1;
	}

	return 0;
}


uint8_t count_potential_wins(grid_t *g, e_mark m) {
	uint8_t res = 0;
	e_mark o = get_opposite(m);

	for (uint8_t x = 0; x<3; x++) {
		uint8_t r = 0;
		for (uint8_t y = 0; y<3; y++) {
			if (get_grid(g,x,y) == o) r++;
		}
		if (r == 0) res++;
	}

	for (uint8_t y = 0; y<3; y++) {
		uint8_t c = 0;
		for (uint8_t x = 0; x<3; x++) {
			if (get_grid(g,x,y) == o) c++;
		}
		if (c == 0) res++;
	}


	if ((get_grid(g,0,0) != o) && 
		(get_grid(g,1,1) != o) && 
		(get_grid(g,2,2) != o)) {
		// winning left diagonal
		res++;
	}

	if ((get_grid(g,2,0) != o) && 
		(get_grid(g,1,1) != o) && 
		(get_grid(g,0,2) != o)) {
		// winning right diagonal
		res++;
	}
	
	return res;
}


uint8_t board_full(grid_t *g) {
	for (uint8_t x = 0; x < 3; x++) {
		for (uint8_t y = 0; y < 3; y++) {
			if (get_grid(g, x, y) == EMPTY) return 0;
		}
	}
	return 1;
}


int8_t evaluate_node(grid_t *g, e_mark player) {
	e_mark opponent = get_opposite(player);
	int8_t r = 0;

	if (check_win(g, player)) {
		r = MAX_SCORE;
	}
	else if (check_win(g, opponent)) {
		r = MIN_SCORE;
	}
	
	return r;
}


int8_t nega_max(grid_t *g, uint8_t d, uint8_t player) {

	int8_t max = MIN_SCORE;
	int8_t score = 0;
	e_mark opponent = get_opposite(player);

#ifdef DEBUG
	printf("depth: %d\n", d);
#endif

	// evaluate the board
	if (board_full(g) || 
		(score = evaluate_node(g, player)) ||
		(d >= MAX_DEPTH)) {
		int8_t w = (count_potential_wins(g, player) - count_potential_wins(g, opponent));
		return score ? score : w;
	}

	// board generation and evaluation in place
	for (uint8_t i = 0; i < GRID_SIZE_TOTAL; i++) {

		// skip occupied fields
		if (g->g[i] != EMPTY) continue;

		// create new board
		g->g[i] = player;
		score = -1 * nega_max(g, d + 1, opponent);

		// restore the original board
		g->g[i] = EMPTY;

#ifdef DEBUG
		printf("score: %d\n", score);
#endif

		if (score > max) {
			max = score;
			if (d==0) _best_moves[_best_moves_cnt++] = i;
		}
	}

	// if came back to the starting node
	if (d == 0) {
		if (_best_moves_cnt) {
			_best_move = _best_moves[rand() % _best_moves_cnt];
		}
		else {
			_best_move = 0;
			// we are in trouble - no best move has been found
			// I will just guess one
#ifdef DEBUG
			printf("warning - guessing best move\n");
#endif
			while (g->g[_best_move] != EMPTY) {
				_best_move = rand() % GRID_SIZE_TOTAL;
			} // while
		}
		_best_moves_cnt = 0;
	}

	return max;
}


void dump_grid(grid_t *g, uint8_t d) {
	for (uint8_t y = 0; y < 3; y++) {
		for (uint8_t i = 0; i<d; i++) printf(" ");
		printf("%c %c %c\n", 
				symbols[get_grid(g,0,y)],
				symbols[get_grid(g,1,y)],
				symbols[get_grid(g,2,y)]);
	}
}


void get_human_move(grid_t *g, e_mark m) {
	uint8_t i = 0x00;
	while (!serial_getc(&i));
	i = i - '0';
	if (i >= 0 && i<= 9) {
		g->g[i] = m;
	}
}

uint8_t get_best_move() {
	return _best_move;
}
