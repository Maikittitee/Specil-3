#ifndef RFID_H
#define RFID_H

#include "stm32f7xx_hal.h"
#include "rc522.h"
#include <stdint.h>
#include "poker.h"

#define MAX_LEN 16
#define MAX_ENTRIES 10
#define NUM_RANKS 13
#define NUM_SUITS 4
#define BLACK 0x0000
#define RED 0xF800
#define UART_BUFFER_SIZE 100
#define MAX_DATA_SIZE 16

typedef enum {
    STATUS_FAILURE = 0,
    STATUS_SUCCESS = 1
} StatusType;

//typedef struct {
//    const char* suit;
//    char* rank;
//    int rankValue;
//    uint32_t* symbol;
//    uint16_t colour;
//} Card;

extern Card stored_data[MAX_ENTRIES];
extern int data_count;
extern volatile uint8_t uartBuffer[UART_BUFFER_SIZE];
extern volatile uint8_t uartBufferIndex;
extern volatile uint8_t uartBufferReady;

void debug_print(const char *message);
void print_card_data(uint8_t *data, uint8_t blockAddr);
void bytes_to_hex_string(uint8_t *data, int length, char *output);
void bytes_to_ascii_string(uint8_t *data, int length, char *output);
void bytes_to_decimal_string(uint8_t *data, int length, char *output);
void print_symbol(const uint32_t *symbol);
Card *parseCardFromString(const char *cardString);
void freeCard(Card *card);
void display_status_led(StatusType status);
void processRFID(uint8_t maxEntries, Card deck[]);

#endif // RFID_H
