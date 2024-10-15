#include "rfid.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define DEBUG_UART &huart3

uint8_t status;
uint8_t str[MAX_LEN];
uint8_t serNum[5];

Card stored_data[MAX_ENTRIES];
int data_count = 0;

volatile uint8_t uartBuffer[UART_BUFFER_SIZE];
volatile uint8_t uartBufferIndex = 0;
volatile uint8_t uartBufferReady = 0;

extern const char *ranks[];
extern const int rankValues[];

static size_t word_len(char *s, char c);
static char *append_word(char *s, char c);
static size_t count_word(char const *s, char c);
static char **ft_split(char const *s, char c);

void debug_print(const char *message)
{
    HAL_UART_Transmit(DEBUG_UART, (uint8_t *)message, strlen(message), HAL_MAX_DELAY);
    HAL_UART_Transmit(DEBUG_UART, (uint8_t *)"\r\n", 2, HAL_MAX_DELAY);
}

void print_card_data(uint8_t *data, uint8_t blockAddr)
{
    char stringData[17];
    memcpy(stringData, data, 16);
    stringData[16] = '\0';

    for (int i = 0; i < 16; i++)
    {
        if (stringData[i] < 32 || stringData[i] > 126)
        {
            stringData[i] = ' ';
        }
    }

    debug_print(stringData);
}

void bytes_to_hex_string(uint8_t *data, int length, char *output)
{
    for (int i = 0; i < length; i++)
    {
        sprintf(output + (i * 2), "%02X", data[i]);
    }
    output[length * 2] = '\0';
}

void bytes_to_ascii_string(uint8_t *data, int length, char *output)
{
    int j = 0;
    for (int i = 0; i < length; i++)
    {
        if (isprint(data[i]))
        {
            output[j++] = (char)data[i];
        }
        else
        {
            output[j++] = '.';
        }
    }
    output[j] = '\0';
}

void bytes_to_decimal_string(uint8_t *data, int length, char *output)
{
    int offset = 0;
    for (int i = 0; i < length; i++)
    {
        offset += sprintf(output + offset, "%d ", data[i]);
    }
    if (offset > 0)
        output[offset - 1] = '\0';
}

void print_symbol(const uint32_t *symbol)
{
    char line[33];
    char newline[3] = "\r\n";

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            line[j] = (symbol[i] & (1 << (31 - j))) ? '1' : '0';
        }
        line[32] = '\0';

        HAL_UART_Transmit(&huart3, (uint8_t *)line, 32, HAL_MAX_DELAY);
        HAL_UART_Transmit(&huart3, (uint8_t *)newline, 2, HAL_MAX_DELAY);
    }
}

static size_t word_len(char *s, char c)
{
    size_t i = 0, len = 0;
    while (s[i] && s[i] != c)
    {
        len++;
        i++;
    }
    return len;
}

static char *append_word(char *s, char c)
{
    size_t len = word_len(s, c);
    char *buffer = malloc(sizeof(char) * (len + 1));
    if (!buffer)
        return NULL;

    size_t i = 0, j = 0;
    while (i < len)
    {
        if (s[i] != c)
            buffer[j++] = s[i];
        i++;
    }
    buffer[j] = '\0';
    return buffer;
}

static size_t count_word(char const *s, char c)
{
    size_t i = 0, count = 0;
    while (s[i])
    {
        while (s[i] && s[i] == c)
            i++;
        if (s[i])
            count++;
        while (s[i] && s[i] != c)
            i++;
    }
    return count;
}

static char **ft_split(char const *s, char c)
{
    if (!s)
        return NULL;

    char **buffer = malloc(sizeof(char *) * (count_word(s, c) + 1));
    if (!buffer)
        return NULL;

    size_t i = 0, j = 0;
    while (s[i])
    {
        while (s[i] && s[i] == c)
            i++;
        if (s[i])
            buffer[j++] = append_word((char *)&s[i], c);
        while (s[i] && s[i] != c)
            i++;
    }
    buffer[j] = NULL;
    return buffer;
}

void freeCard(Card *card)
{
    if (card != NULL)
    {
        free(card->rank);
        free(card);
    }
}

Card *parseCardFromString(const char *cardString)
{
    Card *card = malloc(sizeof(Card));
    char **split = ft_split(cardString, ' ');

    switch (split[0][0])
    {
        case 'H': card->suit = "Hearts"; break;
        case 'D': card->suit = "Diamonds"; break;
        case 'C': card->suit = "Clubs"; break;
        case 'S': card->suit = "Spades"; break;
        default: card->suit = "Unknown"; break;
    }

    card->rank = split[1];

    int getRankIndex(const char *rank)
    {
        for (int i = 0; i < NUM_RANKS; i++)
        {
            if (ranks[i][0] == rank[0])
                return i;
        }
        return -1;
    }

    int rankIndex = getRankIndex(split[1]);
    if (rankIndex != -1)
    {
        card->rankValue = rankValues[rankIndex];
    }
    else
    {
        debug_print("Invalid rank");
        card->rankValue = -1;
    }

    if (strcmp(card->suit, "Hearts") == 0 || strcmp(card->suit, "Diamonds") == 0)
    {
        card->symbol = (strcmp(card->suit, "Hearts") == 0) ? HEART : DIAMOND;
        card->colour = RED;
    }
    else
    {
        card->symbol = (strcmp(card->suit, "Clubs") == 0) ? CLUB : SPADE;
        card->colour = BLACK;
    }

    return card;
}

