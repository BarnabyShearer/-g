/*
 * Powder feed and leveler
 *
 * Copyright 2013 <b@Zi.iS>
 * License GPLv2
 */

#include <stdio.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "main.h"
#include "uart.h"
#include "gcode.h"

int main(void) {

    uart_init();
    stdout = &uart_output;
    //stdin  = &uart_input;

    puts("Please enter G-code");

    char* line;

    while(1) {
        if((line = next_line()) != NULL) {
            push_line(line);
        }
        sleep_cpu();
    }
    return 0;
}
