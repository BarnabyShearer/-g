/*
 * Simple stdio style TX with a small ring buffer.
 * RX tokenizes into line buffers.
 *
 * Copyright 2013 <b@Zi.iS>
 * License GPLv2
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#define BAUD 250000
#include <util/setbaud.h>

#define UART_TX_BUFFER_SIZE 64
#define UART_RX_BUFFERS     4
#define UART_RX_BUFFER_SIZE 64

struct tx_ring {
    int buffer[UART_TX_BUFFER_SIZE];
    int start;
    int end;
};

struct rx_ring {
    char buffer[UART_RX_BUFFERS][UART_RX_BUFFER_SIZE];
    int start;
    int end;
    int pointer;
};

static struct tx_ring tx_buffer;
static struct rx_ring rx_buffer;

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); //8-bit
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);   //Enable

    sei();
}

int uart_putchar(char c, FILE *stream) {
    if (c == '\n') {
        uart_putchar('\r', stream);
    }
    int write_pointer = (tx_buffer.end + 1) % UART_TX_BUFFER_SIZE;
    if (write_pointer != tx_buffer.start) {
        tx_buffer.buffer[tx_buffer.end] = c;
        tx_buffer.end = write_pointer;
        UCSR0B |= _BV(UDRIE0);
    }
    return 0;
}
const char* next_line() {
    if(rx_buffer.start != rx_buffer.end) {
        const char* buf = rx_buffer.buffer[rx_buffer.start];
        rx_buffer.start = (rx_buffer.start + 1) % UART_RX_BUFFERS;
        return buf;
    } else {
        return 0;
    }
}

ISR(USART_RX_vect) {
    int b = UDR0;
    if(b == '\n') {
        rx_buffer.buffer[rx_buffer.end++][rx_buffer.pointer] = 0;
        rx_buffer.pointer = 0;
        rx_buffer.end %= UART_RX_BUFFERS;
    } else {
        if(rx_buffer.pointer >= UART_RX_BUFFER_SIZE) {
            //OVERFLOW
            rx_buffer.buffer[rx_buffer.end][0] = 0;
        } else {
            rx_buffer.buffer[rx_buffer.end][rx_buffer.pointer++] = b;
        }
    }
}

ISR(USART_UDRE_vect) {
    if (tx_buffer.start != tx_buffer.end) {
        UDR0 = tx_buffer.buffer[tx_buffer.start];
        tx_buffer.start = (tx_buffer.start + 1) % UART_TX_BUFFER_SIZE;
    } else {
        UCSR0B &= ~_BV(UDRIE0);
    }
}