void display_status_led(StatusType status)
{
    if (status == STATUS_SUCCESS)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET);
    }

    HAL_Delay(1000);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10 | GPIO_PIN_11, GPIO_PIN_RESET);
}

void processRFID(uint8_t maxEntries, Card deck[])
{
    char message[100];
    uint8_t status;
    uint8_t str[MAX_LEN];
    uint8_t serNum[5];
    uint8_t isFinish = 0;

    debug_print("Initializing RFID Reader...");
    MFRC522_Init();
    debug_print("RFID Reader Started");
    debug_print("Waiting for card...");

    while (!isFinish)
    {
        status = MFRC522_Request(PICC_REQIDL, str);

        if (status == MI_OK)
        {
            debug_print("Card detected");

            status = MFRC522_Anticoll(serNum);
            if (status == MI_OK)
            {
                uint8_t readData[16];
                status = MFRC522_Read(4, readData);
                if (status == MI_OK)
                {
                    snprintf(message, sizeof(message), "Reading Data... (%d)", data_count + 1);
                    debug_print(message);

                    char hex_string[33];
                    char ascii_string[17];

                    for (int i = 0; i < 16; i++)
                    {
                        sprintf(hex_string + (i * 2), "%02X", readData[i]);
                        ascii_string[i] = (readData[i] >= 32 && readData[i] <= 126) ? (char)readData[i] : '.';
                    }
                    ascii_string[16] = '\0';

                    char formatted_ascii[4] = "   ";
                    formatted_ascii[0] = ascii_string[0];
                    formatted_ascii[2] = ascii_string[2];
                    formatted_ascii[3] = '\0';

                    int is_duplicate = 0;
                    for (int i = 0; i < data_count; i++)
                    {
                        char **split = ft_split(formatted_ascii, ' ');
                        if (split && split[0] && split[1])
                        {
                            if (strcmp(stored_data[i].suit, split[0]) == 0 && strcmp(stored_data[i].rank, split[1]) == 0)
                            {
                                is_duplicate = 1;
                                debug_print("Duplicate card detected");
                                for (int j = 0; split[j] != NULL; j++)
                                {
                                    free(split[j]);
                                }
                                free(split);
                                break;
                            }
                            for (int j = 0; split[j] != NULL; j++)
                            {
                                free(split[j]);
                            }
                            free(split);
                        }
                        else
                        {
                            debug_print("Error: Invalid card data format");
                            if (split)
                            {
                                for (int j = 0; split[j] != NULL; j++)
                                {
                                    free(split[j]);
                                }
                                free(split);
                            }
                            is_duplicate = 1;
                            break;
                        }
                    }

                    if (is_duplicate)
                    {
                        debug_print("Duplicate card detected");
                        display_status_led(STATUS_FAILURE);
                    }
                    else
                    {
                        if (data_count < maxEntries)
                        {
                            Card *parsedCard = parseCardFromString(formatted_ascii);
                            if (parsedCard != NULL)
                            {
                                memcpy(&stored_data[data_count], parsedCard, sizeof(Card));
                                display_status_led(STATUS_SUCCESS);
                                data_count++;

                                if (data_count == maxEntries)
                                {
                                    debug_print("Array full. Displaying all entries:");
                                    for (int i = 0; i < maxEntries; i++)
                                    {
                                        snprintf(message, sizeof(message), "Card %d: %s of %s", i + 1, stored_data[i].rank, stored_data[i].suit);
                                        debug_print(message);
                                        memcpy(&deck[i], &stored_data[i], sizeof(Card));
                                    }
                                    isFinish = 1;
                                    data_count = 0;
                                }
                                free(parsedCard);
                            }
                            else
                            {
                                debug_print("Failed to parse card data");
                                display_status_led(STATUS_FAILURE);
                            }
                        }
                    }
                }
                else
                {
                    debug_print("Failed to read data from card");
                    display_status_led(STATUS_FAILURE);
                }

                MFRC522_Halt();
            }
            else
            {
                debug_print("Anticollision failed");
                display_status_led(STATUS_FAILURE);
            }

            HAL_Delay(1000);
        }

        HAL_Delay(100);
    }
}
