#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAX_SCORE 100
#define MIN_SCORE -100

static uint32_t g_nodes_cnt = 0;

typedef enum _e_mark {
	EMPTY = 0,
	CIRCLE,
	CROSS,
	LAST
} e_mark;


const char symbols[LAST] = {
	'-',
	'O',
	'X'
};


struct node {
	struct node *children[8];
	uint8_t _ch_no;

	// board fields
	uint8_t f[3][3];
};


e_mark get_opposite(e_mark mark) {
	return mark == CIRCLE ? CROSS : CIRCLE;
}


uint8_t check_win(struct node *n, e_mark m) {

	for (uint8_t x = 0; x<3; x++) {

		if ((n->f[x][0] == n->f[x][1]) && 
				(n->f[x][0] == n->f[x][2]) &&
				(n->f[x][0] == m)) {
			// winning column found
			return 1;
		}
	}
	
	for (uint8_t y = 0; y<3; y++) {

		if ((n->f[0][y] == n->f[1][y]) && 
				(n->f[0][y] == n->f[2][y]) &&
				(n->f[0][y] == m)) {
			// winning row found
			return 1;
		}
	}

	if ((n->f[0][0] == n->f[1][1]) && 
			(n->f[0][0] == n->f[2][2]) &&
			(n->f[0][0] == m)) {
		// winning left diagonal
		return 1;
	}

	if ((n->f[2][0] == n->f[1][1]) && 
			(n->f[2][0] == n->f[0][2]) &&
			(n->f[2][0] == m)) {
		// winning right diagonal
		return 1;
	}

	return 0;
}


uint8_t count_wins(struct node *n, e_mark m) {
	uint8_t res = 0;
	e_mark o = get_opposite(m);

	for (uint8_t x = 0; x<3; x++) {
		uint8_t r = 0;
		for (uint8_t y = 0; y<3; y++) {
			if (n->f[x][y] == o) r++;
		}

		if (r == 0) res++;
	}

	for (uint8_t y = 0; y<3; y++) {
		uint8_t c = 0;
		for (uint8_t x = 0; x<3; x++) {
			if (n->f[x][y] == o) c++;
		}

		if (c == 0) res++;
	}


	if ((n->f[0][0] != o) && 
			(n->f[1][1] != o) && 
			(n->f[2][2] != o)) {
		// winning left diagonal
		res++;
	}

	if ((n->f[2][0] != o) && 
			(n->f[1][1] != o) && 
			(n->f[2][0] != o)) {
		// winning right diagonal
		res++;
	}
	
	return res;
}


uint8_t board_full(struct node *n) {
	for (uint8_t x = 0; x < 3; x++) {
		for (uint8_t y = 0; y < 3; y++) {
			if (n->f[x][y] != EMPTY) return 0;
		}
	}
	return 1;
}


int8_t evaluate_node(struct node *n, e_mark player) {
	e_mark opponent = get_opposite(player);
	int8_t r = 0;

	if (check_win(n, player)) {
		r = MAX_SCORE;
	}
	else if (check_win(n, opponent)) {
		r = MIN_SCORE;
	}
	
	return r;
}


void generate_nodes(struct node *p, e_mark m) {
	struct node *n = NULL;
	uint8_t cnt = 8;
	uint8_t x = 0, y = 0;
	uint8_t mark = get_opposite(m);

	while (cnt) {
		x = cnt/3;
		y = cnt%3;
		cnt--;

		if (EMPTY != p->f[x][y]) continue;

		if (!(n = malloc(sizeof(struct node)))) {
			fprintf(stderr, "out of memory");
			exit;
		}

		memset(n, 0x00, sizeof(struct node));
		memcpy(n->f, p->f, 9);
		n->f[x][y] = mark;
		p->children[p->_ch_no++] = n;
		g_nodes_cnt++;

		// recursively generate the rest of nodes if the current one isn't 
		// the winning one
		if (!check_win(n,m) && !check_win(n, mark))
			generate_nodes(n, mark);
	} 
}

struct node *best_moves[9] = {0x00};
struct node *best_move = NULL;
int8_t ccc = 0;

e_mark active;

int8_t nega_max(struct node *n, int d, e_mark player) {

	int8_t max = MIN_SCORE;
	int8_t score = max;
	int8_t best = 0;
	e_mark opponent = get_opposite(player);

#define MAX_DEPTH 9

	if (board_full(n) || 
			(score = evaluate_node(n, player)) ||
			(d == MAX_DEPTH)) {
		return score ? score : (count_wins(n, player) - count_wins(n, opponent));
	}
	
	for (uint8_t i = 0; i < n->_ch_no; i++) {
		if (n->children[i])
			score = -1 * nega_max(n->children[i], d + 1, opponent);

		if (score >= max) {
			max = score;
			if (d==0) best_moves[ccc++] = n->children[i];
		}
	}

	if (d == 0) {
		best_move = best_moves[rand() % ccc];
		ccc = 0;
	}

	return max;
}



void dump_node(struct node *n, uint8_t d) {
	for (uint8_t y = 0; y < 3; y++) {
		for (uint8_t i = 0; i<d; i++) printf(" ");
		printf("%c | %c | %c\n", 
				symbols[n->f[0][y]],
				symbols[n->f[1][y]],
				symbols[n->f[2][y]]);
	}
}


void dump_nodes(struct node *n, uint8_t d) {
	for (uint8_t y = 0; y < 3; y++) {
		for (uint8_t i = 0; i<d; i++) printf(" ");
		printf("%c %c %c\n", 
				symbols[n->f[0][y]],
				symbols[n->f[1][y]],
				symbols[n->f[2][y]]);
	}

	for (uint8_t i = 0; i < n->_ch_no; i++) 
		dump_nodes(n->children[i], d + 1);

	printf("\n");
}




struct node* get_human_move(struct node *n, e_mark m) {
	char i = getchar();

	if (i == 0x0a) {
		i = getchar();
	}

	i = i - '0';

	if (i >= 0 && i<= 9) {
		n->f[i/3][i%3] = m;
	}

	return n;
}


#define _GNU_SOURCE
#include <fenv.h>


int main(int argc, char *argv[])
{
	struct node root;
	memset(&root, 0x00, sizeof(struct node));

	struct node *n = &root;
	srand(time(NULL));

	feenableexcept(FE_INVALID   | 
			FE_DIVBYZERO | 
			FE_OVERFLOW  | 
			FE_UNDERFLOW);

	while (1) {
		n = get_human_move(n, CROSS);
		dump_node(n, 0);
		printf("CPU\n");

		n->_ch_no = 0;
		generate_nodes(n, CROSS);
		nega_max(n, 0, CIRCLE);

		dump_node(best_move, 1);
		n = best_move;

		if (evaluate_node(n, CROSS)) {
			break;
		}
	}




	/* dump_node(&root, 0); */
	/* printf("Nodes: %d\n", g_nodes_cnt); */

	return 0;
}
