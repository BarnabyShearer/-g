/*
 * Copyright 2013 <b@Zi.iS>
 * License GPLv2
 */

int uart_putchar(char c, FILE *stream);
//int uart_getchar(FILE *stream);
char* next_line();

void uart_init(void);

struct rx_ring;
struct tx_ring;

FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
//FILE uart_input = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);

