#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "pca.h"
#include "nega.h"

#define LCD_STDOUT 0


int main(void)
{
	grid_t grid;
	int8_t result = 0;

#if LCD_STDOUT == 1
	struct dev_pcd8544_ctx lcd;
	struct bus_t spi_bus;
#endif

	serial_init(E_BAUD_4800);	
	serial_install_interrupts(E_FLAGS_SERIAL_RX_INTERRUPT);
	serial_flush();

	serial_install_stdio();

#if LCD_STDOUT == 1
	spi_bus = spi_hw_poll_bus_get();
	lcd.bus = &spi_bus;

	lcd.sce.port = &PORTB;
	lcd.sce.pin = PORTB0;
	lcd.dc.port = &PORTB;
	lcd.dc.pin = PORTB1;
	lcd.res.port = &PORTB;
	lcd.res.pin = PORTB2;

	spi_hw_poll_init(E_SPI_MODE_MASTER, E_SPI_SPEED_F4);
	pcd8544_init(&lcd);
	pcd8544_clrscr(&lcd);
	pcd8544_install_stdout(&lcd);
#endif

	while (1) {
#if LCD_STDOUT == 1
		pcd8544_clrscr(&lcd);
#endif
		printf("Let's play a game...\n");
		memset(&grid, 0x00, sizeof(grid_t));
		dump_grid(&grid, 1);

		while (1) {
			get_human_move(&grid, CROSS);

#if LCD_STDOUT == 1
			pcd8544_clrscr(&lcd);
#endif
			dump_grid(&grid, 1);

			nega_max(&grid, 0, CIRCLE);
			grid.g[get_best_move()] = CIRCLE;

#if LCD_STDOUT == 1
			pcd8544_clrscr(&lcd);
#endif
			printf("CPU\n");
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
