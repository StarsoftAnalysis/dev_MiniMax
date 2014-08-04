#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "ttt.h"


e_mark get_opposite(e_mark mark) {
	return mark == CIRCLE ? CROSS : CIRCLE;
}


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


/* void generate_nodes(struct node *p, uint8_t d, e_mark m) { */
/* 	struct node *n = NULL; */
/* 	uint8_t cnt = 8; */
/* 	uint8_t x = 0, y = 0; */
/* 	uint8_t mark = get_opposite(m); */
/*  */
/* 	while (cnt) { */
/* 		x = cnt/3; */
/* 		y = cnt%3; */
/* 		cnt--; */
/*  */
/* 		if (EMPTY != get_grid(p,x,y)) continue; */
/*  */
/* 		if (!(n = malloc(sizeof(struct node)))) { */
/* 			fprintf(stderr, "out of memory"); */
/* 			exit; */
/* 		} */
/*  */
/* 		memset(n, 0x00, sizeof(struct node)); */
/* 		memcpy(n->grid, p->grid, 3); */
/* 		set_grid(n, x, y, mark); */
/* 		p->children[p->_ch_no++] = n; */
/* 		g_nodes_cnt++; */
/*  */
/* 		// recursively generate the rest of nodes if the current one isn't  */
/* 		// the winning one */
/* 		if (!evaluate_node(n, m) && (d<MAX_DEPTH)) generate_nodes(n, d+1, mark); */
/* 	}  */
/* } */

uint8_t best_moves[9] = {0x00};
uint8_t best_move = 0x00;
int8_t ccc = 0;


int8_t nega_max(grid_t *g, int d, e_mark player) {

	int8_t max = MIN_SCORE;
	int8_t score = 0;
	int8_t best = 0;
	e_mark opponent = get_opposite(player);

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
		g->g[i] = EMPTY;

		if (score > max) {
			max = score;
			if (d==0) best_moves[ccc++] = i;
		}
	}

	// if depth == 0
	if (d == 0) {
		if (ccc) {
			best_move = best_moves[rand() % ccc];
		}
		else {
			best_move = 0;

			// we are in trouble - no best move has been found
			// I will just guess one
			printf("warning - guessing best move\n");
			while (g->g[best_move] != EMPTY) {
				best_move = rand() % GRID_SIZE_TOTAL;
			} // while
		}
		ccc = 0;
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
	char i = getchar();
	if (i == 0x0a) {
		i = getchar();
	}

	i = i - '0';
	if (i >= 0 && i<= 9) {
		g->g[i] = m;
	}
}


int main(int argc, char *argv[])
{
	grid_t grid;
	memset(&grid, 0x00, sizeof(grid_t));
	srand(time(NULL));

	while (1) {
		get_human_move(&grid, CROSS);
		dump_grid(&grid, 0);

		printf("CPU\n");
		nega_max(&grid, 0, CIRCLE);
		grid.g[best_move] = CIRCLE;
		dump_grid(&grid, 1);

		// break the loop if end of game
		if (evaluate_node(&grid, CROSS) || board_full(&grid)) {
			break;
		}
	}

	return 0;
}
