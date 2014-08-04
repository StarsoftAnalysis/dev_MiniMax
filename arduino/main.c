#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pca.h"
#include "nega.h"

int main(void)
{
	grid_t grid;
	int8_t result = 0;

	serial_init(E_BAUD_4800);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();
	serial_install_stdio();

	while (1) {
		printf("Let's play a game...\n");
		memset(&grid, 0x00, sizeof(grid_t));
		dump_grid(&grid, 0);

		while (1) {
			get_human_move(&grid, CROSS);
			dump_grid(&grid, 0);

			printf("CPU\n");
			nega_max(&grid, 0, CIRCLE);
			grid.g[get_best_move()] = CIRCLE;
			dump_grid(&grid, 1);

			// break the loop if end of game
			if ((result = evaluate_node(&grid, CROSS))) {
				printf("%c won\n", result == MAX_SCORE ? 'X' : 'O');
				break;
			}

			if (board_full(&grid)) {
				printf("TIE\n");
				break;
			}
		}
	} // main while

	return 0;
}
